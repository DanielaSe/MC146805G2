/***************************************************************
 * 
 * Replacement for uP MC146805G2 in Philips 70CD555
 * 
 * www.70cd555.com
 * Daniela Sefzig, Oct.2020
 * 
 *
 * 
 * 
 * * ************************************************************/





#include "TTapeController.h"
#include "Arduino.h"
#include "TPinLayout.h"
#include "TDisplay.h"
#include "TInputs.h"
#include "defines.h"


/************************************************************************
 * constructor
 * 
 * *********************************************************************/
TTapeController::TTapeController(TDisplay *_display, TInputs *_inputs) {


    TrackNumber = 1;
    Programming = false;
    FastWinding = fwNone;
    playing = false;
    ReverseMode = rmNone;
    direction = 1;
    inputs = _inputs;
    lcd = _display;
    lcd->Clear();        
    StartUp = true;
    ClearProgramm();

    pinMode(SET_WIND_LEFT, OUTPUT); 
    pinMode(SET_WIND_LEFT, OUTPUT); 
    pinMode(SET_WIND_RIGHT, OUTPUT);
    pinMode(SET_FAST_WIND, OUTPUT);
    pinMode(SET_FAST_WIND_RELAY, OUTPUT);
    pinMode(SET_CAPSTAN_MOTOR, OUTPUT);
    pinMode(SET_SLIDE_SERVO, OUTPUT);
    pinMode(SET_MUTE, OUTPUT);

    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);
    digitalWrite(SET_FAST_WIND, HIGH);
    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_CAPSTAN_MOTOR, LOW);
    digitalWrite(SET_SLIDE_SERVO, HIGH);
    digitalWrite(SET_MUTE, LOW);

}


/************************************************************************
 * Helper functions
 * 
 * *********************************************************************/
int TTapeController::GetDirection() { return direction;}
bool TTapeController::Playing() { return playing; }
bool TTapeController::Paused() { return paused; }
int TTapeController::InRange(int value, int min, int max)
{
    if (value < min) { value = max; }
    if (value > max) { value = min; }
    return value;
}
bool TTapeController::IsOnRecord() { return StateRecord; }
bool TTapeController::IsOnAutoRecord() { return StateRecord && RecordMode == recAuto; }
bool TTapeController::IsOnSyncRecord() { return StateRecord && RecordMode == recSync; }


void TTapeController::Reset() 
{
    if (playing || FastWinding != fwNone) Stop();
}
void TTapeController::StartCapstan() 
{
    SwitchOffCapstan = 0;
    digitalWrite(SET_CAPSTAN_MOTOR, HIGH);    
}
void TTapeController::StopCapstan() 
{
    SwitchOffCapstan = ms + CAPSTAN_OFF_DELAY;
}


/************************************************************************
 * Play
 * 
 * *********************************************************************/
