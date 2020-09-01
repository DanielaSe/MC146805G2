/***************************************************************
 * 
 * Play
 * Capstan Motor ON and impulse on Relay 1 
 * 
 * Stop
 * Same as Play - Capstan Motor ON and impulse on Relay 1 
 * It seems we need to consider the sensors, if a tape is inserted
 * 
 * * ************************************************************/





#include "TTapeController.h"
#include "Arduino.h"
#include "TPinLayout.h"
#include "TDisplay.h"
#include "TInputs.h"



/************************************************************************
 * constructor
 * 
 * *********************************************************************/
TTapeController::TTapeController(TDisplay *_display, TInputs *_inputs) {


    TrackNumber = 1;
    Programming = false;
    FastWinding = false;
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

    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);
    digitalWrite(SET_FAST_WIND, HIGH);
    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_CAPSTAN_MOTOR, LOW);
    digitalWrite(SET_SLIDE_SERVO, HIGH);
}



int TTapeController::GetDirection() { return direction;}
bool TTapeController::Playing() { return playing; }
bool TTapeController::Paused() { return paused; }
int TTapeController::InRange(int value, int min, int max)
{
    if (value < min) { value = max; }
    if (value > max) { value = min; }
    return value;
}






/************************************************************************
 * Play
 * 
 * *********************************************************************/
void TTapeController::Play() 
{
    GetState();
    if (paused) {
        digitalWrite(SET_FAST_WIND, HIGH);
        digitalWrite(SET_CAPSTAN_MOTOR, HIGH);
        if (direction > 0) {
            digitalWrite(SET_WIND_RIGHT, LOW);
            digitalWrite(SET_WIND_LEFT, HIGH);
        }
        else {
            digitalWrite(SET_WIND_LEFT, LOW);
            digitalWrite(SET_WIND_RIGHT, HIGH);    
        }
        paused = false;
        lcd->Play();
        return;
    }

    if (playing && !FastWinding) {
        return;
    }

    if (!PlayProgramm && HasProgramm()) {
        StartProgramm();
        return;
    }
    
    if (StateSlideServoUp) {   
        Stop();
        GetState();
    }

    if (!playing && RecordMode != recNone) {
        Recording = true;
        ReverseMode = rmNone;
    }
    
    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_FAST_WIND, HIGH); 
    digitalWrite(SET_WIND_RIGHT, LOW);
    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_CAPSTAN_MOTOR, HIGH);

    if (!StateSlideServoUp) {     
        delay(100);
        PushSlideServo();
        delay(50);
        
    }
    if (direction > 0) {
        digitalWrite(SET_WIND_RIGHT, LOW);
        digitalWrite(SET_WIND_LEFT, HIGH);
    }
    else {
        digitalWrite(SET_WIND_LEFT, LOW);
        digitalWrite(SET_WIND_RIGHT, HIGH);    
    }
    lcd->Play();
    FastWinding = false;
    playing = true;
    paused = false;  
    SearchTrack = false;
    TrackFound = false;
    if (ReverseMode != rmNone) {
        RepeatSecondSide = false;
        RewindOneSide = false;
    }
    delay(200);
}



/************************************************************************
 * Stop
 * 
 * *********************************************************************/
void TTapeController::Stop() 
{
    if (paused) {
        digitalWrite(SET_CAPSTAN_MOTOR, HIGH);   
    }

    if (!playing && !FastWinding) {
        return;
    }
    GetState();
    if (FastWinding)
    {
        // Correction Impulse
        if (digitalRead(SET_WIND_LEFT) == HIGH) {
            digitalWrite(SET_WIND_LEFT, LOW);
            digitalWrite(SET_WIND_RIGHT, HIGH);      
        }
        else {
            digitalWrite(SET_WIND_LEFT, HIGH);
            digitalWrite(SET_WIND_RIGHT, LOW);  
        }
        delay(40); 
    }

    
    if (StateSlideServoUp) {
        PushSlideServo();    
    }
    digitalWrite(SET_FAST_WIND_RELAY, HIGH);
    digitalWrite(SET_FAST_WIND, HIGH);  
    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);  
    delay(400);
    digitalWrite(SET_CAPSTAN_MOTOR, LOW);   
    lcd->Stop();
    playing = false;
    FastWinding = false;
    paused = false;
    if (ReverseMode != rmNone && ReverseMode != rmEndless) {
        RepeatSecondSide = false;
        RewindOneSide = false;
        ReverseMode = rmNone;
        lcd->ReverseMode((int)ReverseMode);
    }
    SearchTrack = 0;

    if (Recording) {
        Recording = false;
    }

  //  delay(200);
}



