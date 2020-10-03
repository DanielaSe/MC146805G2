/***************************************************************************
 * 
 * Replacement for uP MC146805G2 in Philips 70CD555
 * 
 *      www.70cd555.com         
 *      Daniela Sefzig, Oct.2020
 * 
 * 
 * 
 * Direct replacement for item 7501.
 * Still work in progress. Needs more testing.
 * 
 * Requires the following changings on the uProc panel:
 * - put a socket instead of the MC146805G2 and insert replacement PCB
 * - use additional small clock PCB for the CD-Player CPU with 4MHz crystal
 *   (powering the clock with inverters was not stable, maybe retry with the next revision)
 * 
 * 
 * The original CPU runs on 8.7MHz - Arduino is almost as twice as fast with 16MHz, 
 * thus the timing has to be different. Only one key press at a time is supported, except the demo keys.
 * 
 * 
 * What is different to the orignal - because I liked to improve the handling
 * 
 * - When pressing record and the tape reader recognises the tape is
 *   at the very beginning it moves forward until the tape really starts.
 *   Avoids to record music on the non magnetic part of the tape.
 * 
 * - Pause does not move the slide servo - otherwise it would be the
 *   same behaviour as stop. Need some further checks if this handling is ok.
 * 
 * - Programming works differently. The tape rewinds when you start
 *   the program, not before. also side B is supported for programming
 * 
 * - Demo mode only supports to check the keys (eq. demo mode 0)
 * 
 * - Since the CD CPU is not snyced anymore the events from the CPU
 *   might not appear - or at least not sure when they get fired because there
 *   is no documentation. Events just used in auto-rec mode.
 * 
 * - Not sure about the exact handling of the mute output. In this case for forward/rewind
 *   it is only used when in play mode
 * 
 * 
 * 
 * 
 * 
 * 
 * Todo: find a way to enable the oscillator without the original CPU
 * 
 * 
 * 
 **************************************************************************/




#include "TPinLayout.h"

#include "PinChangeInterrupt.h"
#include "TInputs.h"
#include "TCDController.h"
#include "TTapeController.h"
#include "TDisplay.h"
#include "defines.h"




TDisplay lcd = TDisplay();
TInputs input = TInputs();
TCDController cd = TCDController();
TTapeController tape = TTapeController(&lcd, &input);

bool blink = false;
int cycle = 0;
int OldState = 0;

void NewTrack()
{
    tape.NewTrack();
    #ifdef DEBUG
        Serial.println("IRQ Detected");
    #endif
}

void OnCDPlayerStateChanged(const int state)
{
    #ifdef DEBUG
        Serial.print("CD-Player State: ");
        Serial.println(state);
    #endif
    // we are not going to use this events. we might losing them from time to time since the two
    // CPUs are not synced anymore. However, we know when we press Play, Pause and Stop
    // For the events we consider the 4 seconds pause during auto-rec mode.

    // CD Pause    
    if ((bool)(state & PIN_PC4)) {
        if (tape.IsOnAutoRecord()) {
            cd.Pause(true);
        }

    }

    // CD Auto Pause 
    if ((bool)(state & PIN_PA7)) {


    }

    // CD Play 
    if ((bool)(state & PIN_PA6)) {


    }

}


void setup() {


    attachPinChangeInterrupt(9, NewTrack, FALLING);
    pinMode(PIN_BUILD_IN_LED, OUTPUT);

    #ifdef DEBUG
        Serial.begin(9600); 
    #endif
    
    cd.OnStateChanged(OnCDPlayerStateChanged);


    delay(10);
}


bool CHECK_BIT(int value, int b) {

    return value & b;
}

String StateToString(int value) 
{
    String result = "State: " + String(value,DEC) + " ";   
    if (!CHECK_BIT(value, input.IN_HEAD_SERVO)) { result += "HeadServo "; }
    if (!CHECK_BIT(value, input.IN_PAUSE_CASS)) { result += "Pause "; }
    if (CHECK_BIT(value, input.IN_REC)) { result += "Rec "; }
    if (CHECK_BIT(value, input.IN_REC_DISC_END)) { result += "RecDiscEnd "; }
    if (CHECK_BIT(value, input.IN_REC_SIDE_A)) { result += "RecSideA "; }
    if (CHECK_BIT(value, input.IN_REC_SIDE_B)) { result += "RecSideB "; }
    if (CHECK_BIT(value, input.IN_REEL_MOTOR)) { result += "ReelMotor "; }
    if (CHECK_BIT(value, input.IN_SLIDE_SERVO)) { result += "SlideServo "; }
    if (CHECK_BIT(value, input.IN_TAPE_READER)) { result += "TapeReader "; }
    return result;
}