void TTapeController::Play() 
{
    #ifdef DEBUG
        Serial.println(":Play()");
    #endif
    GetState();
    AutoRestart = false;

    if (lcd->WaitingForInput()) lcd->Clear();

    

    if (paused) {
        digitalWrite(SET_FAST_WIND, HIGH);
        StartCapstan();
        if (direction < 0) {
            digitalWrite(SET_WIND_RIGHT, LOW);
            digitalWrite(SET_WIND_LEFT, HIGH);
        }
        else {
            digitalWrite(SET_WIND_LEFT, LOW);
            digitalWrite(SET_WIND_RIGHT, HIGH);    
        }
        paused = false;
        SetMute(false);
        lcd->Play();
        return;
    }

    if (playing && !FastWinding != fwNone) return;

    if (!PrepareForProgramm && !PlayProgramm && HasProgramm()) {
        StartProgramm();
        return;
    }
    PrepareForProgramm = false;
    
    if (StateSlideServoUp) {   
        Stop();
        GetState();
    }

    if (!playing && RecordMode != recNone) {
        Recording = true;
        ReverseMode = rmNone;
        if (RecordMode == recAuto) {
            AutoRecordTrackNumber = 1;
            lcd->ShowDigit(AutoRecordTrackNumber);
        }
    }

    StartedWithEmptyTape = StateTapeReader;
    #ifdef DEBUG
        Serial.print("StartedWithEmptyTape = "); 
        Serial.println(StartedWithEmptyTape);
    #endif

    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_FAST_WIND, HIGH); 
    digitalWrite(SET_WIND_RIGHT, LOW);
    digitalWrite(SET_WIND_LEFT, LOW);
    StartCapstan();

    if (!StateSlideServoUp) {     
        delay(100);
        PushSlideServo();
        delay(50);
        
    }
    if (direction < 0) {
        digitalWrite(SET_WIND_RIGHT, LOW);
        digitalWrite(SET_WIND_LEFT, HIGH);
    }
    else {
        digitalWrite(SET_WIND_LEFT, LOW);
        digitalWrite(SET_WIND_RIGHT, HIGH);    
    }
    
    SetMute(false);
    lcd->Play();
    FastWinding = fwNone;
    playing = true;
    paused = false;  
    SearchTrack = false;
    SearchTrackNumber = 0;
    TrackFound = false;
    if (ReverseMode != rmNone) {
        RepeatSecondSide = false;
        RewindOneSide = false;
    }



    // Sometimes it can happen that the reader is not able to read the tape
    // at the very beginning because the first part of the tape is not
    // transparent
    IgnoreStateTapeReader = ms + 500;
    // give it some time to gain speed before enable audio
    delay(200);
    SetMute(false);
}





/************************************************************************
 * Stop
 * 
 * *********************************************************************/
void TTapeController::Stop() 
{
    #ifdef DEBUG
        Serial.println(":Stop()");
    #endif



    if (!playing && StateRecord) {
        StartRecordMode();
        return;
    }


    if (!playing && FastWinding == fwNone && !SearchTrack) return;
    GetState();

    SetMute(true);

    if (paused) StartCapstan(); 
    if (StateSlideServoUp) PushSlideServo();    

    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_FAST_WIND, HIGH);  
    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);  
    
    StopCapstan();

    
    lcd->Stop();
    playing = false;
    FastWinding = fwNone;
    SearchTrack = false;
    StopWhenTapeStarts = false;
    SearchTrackNumber = 0;
    paused = false;
    Recording = false;
    IgnoreStateTapeReader = 0;
    if (ReverseMode != rmNone && ReverseMode != rmEndless) {
        RepeatSecondSide = false;
        RewindOneSide = false;
        ReverseMode = rmNone;
        lcd->ReverseMode((int)ReverseMode);
    }
    delay(150);

}



/************************************************************************
 * Pause
 * In orignal firmware the servo slide goes down which is the same as
 * stop. do we really want this? needs further testing.
 * *********************************************************************/
void TTapeController::Pause() 
{
    #ifdef DEBUG
        Serial.println(":Pause()");
    #endif
    if (paused || SearchTrack) {return;}
    GetState();
    if (!StateSlideServoUp) {
        return;
    }

    SetMute(true);
    lcd->Pause();
    digitalWrite(SET_FAST_WIND, HIGH);
    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);     
    digitalWrite(SET_CAPSTAN_MOTOR, LOW); 
    
    paused = true;
    SearchTrack = false;
    FastWinding = fwNone; 
}



/************************************************************************
 * 
 * 
 * *********************************************************************/
void TTapeController::WindLeft() 
{
    #ifdef DEBUG
        Serial.println(":WindLeft()");
    #endif
    if (paused) return;
    if (AutoRestart) return;

    if (FastWinding != fwNone) 
    {
        if (digitalRead(SET_WIND_LEFT) == LOW && direction < 0) return;
        
    }
    AutoRestart = playing;

    Stop();
    GetState();

    IgnoreStateTapeReader = ms + 2000;
    if (AutoRestart && !SearchTrack) digitalWrite(SET_MUTE,LOW);
    else SetMute(true);

 //   if (!AutoRestart) SetMute(true);
 // else digitalWrite(SET_MUTE,LOW);
    StartCapstan(); 
    digitalWrite(SET_FAST_WIND_RELAY, LOW);
    digitalWrite(SET_FAST_WIND, HIGH);
    delay(100); 
    
    if (direction < 0) {
        digitalWrite(SET_WIND_LEFT, LOW);  
        digitalWrite(SET_WIND_RIGHT, HIGH);  
    }
    else {
        digitalWrite(SET_WIND_RIGHT, LOW);  
        digitalWrite(SET_WIND_LEFT, HIGH);  
    }
    delay(50);
    PushSlideServo();
    delay(100);
    digitalWrite(SET_FAST_WIND, LOW);  
    if (AutoRestart) {
        lcd->Play();
    }   
    if (direction < 0) {
        lcd->WindLeft();
        FastWinding = fwRewind; 
    }
    else {
        lcd->WindRight();
        FastWinding = fwForward; 
    }  

    SearchTrack = false;
    
}