/************************************************************************
 * Pause
 * 
 * *********************************************************************/
void TTapeController::Pause() 
{
    if (paused || SearchTrack) {return;}
    GetState();
    if (!StateSlideServoUp) {
        return;
    }
    lcd->Pause();
    digitalWrite(SET_FAST_WIND, HIGH);  
    digitalWrite(SET_WIND_LEFT, LOW);
    digitalWrite(SET_WIND_RIGHT, LOW);
    digitalWrite(SET_CAPSTAN_MOTOR, LOW); 
    paused = true;
    FastWinding = false; 
}



/************************************************************************
 * 
 * 
 * *********************************************************************/
void TTapeController::WindLeft() 
{
    if (paused) {return;}
    Stop();

    GetState();
    digitalWrite(SET_CAPSTAN_MOTOR, HIGH); 
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
    delay(200);
    digitalWrite(SET_FAST_WIND, LOW);   
    if (direction < 0) {
        lcd->WindRight();
    }
    else {
        lcd->WindLeft();
    }    
    SearchTrack = false;
    FastWinding = true; 
}



/************************************************************************
 * Rewind
 * 
 * *********************************************************************/
void TTapeController::WindRight() 
{
    if (paused) {return;}
    Stop();
    GetState();
  
    digitalWrite(SET_CAPSTAN_MOTOR, HIGH); 
    digitalWrite(SET_FAST_WIND_RELAY, LOW);
    digitalWrite(SET_FAST_WIND, HIGH);
    delay(100); 
    
    if (direction > 0) {
        digitalWrite(SET_WIND_RIGHT, LOW);  
        digitalWrite(SET_WIND_LEFT, HIGH);  
    }
    else {
        digitalWrite(SET_WIND_LEFT, LOW);  
        digitalWrite(SET_WIND_RIGHT, HIGH);         
    }
    delay(50);
    PushSlideServo();
    delay(200);
    digitalWrite(SET_FAST_WIND, LOW);   
    if (direction < 0) {
        lcd->WindLeft();
    }
    else {
        lcd->WindRight();
    }
    SearchTrack = false;
    FastWinding = true; 
}



/************************************************************************
 * NewTrack
 * From IRQ Pin 2
 * *********************************************************************/
void TTapeController::NewTrack()
{
    TrackFound = true;
}



/************************************************************************
 * ProgrammedTracks
 * Amount of programmed tracks
 * *********************************************************************/
int TTapeController::ProgrammedTracks()
{
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
    TrackNumber = 1;
    PlayProgramm = false;
    ProgrammPosition = -1;
    ClearProgramm();
    Stop();
    lcd->Stop();
}



/************************************************************************
 * StartProgramm
 * Starts the playback of a programm
 * *********************************************************************/
void TTapeController::StartProgramm()
{
    Programming = false;
    ReverseMode = rmBothSides;
    RepeatSecondSide = false;
    PlayProgramm = true;
    ProgrammPosition = 0;
    TrackNumber = 0;
    if (StateTapeReader) {
        RepeatSecondSide = true;
        paused = true;
        Play();
        ToggleDirection();  
        delay(1000);   
        TrackFound = true;
        SearchTrack = false;
        return;
    }
    // not at the beginning of the tape -> rewind
    lcd->ReverseMode((int)ReverseMode);
    lcd->ShowDigit(-1);
    if (direction > 0 ) {WindLeft(); }
    else {WindRight();}
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
    lcd->ShowDigit(GetNextProgrammedTrack());
}



/************************************************************************
 * NextTrack
 * Music search in current direction
 * *********************************************************************/