String KeyToString(int value) 
{
    switch (value)
    {

        case 0x0001 : return "Key:Reverse Mode";
        case 0x0002 : return "Key:Rec Mode    "; 
        case 0x0004 : return "Key:Direction   "; 

        case 0x0008 : return "Key:Play       ";
        case 0x0010 : return "Key:Rewind     "; 
        case 0x0020 : return "Key:Forward    "; 

        case 0x0040 : return "Key:Stop/Clear "; 
        case 0x0080 : return "Key:Pause      "; 
        case 0x0100 : return "Key:Rec        "; 

        case 0x0200 : return "Key:Next       "; 
        case 0x0400 : return "Key:Previous   ";
        case 0x0800 : return "Key:Program    "; 

        case 0x1000 : return "Key:CD Play    "; 
        case 0x2000 : return "Key:CD Stop    "; 
        case 0x4000 : return "Key:CD Pause   "; 

    }
    return "Unknonw key    "; 
}


void loop() {


    // Read Keys
  
    input.read();

 
    // demo mode gives several options to check the function of the tape deck
    // this method is currently not equal to the original code
    if (input.IsDemoMode()) {
  
        tape.Reset();
        
        if (input.pressedKeys != 0 && input.pressedKeys != input.CD_PLAY) {
            lcd.ShowNone();
            lcd.ShowNone();
            lcd.ShowNone();
            delay(500);
        }
        else { 
            lcd.ShowAll();
        }
        tape.Update();
        delay(100);
        return;
    }



    // we are only considering one key at a time
    // keys Play/Stop/Pause on tape deck
    switch(input.pressedKeys)
    {
        // CD Player
        case input.CD_PLAY:
            cd.Play();
            if (tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) tape.Play();
            break;
            
        case input.CD_PAUSE:
            cd.Pause(tape.IsOnAutoRecord()); 
            if (tape.IsOnSyncRecord()) tape.Pause();
            break;
            
        case input.CD_STOP:
            cd.Stop();
            if (tape.IsOnAutoRecord()) tape.Pause();    
            if (tape.IsOnSyncRecord()) tape.Stop();   
            break;

        // Tape Deck
        case input.CASS_PLAY:
            if (tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break;
            tape.Play(); 
            break;

        case input.CASS_PAUSE:
            if (tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break; 
            if (tape.Programming) lcd.ShowError();
            else tape.Pause();
            break;

        case input.CASS_STOP:
            if (tape.Programming) tape.ClearProgramm();
            else if (tape.PlayProgramm) tape.EndProgramm();
            else tape.Stop();
            break;

        case input.CASS_REC_MODE:
            if (tape.Programming || tape.Recording) lcd.ShowError();
            else tape.ToggleRecordMode();
            break;
            
        case input.CASS_REC:
            if (tape.Programming || tape.Recording) lcd.ShowError();
            else tape.StartRecordMode();
            break;
    
    }


    // other tape deck keys only if record is not active
    if (!tape.IsOnRecord()) {
        switch(input.pressedKeys)
        {
            case input.CASS_NEXT:
                if (tape.GetDirection() < 0 ) {
                    tape.NextTrack();
                }
                else {
                tape.PreviousTrack(); 
                }
                break;

            case input.CASS_PREV:
                if (tape.GetDirection() < 0 ) {
                    tape.PreviousTrack();
                }
                else {
                tape.NextTrack(); 
                }
                break;

            case input.CASS_FORWARD:
                if (tape.Programming || tape.Recording) lcd.ShowError();
                else if (tape.GetDirection() > 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_REWIND:
                if (tape.Programming || tape.Recording) lcd.ShowError();
                else if (tape.GetDirection() < 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_REVERSE_MODE:
                if (tape.Programming || tape.Recording) lcd.ShowError();
                else tape.ToggleReverseMode();
                break;

            case input.CASS_PROG:
                if (tape.Recording) lcd.ShowError();
                else tape.ProgrammKeyPressed(); 
                break;

            case input.CASS_DIRECTION:
                if (tape.Programming || tape.Recording) lcd.ShowError();
                else tape.ToggleDirection();
                break;

        }
    }




    if (tape.AutoRestart && !tape.SearchTrack && input.pressedKeys != input.CASS_FORWARD && input.pressedKeys != input.CASS_REWIND) {
        tape.Play();
    }

    // for easier debugging using the internal led to show states
    #ifdef DEBUG
 /*       if (tape.StateTapeReader == true) {
            digitalWrite(PIN_BUILD_IN_LED, HIGH);
            delay(5000);
        }
        else {
            digitalWrite(PIN_BUILD_IN_LED, LOW);
        }*/
    #endif   

    input.pressedKeys = 0;
    cd.Update();
    
    cycle++;
    if (cycle > 1) {
        cycle = 0;


        #ifdef DEBUG
            if (input.pressedKeys != 0) {
                Serial.println(KeyToString(input.pressedKeys));
            }
            int state = input.GetCassetteState();
            if (state != OldState)
            {
                Serial.println(StateToString(state));
                OldState = state;
            }
            
        #endif   


        tape.Update();
        lcd.Update();
    }
  



    delay(40);

    if (tape.AutoRestart) {
        // a wee delay if user only hit forward/rewind during playback
        // otherwise white lever of mechanics is going crazy
        delay(200);
    }

 


}