/************************************************************************
 * Rewind
 * 
 * *********************************************************************/
void TTapeController::WindRight() 
{
    #ifdef DEBUG
        Serial.println(":WindRight()");
    #endif
    if (paused) return;
    if (AutoRestart) return;

    if (FastWinding != fwNone) 
    {
        if (digitalRead(SET_WIND_LEFT) == HIGH && direction < 0) return;      
    }
    AutoRestart = playing;
    
    Stop();
    GetState();

    IgnoreStateTapeReader = ms + 2000;

    if (AutoRestart && !SearchTrack)digitalWrite(SET_MUTE,LOW);
    else SetMute(true);
 //   if (!AutoRestart &) SetMute(true);
 //   else digitalWrite(SET_MUTE,LOW);
    StartCapstan();
    digitalWrite(SET_FAST_WIND_RELAY, LOW);
    digitalWrite(SET_FAST_WIND, HIGH);
    delay(100); 
    
    if (direction < 0) {
        digitalWrite(SET_WIND_RIGHT, LOW);  
        digitalWrite(SET_WIND_LEFT, HIGH);  
    }
    else {
        digitalWrite(SET_WIND_LEFT, LOW);  
        digitalWrite(SET_WIND_RIGHT, HIGH);         
    }
    delay(50);
    PushSlideServo();
    delay(100);
    digitalWrite(SET_FAST_WIND, LOW);   
    if (AutoRestart) {
        lcd->Play();
    }
    if (direction < 0) {
        lcd->WindRight();
        FastWinding = fwRewind; 
    }
    else {
        lcd->WindLeft();
        FastWinding = fwForward; 
    }
    SearchTrack = false;
    
}



/************************************************************************
 * NewTrack
 * From IRQ Pin 2
 * *********************************************************************/
void TTapeController::NewTrack()
{
    #ifdef DEBUG
        Serial.println(":NewTrack()");
    #endif
    if (!PrepareForProgramm) TrackFound = true;
}



/************************************************************************
 * ProgrammedTracks
 * Amount of programmed tracks
 * *********************************************************************/
int TTapeController::ProgrammedTracks()
{
    #ifdef DEBUG
        Serial.println(":ProgrammedTracks()");
    #endif
    int x = 0;
    for (byte i = 0; i <= 20; i++) {
        if (Programm[i]) x++;
    }
    return x;
}



/************************************************************************
 * EndProgramm
 * Ends the playback of a programm
 * *********************************************************************/
void TTapeController::EndProgramm()
{
    #ifdef DEBUG
        Serial.println(":EndProgramm()");
    #endif
    TrackNumber = 1;
    PlayProgramm = false;
    ProgrammPosition = -1;
    SearchTrack = false;
    ClearProgramm();
    Stop();
}



/************************************************************************
 * StartProgramm
 * Starts the playback of a programm
 * *********************************************************************/
void TTapeController::StartProgramm()
{
    #ifdef DEBUG
        Serial.println(":StartProgramm()");
    #endif
    Programming = false;
    ReverseMode = rmBothSides;
    RepeatSecondSide = false;
    PlayProgramm = true;
    ProgrammPosition = 0;
    TrackNumber = 0;
    PrepareForProgramm = true;

    // rewind the tape
    if (direction < 0 ) {WindLeft(); }
    else {WindRight();}

    lcd->ReverseMode((int)ReverseMode);
    lcd->ShowDigit(-1);
}