void TTapeController::NextTrack() 
{
      
    if (direction < 0 ) {WindLeft(); }
    else {WindRight();}
    lcd->Play();
    SearchTrack = true;
/*
    if (FastWinding || paused || SearchTrack) {return;}
    if (!playing) {
        if (Programming) {   
            TrackNumber = InRange(TrackNumber + 1, 1, 20);
        }
        else {
            TrackNumber = 1;
            Programming = true;
        }
        lcd->Programm(TrackNumber, (int)Programm[TrackNumber]);
        return;
    }
    TrackFound = false;
    SearchTrack = true;  
    digitalWrite(SET_FAST_WIND, LOW);
    if (direction < 0) {
        lcd->WindLeft();
    }
    else {
        lcd->WindRight();
    }
*/
}



/************************************************************************
 * PreviousTrack
 * Music search in previous direction
 * *********************************************************************/
void TTapeController::PreviousTrack() 
{
    if (direction > 0 ) {WindLeft(); }
    else {WindRight();}
    lcd->Play();
    SearchTrack = true;
/*
    if (FastWinding || paused || SearchTrack) {return;}
    if (!playing) {
        if (Programming) {   
            TrackNumber = InRange(TrackNumber - 1, 1, 20);
        }
        else {
            TrackNumber = 20;
            Programming = true;
        }
        lcd->Programm(TrackNumber, (int)Programm[TrackNumber]);
        return;
    }

    SearchTrack = true;
    digitalWrite(SET_WIND_RIGHT, LOW);  
    digitalWrite(SET_WIND_LEFT, LOW);    
    digitalWrite(SET_FAST_WIND, LOW);
    delay(100);     
    if (direction < 0) {
        digitalWrite(SET_WIND_RIGHT, LOW);  
        digitalWrite(SET_WIND_LEFT, HIGH);  
    }
    else {
        digitalWrite(SET_WIND_LEFT, LOW);  
        digitalWrite(SET_WIND_RIGHT, HIGH);         
    }
    
    if (direction < 0) {
        lcd->WindRight();
    }
    else {
        lcd->WindLeft();
    }
    */
}



/************************************************************************
 * ToggleDirection
 * Switch direction, turn head in play or stop mode
 * *********************************************************************/
void TTapeController::ToggleDirection() 
{
    if (paused) {return;}
    if (FastWinding) {return; }
    bool isPlaying = playing;

    if (isPlaying) {
        Stop();
    }

    digitalWrite(SET_CAPSTAN_MOTOR, HIGH);  
    digitalWrite(SET_WIND_LEFT, HIGH);  
    digitalWrite(SET_WIND_RIGHT, LOW);  

    digitalWrite(SET_FAST_WIND, LOW); 
    delay(100); 
    digitalWrite(SET_FAST_WIND, HIGH); 
    delay(50); 
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, LOW);  
    

    direction *= -1;
    if (isPlaying) {
        Play();
    }
    else
    {
        delay(200);
        digitalWrite(SET_CAPSTAN_MOTOR, LOW);  
        GetState();
    }
}



/************************************************************************
 * PushSlideServo
 * Push the slide servo relay. 40ms gives good results
 * *********************************************************************/