/************************************************************************
 * AnyProgrammedTracksAfter
 * Checks if there are any tracks stored after the x track
 * *********************************************************************/
bool TTapeController::AnyProgrammedTracksAfter(int x)
{
    x++;
    for (byte i = x; i <= 20; i++) {
        if (Programm[i]) return true;
    }
    return false;
}



/************************************************************************
 * ClearProgramm
 * Clears the programm
 * *********************************************************************/
void TTapeController::ClearProgramm()
{
    #ifdef DEBUG
        Serial.println(":ClearProgramm()");
    #endif
    Programming = false;
    for (byte i = 0; i <= 20; i++) {
        Programm[i] = false;
    }
    lcd->Clear();
}



/************************************************************************
 * HasProgramm
 * Checks if a programm is stored
 * *********************************************************************/
bool TTapeController::HasProgramm()
{
    for (byte i = 0; i <= 20; i++) {
        if (Programm[i]) return true;
    }    
    return false;
}



/************************************************************************
 * GetNextProgrammedTrack
 * Gets the index of the next programmed track or returns 0 if there are no
 * *********************************************************************/
int TTapeController::GetNextProgrammedTrack()
{
    int i = TrackNumber + 1;
    if (i > 20) { i = 1; }
    int max = 20; // in there are no tracks in the programm
    while (!Programm[i] && max > 0) {
        i++;
        if (i > 20) { i = 1; }
        max--;
    }
    if (max == 0) { TrackNumber = 0; }
    else { TrackNumber = i; } 
    return TrackNumber;
}



/************************************************************************
 * ProgrammKeyPressed
 * 
 * *********************************************************************/
void TTapeController::ProgrammKeyPressed()
{
    if (lcd->WaitingForInput()) {

        if (Programm[TrackNumber]) {
            Programm[TrackNumber] = false;
            lcd->Programm(TrackNumber,2);
        }
        else {
            Programm[TrackNumber] = true;
            lcd->Programm(TrackNumber,1);
        }
        return;
    }
    if (HasProgramm()) {
        lcd->ShowDigit(GetNextProgrammedTrack());
    }
    else {
        lcd->ShowError();
    }

    
}



/************************************************************************
 * AddSearchTrackNumber
 * Skip a maximum of 9 tracks
 * *********************************************************************/
bool TTapeController::AddSearchTrackNumber(int value) {

    bool r = false;
    if (value > 0 ) {

        if (SearchTrackNumber < 9) {
            SearchTrackNumber++;           
        }   
        if (SearchTrackNumber == 0) {
            SearchTrackNumber++;
            r = true;
        }  
    }
    else {
        if (SearchTrackNumber > -9) {
            SearchTrackNumber--;           
        }
        if (SearchTrackNumber == 0) {
            SearchTrackNumber--;
            r = true;
        }  
    }    
    return r;
}



/************************************************************************
 * GetTrackForProgramm
 * Programm Track +/-
 * *********************************************************************/
void TTapeController::GetTrackForProgramm(int value) {

    if (GetDirection() > 0 ) value *= -1;


    if (value < 0) {
        if (Programming) {   
            TrackNumber = InRange(TrackNumber - 1, 1, 20);
        }
        else {
            TrackNumber = 20;
            Programming = true;
        }

    }
    else {
         if (Programming) {   
            TrackNumber = InRange(TrackNumber + 1, 1, 20);
        }
        else {
            TrackNumber = 1;
            Programming = true;
        }       
    }

    lcd->Programm(TrackNumber, (int)Programm[TrackNumber]);

} 



/************************************************************************
 * NextTrack
 * Music search in current direction
 * *********************************************************************/