void TTapeController::PushSlideServo() 
{
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
            ReverseMode = rmOneSide;
            break;
        case rmOneSide :
            ReverseMode = rmBothSides;
            break;
        case rmBothSides :
            ReverseMode = rmEndless;
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


/************************************************************************
 * MoveRecPlaybackLever
 * 
 * *********************************************************************/
void TTapeController::MoveRecPlaybackLever()
{
    bool WaitForState = !StateRecord;
    
    int Error = 2000;
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, HIGH);  
    digitalWrite(SET_FAST_WIND, LOW); 
    while (StateRecord = WaitForState) {
        delay(20); 
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



    /*
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, HIGH);  
    digitalWrite(SET_FAST_WIND, LOW); 
    delay(190); 
    digitalWrite(SET_FAST_WIND, HIGH); 
    digitalWrite(SET_WIND_LEFT, LOW);  
    digitalWrite(SET_WIND_RIGHT, LOW);     
    */ 
}


/************************************************************************
 * StartRecordMode
 * 
 * *********************************************************************/
void TTapeController::StartRecordMode()
{
    if (playing || FastWinding) {
        Stop();
    }
    if (RecordMode == recNone) RecordMode = recOn;
    else RecordMode = recNone;

    MoveRecPlaybackLever();
    lcd->RecordMode((int)RecordMode);
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
    StateTapeReader = (bool)((CurrentState & inputs->IN_TAPE_READER) != 0);
    StateReelMotor = (bool)(CurrentState & inputs->IN_REEL_MOTOR);
    RecordDiskEnd = (bool)(CurrentState & inputs->IN_REC_DISC_END);
    RecordEnabledForSideA = (bool)(CurrentState & inputs->IN_REC_SIDE_A);
    RecordEnabledForSideB = (bool)(CurrentState & inputs->IN_REC_SIDE_B);
}



/************************************************************************
 * Update
 * Refresh the current states and check if any action is needed
 * *********************************************************************/
void TTapeController::Update() 
{


    GetState();

    // Playback of programmed tracks
    if (PlayProgramm) {
        // tape reached the end
        if (StateTapeReader) {
      
            if (FastWinding) {
    
                ProgrammPosition = 1;
                
                Play();
                lcd->ShowDigit(ProgrammPosition);
                delay(500);
               
                if (!Programm[ProgrammPosition]) {
                    NextTrack();
                }
               
                return;
            }
            if (RepeatSecondSide) {
                EndProgramm();
                return;                
            }
            RepeatSecondSide = true;
            paused = true;
            Play();
            ToggleDirection();  
            delay(1000);   
            TrackFound = true;
            SearchTrack = false;
        }

        // new track found
        if (TrackFound) {
            
            TrackFound = false;
            ProgrammPosition++;
            if (!Programm[ProgrammPosition] && !AnyProgrammedTracksAfter(ProgrammPosition)) {
                EndProgramm();
                return;
            }
            if (ProgrammPosition > 20) {
                EndProgramm();
                return;
            }

            lcd->ShowDigit(ProgrammPosition);
            if (!Programm[ProgrammPosition] && !SearchTrack) {
                NextTrack();
                return;
            }

            if (Programm[ProgrammPosition] && SearchTrack) {
                SearchTrack = false;
                paused = true;
                Play(); 
            }
            
        }
        return;
    }

    // Search for track and continue to play when found
    if (SearchTrack != 0) {
        if (TrackFound) {
            
            SearchTrack = false;
            TrackFound = false;
            // paused = true;
            Play();
            return;
        }
    }

    // when fast winding check if tape is at the end 
    if (FastWinding) {
        if (StateTapeReader) {
            if (RewindOneSide) {

                Play();
                RewindOneSide = false;
            }
            else {
                Stop();
            }          
            return;
        }
    }


    // Synchronised record from CD
    if (Recording && !paused && RecordMode != recOn) {

        if (RecordDiskEnd) {
            Pause();
            return;
        }
        if (digitalRead(CD_INPUT_PAUSE) == LOW) {
            Pause();
            return;
        }
        if (digitalRead(CD_INPUT_STOP) == LOW) {
            Pause();
            return;
        }
    }

    // when playing check for tape and if any reverse modes are active
    if (playing) {
        if (StateTapeReader) {
            if (Recording) {
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

    // set the current direction based on the head switch
    if (StateHeadServo != 0) {
        direction = -1;
        lcd->DirectionRight();
    }
    else {
        direction = 1;
        lcd->DirectionLeft();
    }

    // check the server slide position at the start up
    // needs to be done otherwise the lid cannot be opened
    // if the system hangs. replaces hardware integration around 7517
    if (StartUp) {
        StartUp = false;
        if (StateSlideServoUp) {
            digitalWrite(SET_CAPSTAN_MOTOR, HIGH);
            PushSlideServo();
            delay(500); 
            digitalWrite(SET_CAPSTAN_MOTOR, LOW);
            GetState();
            if (StateSlideServoUp) lcd->ShowError();
        }
        lcd->Clear();
        return;
    }

    


}