void TTapeController::NextTrack() 
{
    #ifdef DEBUG
        Serial.println(":NextTrack()");
    #endif
    if ((FastWinding != fwNone && !SearchTrack) || paused) {return;}
    if (!SearchTrack && !playing) {
        GetTrackForProgramm(1);
        return;
    }

    if (SearchTrack) {
        bool changeDir = false;
        if (direction > 0 ) {
            changeDir = AddSearchTrackNumber(-1);
        }
        else {
            changeDir = AddSearchTrackNumber(1);
        } 
        lcd->ShowDigit9(SearchTrackNumber);
        if (changeDir) {
            FastWinding = fwNone;
        }
        else {
            return;
        }
        
    }
    
    SearchTrack = true;
    if (direction > 0 ) { WindLeft(); }
    else { WindRight(); }
    digitalWrite(SET_MUTE,HIGH);
    AutoRestart = false;
    TrackFound = false;
    SearchTrack = true; // reseted by windleft/right
    if (direction > 0 ) {
        AddSearchTrackNumber(-1);
    }
    else {
        AddSearchTrackNumber(1);
    } 
    lcd->Play();
    lcd->BlinkPlay(true);
    lcd->ShowDigit9(SearchTrackNumber);
    lcd->WindRight();
}



/************************************************************************
 * PreviousTrack
 * Music search in previous direction
 * *********************************************************************/
void TTapeController::PreviousTrack() 
{
    #ifdef DEBUG
        Serial.println(":PreviousTrack()");
    #endif
    if ((FastWinding != fwNone && !SearchTrack) || paused) {return;}
    if (!SearchTrack && !playing) {
        GetTrackForProgramm(-1);
        return;
    }
   
    if (SearchTrack) {
        bool changeDir = false;
        if (direction < 0 ) {
            changeDir = AddSearchTrackNumber(-1);
        }
        else {
            changeDir = AddSearchTrackNumber(1);
        } 
        lcd->ShowDigit9(SearchTrackNumber);
        if (changeDir) {
            FastWinding = fwNone;
        }
        else {
            return;
        }
        
        
    }
    
    SearchTrack = true;
    if (direction < 0 ) { WindLeft(); }
    else { WindRight(); }
    digitalWrite(SET_MUTE,HIGH);
    AutoRestart = false;
    TrackFound = false;
    SearchTrack = true; // reseted by windleft/right
    if (direction < 0 ) {
        AddSearchTrackNumber(-1);
    }
    else {
        AddSearchTrackNumber(1);
    } 
    lcd->Play();
    lcd->BlinkPlay(true);
    lcd->ShowDigit9(SearchTrackNumber);
    lcd->WindLeft();
}



/************************************************************************
 * SetMute
 * 
 * *********************************************************************/
void TTapeController::SetMute(bool value)
{
    #ifdef DEBUG
        Serial.print(":SetMute(");
        Serial.print(value);
        Serial.println(")");
    #endif
    if (value) {
        SwitchOnMusic = 0;
        digitalWrite(SET_MUTE,HIGH);
    }
    
    else {
        SwitchOnMusic = ms + SWITCHON_MUSIC_DELAY; 
    }
}



/************************************************************************
 * ToggleDirection
 * Switch direction, turn head in play or stop mode
 * *********************************************************************/
void TTapeController::ToggleDirection() 
{
    #ifdef DEBUG
        Serial.println(":ToggleDirection()");
    #endif
    if (paused) {return;}
    if (FastWinding != fwNone) {return; }
    bool isPlaying = playing;

    if (isPlaying) Stop();
  

    bool WaitForState = !StateHeadServo;
    StartCapstan(); 
    digitalWrite(SET_WIND_LEFT, HIGH);  
    digitalWrite(SET_WIND_RIGHT, LOW);  

    digitalWrite(SET_FAST_WIND, LOW); 
    
    int tries = 0;


    while (StateHeadServo != WaitForState && tries < 1000) {
        GetState();
        tries++;
    }
    digitalWrite(SET_FAST_WIND, HIGH); 
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, LOW);  
    
    if (tries >= 1000) lcd->ShowError();


    direction *= -1;
    if (isPlaying) Play();
    else
    {
        delay(200);
        StopCapstan();  
        GetState();
    }
}



/************************************************************************
 * PushSlideServo
 * Push the slide servo relay. 40ms gives good results
 * *********************************************************************/
void TTapeController::PushSlideServo() 
{
    #ifdef DEBUG
        Serial.println(":PushSlideServo()");
    #endif
    digitalWrite(SET_SLIDE_SERVO, LOW);
    delay(DELAY_SLIDE_SERVO);
    digitalWrite(SET_SLIDE_SERVO, HIGH);
}



/************************************************************************
 * ToggleReverseMode
 * Switch through the different reverse modes
 * *********************************************************************/
void TTapeController::ToggleReverseMode()
{
    switch (ReverseMode) 
    {
        case rmNone :
            if (RecordMode != recNone) ReverseMode = rmBothSides;
            else ReverseMode = rmOneSide;
            break;
        case rmOneSide :
            ReverseMode = rmBothSides;
            break;
        case rmBothSides :
            if (RecordMode != recNone) ReverseMode = rmNone;
            else ReverseMode = rmEndless;
            break;
        case rmEndless :
            ReverseMode = rmNone;
            break;
    }
    lcd->ReverseMode((int)ReverseMode);
}



/************************************************************************
 * ToggleRecordMode
 * Switch through the different record modes
 * *********************************************************************/
void TTapeController::ToggleRecordMode()
{
    switch (RecordMode) {
        case recNone:
            return;
        case recOn:
            RecordMode = recSync;
            break;
        case recSync:
            RecordMode = recAuto;
            break;
        case recAuto:
            RecordMode = recOn;
            break;

    }
    lcd->RecordMode((int)RecordMode);
}

void TTapeController::StateToString() 
{
    Serial.print("+Tape: " + String(CurrentState,DEC) + " ");
    if (StateSlideServoUp) { Serial.print("SlideServo "); }
    if (StateRecord) { Serial.print( "Rec "); }
    if (StateHeadServo) { Serial.print( "HeadServo "); }
    if (StatePause) { Serial.print( "Pause "); }
    if (StateTapeReader) { Serial.print( "TapeReader "); }
    if (StateReelMotor) { Serial.print( "ReelMotor "); }
    if (RecordDiskEnd ) { Serial.print( "RecDiscEnd "); }
    if (RecordEnabledForSideA ) { Serial.print( "RecSideA "); }
    if (RecordEnabledForSideB) { Serial.print( "RecSideB "); }
    Serial.println();
}

/************************************************************************
 * MoveRecPlaybackLever
 * 
 * *********************************************************************/
void TTapeController::MoveRecPlaybackLever()
{
    bool WaitForState = !StateRecord;

    #ifdef DEBUG
        Serial.println(":MoveRecPlaybackLever()");
        Serial.print(StateRecord);
        Serial.print(" > ");
        Serial.println(WaitForState);
    #endif
  
    int Error = 200;
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, HIGH);  
    digitalWrite(SET_FAST_WIND, LOW); 
    while (StateRecord != WaitForState) {
        StateToString();
        delay(5); 
        GetState();
        Error--;
        if (Error <= 0) {
            lcd->ShowError();
            break;
        }
    }

    digitalWrite(SET_FAST_WIND, HIGH); 
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, LOW); 
}



/************************************************************************
 * NewAutoRecordTrackStarted
 * 
 * *********************************************************************/
void TTapeController::NewAutoRecordTrackStarted() {

    AutoRecordTrackNumber++;
    lcd->ShowDigit(AutoRecordTrackNumber);
}



/************************************************************************
 * CheckIfRecordingIsPossible
 * 
 * *********************************************************************/
bool TTapeController::CheckIfRecordingIsPossible()
{
    if ((direction > 0 && !RecordEnabledForSideA) || (direction < 0 && !RecordEnabledForSideB)) {
        lcd->ShowError();
        Stop();
        return false;
    }
    return true;
}

/************************************************************************
 * StartRecordMode
 * 
 * *********************************************************************/
void TTapeController::StartRecordMode()
{
    #ifdef DEBUG
        Serial.println(":StartRecordMode()");
    #endif
    if (paused) return;
    if (playing || FastWinding != fwNone) Stop();

    if (RecordMode == recNone) {
        if (!CheckIfRecordingIsPossible()) return;
        RecordMode = recOn;
    }
    else RecordMode = recNone;

    

    MoveRecPlaybackLever();
    lcd->RecordMode((int)RecordMode);

    // ReverseMode can only be A->B
    if (ReverseMode != rmNone && ReverseMode != rmBothSides) {
        ReverseMode = rmBothSides;
        lcd->ReverseMode((int)ReverseMode);
    }

    // Move forward if cassette is at the start of the tape
    if (RecordMode == recOn && StateTapeReader) {
        StopWhenTapeStarts = true;
        lcd->BlinkRecord(true);
        Play();   
    }
}



/************************************************************************
 * GetState
 * Reads the current state
 * *********************************************************************/
void TTapeController::GetState() 
{
    CurrentState = inputs->GetCassetteState(); 
    StateSlideServoUp = (bool)((CurrentState & inputs->IN_SLIDE_SERVO) != 0);
    StateRecord = (bool)(CurrentState & inputs->IN_REC);
    StateHeadServo = (bool)(CurrentState & inputs->IN_HEAD_SERVO);
    StatePause = (bool)(CurrentState & inputs->IN_PAUSE_CASS);
    StateTapeReader = (bool)(CurrentState & inputs->IN_TAPE_READER);
    StateReelMotor = (bool)(CurrentState & inputs->IN_REEL_MOTOR);
    RecordDiskEnd = !(bool)((CurrentState & inputs->IN_REC_DISC_END));
    RecordEnabledForSideA = (bool)(CurrentState & inputs->IN_REC_SIDE_A);
    RecordEnabledForSideB = (bool)(CurrentState & inputs->IN_REC_SIDE_B);

    if (IgnoreStateTapeReader > 0) {
        if (IgnoreStateTapeReader > ms) StateTapeReader = false;
        else {
            StartedWithEmptyTape = StateTapeReader;
            IgnoreStateTapeReader = 0;
        }
    }
}



/************************************************************************
 * Update
 * Refresh the current states and check if any action is needed
 * *********************************************************************/
void TTapeController::Update() 
{
    ms = millis();
    GetState();

    if (SwitchOffCapstan > 0 && SwitchOffCapstan < ms) {
        SwitchOffCapstan = 0;
        digitalWrite(SET_CAPSTAN_MOTOR, LOW); 
        #ifdef DEBUG
            Serial.println("> Capstan Motor OFF");
        #endif

    }

    if (SwitchOnMusic > 0 && SwitchOnMusic < ms) {
        SwitchOnMusic = 0;
        digitalWrite(SET_MUTE,LOW);
        #ifdef DEBUG
            Serial.println("> Mute OFF");
        #endif
    }

    // set the current direction based on the head switch
    if (StateHeadServo != 0) {
        direction = -1;
        lcd->DirectionLeft();
    }
    else {
        direction = 1;
        lcd->DirectionRight();
    }

/*
    // for debugging reasons
    if (StateTapeReader) {
        digitalWrite(PIN_BUILD_IN_LED, HIGH);
        delay(100);
    }
    else {
        digitalWrite(PIN_BUILD_IN_LED, LOW);
    }
*/

    // Search for track and continue to play when found
    if (!PlayProgramm && SearchTrack) {

        if (TrackFound) {
            TrackFound = false;
            SearchTrackNumber += (SearchTrackNumber > 0) ? -1 : 1;
            lcd->ShowDigit9(SearchTrackNumber);         
            if (SearchTrackNumber == 0) {
                SearchTrack = false;  
                Play();
            }        
            return;
        }
    }





    // Playback of programmed tracks
    if (PlayProgramm) {

        if (!PrepareForProgramm && StateTapeReader && !StartedWithEmptyTape) {
            ToggleDirection();  
            ProgrammPosition++;
            if (!Programm[ProgrammPosition]) NextTrack();
            lcd->ShowDigit(ProgrammPosition);
            return;
        }
        
        if (PrepareForProgramm && (StateTapeReader || StateReelMotor)) {
            Stop();
            PrepareForProgramm = false;               
            Play();   
            StartedWithEmptyTape = true;
            ProgrammPosition = 1;    
            lcd->ShowDigit(ProgrammPosition);
            if (!Programm[ProgrammPosition]) NextTrack();
        }

        // new track found
        if (TrackFound) {
            
            TrackFound = false;
            ProgrammPosition++;

            
            if (!Programm[ProgrammPosition] && !SearchTrack) {

                if (!AnyProgrammedTracksAfter(ProgrammPosition) || ProgrammPosition == 20) EndProgramm();
                else {
                    NextTrack();           
                    lcd->ShowDigit(ProgrammPosition);
                }
                return;
            }

            if (Programm[ProgrammPosition] && SearchTrack) {
                SearchTrack = false;
                Play(); 
            }

            
        }
        if (PrepareForProgramm) lcd->ShowDigit(-1);
        else lcd->ShowDigit(ProgrammPosition);
        return;
    }

    // when fast winding check if tape is at the end 
    if (FastWinding != fwNone) {
   
        if (StateTapeReader || StateReelMotor) {
            if (RewindOneSide) {
                Play();
                RewindOneSide = false;
            }
            else {
                // in case of rewind and tape end just put a little bit forward
                // only to avoid that the tape is on the very end and the tape reader
                // cannot shine through because of a splice
                if (FastWinding == fwRewind && StateReelMotor && !StateTapeReader) {
                    Stop();
                    delay(100); 
                    Play();
                    delay(500);                                
                }
                Stop();
            }          
            return;
        }
    }



    if (Recording && RecordDiskEnd && (RecordMode == recAuto || RecordMode == recSync)) {
        #ifdef DEBUG
            Serial.println("Disc end, stop recording");
        #endif
        Stop();
        return;
    }

    // Synchronised record from CD
    if (Recording && !paused && RecordMode != recOn) {

        if (RecordDiskEnd) {
            Pause();
            return;
        }
   /*     if (digitalRead(CD_INPUT_PAUSE) == LOW) {
            Pause();
            return;
        }*/
        if (digitalRead(CD_INPUT_STOP) == LOW) {
            Pause();
            return;
        }
    }



    // when playing check for tape and if any reverse modes are active
    if (playing) {
        
        if (StopWhenTapeStarts && !StateTapeReader) {
            StopWhenTapeStarts = false;
            Stop();
            lcd->BlinkRecord(false);
            lcd->RecordMode(recOn);
            #ifdef DEBUG
                Serial.println("Tape ready for record"); 
            #endif 
            return;
        }


        if (StartedWithEmptyTape && !StateTapeReader) {
           StartedWithEmptyTape = false;
           #ifdef DEBUG
                Serial.println(">StartedWithEmptyTape = false");  
           #endif 
        }

  
        if (StateReelMotor || (StateTapeReader && !StartedWithEmptyTape)) {
            if (paused) return;
            if (Recording) {
                if (ReverseMode == rmBothSides) {
                    ToggleDirection();
                    ReverseMode = rmNone;
                    if (!CheckIfRecordingIsPossible()) return;
                    return;
                }
                Stop();
                return;
            }
            switch(ReverseMode)
            {
                case rmNone:
                    Stop();
                    return;

                case rmOneSide:
                    Stop();
                    delay(500);
                    if (direction < 0) {
                        WindRight();
                    }
                    else {
                        WindLeft();
                    }       
                    delay(2000);
                    RewindOneSide = true;
                    return;

                case rmBothSides:
                    if (RepeatSecondSide) {
                        Stop();
                        return;
                    }
                    ToggleDirection();
                    RepeatSecondSide = true;
                    delay(2000);
                    return;

                case rmEndless:
                    ToggleDirection();
                    delay(1000);
                    return;
            }         
        }       
    }







    // check the server slide position at the start up
    // needs to be done otherwise the lid cannot be opened
    // if the system hangs. replaces hardware integration around 7517
    if (StartUp) {
        SetMute(true);
        StartUp = false;
        if (StateSlideServoUp) {
            digitalWrite(SET_CAPSTAN_MOTOR, HIGH);
            PushSlideServo();
            delay(500); 
            digitalWrite(SET_CAPSTAN_MOTOR, LOW);
            GetState();
            if (StateSlideServoUp) lcd->ShowError();
        }
        if (StateRecord) MoveRecPlaybackLever();
        lcd->Clear();
        return;
    }



}
