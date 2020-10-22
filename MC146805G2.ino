/***************************************************************************
 * 
 * Replacement for uP MC146805G2 in Philips 70CD555 
 * Philips Code 4822 157 50967
 * 
 * 
 * 
 *      www.70cd555.com         
 *      Daniela Sefzig, Oct.2020
 * 
 * 
 * 
 * Direct replacement for item 7501. Use at your own risk!
 * 
 * Requires the following changings on the uProc panel:
 * 
 * - put a socket instead of the MC146805G2 and insert replacement PCB
 * 
 * - Revision A 
 *   requires an additional small clock PCB for the CD-Player CPU with 4MHz crystal
 *   (powering the clock with inverters was not stable, maybe retry with the next revision)
 *   Remove the bridge close to pin 15 when using the replacement clock
 * 
 * - Revision B
 *   In progress...
 * 
 * 
 * The original CPU runs on 8.7MHz - The AT1284P is almost as twice as fast with 16MHz, 
 * thus the timing has to be different. Only one key press at a time is supported, except the demo keys.
 * 
 * 
 * What is different to the orignal - because I liked to improve the handling
 * 
 * Changed behaviour:
 * - Music search supports up to 9 tracks when pressing next/Previous
 *   Original behaviour: only next or previous track
 * 
 * - Programming works differently. The tape rewinds when you start
 *   the program, not before. Also side B is supported! Therefore, up to 40 tracks can 
 *   be programmed.
 *   Original behaviour: 20 tracks only one side
 * 
 *  - The button "record" can be used to enable or disable record mode. In addition, the Stop key 
 *   always disables the record mode.
 *   Original behaviour: Record button only activates record mode
 * 
 * New features:
 * - When pressing record and the tape reader recognises the tape is
 *   at the very beginning it moves forward until the tape really starts.
 *   Avoids to record music on the non magnetic part of the tape. During this procedure
 *   record is blinking
 * 
 * - In Record-Mode the reverse key only allows A->B and does not show other reverse modes
 * 
 * - In case of rewind, when the tape moves to the very end it moves back forward a little bit
 *   to avoid being on a splice and the tape reader does not recognise the transparent tape
 * 
 * - Auto-Record mode shows the number of recorded tracks. Always wondered why Philips was not using
 *   the two digits and always displays 00?
 * 
 * 
 * 
 * Known limitations:
 * - Demo mode only supports to check the keys (eq. demo mode 0)
 * 
 * - In some cases on some cd's the new track irq does not fire. a possible reason might
 *   be the fact that the cpu's are not synchronised
 * 
 * 
 * 
 * 
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
long IgnoreCDEvent = 0;
long IgnoreNewTrackEvent = 0;

void NewTrack()
{
    long ms = millis();
    if (ms < IgnoreNewTrackEvent) return;
    IgnoreNewTrackEvent = ms + 4000;

    tape.NewTrack();
    #ifdef DEBUG
        Serial.println("+IRQ:NewTrack");
    #endif
}

void CDPlayerStartsNewTrack()
{
    #ifdef DEBUG
        Serial.println("+IRQ:CDPlayerStartsNewTrack");
    #endif
    // we need to ignore more rising signals when they are too
    // short behind. We do not want to handle them twice.
    // Since the CPUs are not synced anymore the behaviour is 
    // different and we need to consider this.
    long ms = millis();
    if (ms < IgnoreCDEvent) return;
    IgnoreCDEvent = ms + 1000;

    if (tape.IsOnAutoRecord() && tape.IsRecording()) 
    {
        cd.Pause(true);
        tape.NewAutoRecordTrackStarted();
    }


}


void setup() 
{
    attachPinChangeInterrupt(9, NewTrack, FALLING);
    attachPinChangeInterrupt(18, CDPlayerStartsNewTrack, RISING);
    
    pinMode(PIN_BUILD_IN_LED, OUTPUT);

    delay(10);
    #ifdef DEBUG
        Serial.begin(9600); 
        Serial.println("");
        Serial.println("");
        Serial.println("Ready.");
    #endif
}


bool CHECK_BIT(int value, int b) 
{
    return value & b;
}

String StateToString(int value) 
{
    String result = "+Tape: " + String(value,DEC) + " ";   
    if (CHECK_BIT(value, input.IN_HEAD_SERVO)) { result += "HeadServo "; }
    if (CHECK_BIT(value, input.IN_PAUSE_CASS)) { result += "Pause "; }
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

        case 0x0001 : return "+Key:Reverse Mode";
        case 0x0002 : return "+Key:Rec Mode"; 
        case 0x0004 : return "+Key:Direction"; 

        case 0x0008 : return "+Key:Play";
        case 0x0010 : return "+Key:Rewind"; 
        case 0x0020 : return "+Key:Forward"; 

        case 0x0040 : return "+Key:Stop/Clear"; 
        case 0x0080 : return "+Key:Pause"; 
        case 0x0100 : return "+Key:Rec"; 

        case 0x0200 : return "+Key:Next"; 
        case 0x0400 : return "+Key:Previous";
        case 0x0800 : return "+Key:Program"; 

        case 0x1000 : return "+Key:CD Play"; 
        case 0x2000 : return "+Key:CD Stop"; 
        case 0x4000 : return "+Key:CD Pause"; 

    }
    return "+Key:Unknonw"; 
}


void loop() 
{


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
            if (tape.PlayProgramm || tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break;
            tape.Play(); 
            break;

        case input.CASS_PAUSE:
            if (tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break; 
            if (tape.PlayProgramm || tape.Programming) lcd.ShowError();
            else tape.Pause();
            break;

        case input.CASS_STOP:
            if (tape.Programming) tape.ClearProgramm();
            else if (tape.PlayProgramm) tape.EndProgramm();
            else tape.Stop();
            break;

        case input.CASS_REC_MODE:
            if (tape.PlayProgramm || tape.Programming || tape.IsRecording()) lcd.ShowError();
            else tape.ToggleRecordMode();
            break;
            
        case input.CASS_REC:
            if (tape.PlayProgramm || tape.Programming || tape.IsRecording()) lcd.ShowError();
            else tape.StartRecordMode();
            break;

        case input.CASS_REVERSE_MODE:
            if (tape.PlayProgramm || tape.Programming) lcd.ShowError();
            else tape.ToggleReverseMode();
            break;

        case input.CASS_NEXT:
        case input.CASS_PREV:
        case input.CASS_FORWARD:
        case input.CASS_REWIND:
        case input.CASS_PROG:
        case input.CASS_DIRECTION:
            if (tape.IsOnRecord()) lcd.ShowError();
            break;
    }


    // other tape deck keys only if record is not active
    if (!tape.IsOnRecord() && !tape.PlayProgramm) {
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
                if (tape.Programming || tape.IsRecording()) lcd.ShowError();
                else if (tape.GetDirection() < 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_REWIND:
                if (tape.Programming || tape.IsRecording()) lcd.ShowError();
                else if (tape.GetDirection() > 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_PROG:
                if (tape.IsRecording()) lcd.ShowError();
                else tape.ProgrammKeyPressed(); 
                break;

            case input.CASS_DIRECTION:
                if (tape.Programming || tape.IsRecording()) lcd.ShowError();
                else tape.ToggleDirection();
                break;

        }
    }




    if (tape.AutoRestart && !tape.SearchTrack && input.pressedKeys != input.CASS_FORWARD && input.pressedKeys != input.CASS_REWIND) {
        #ifdef DEBUG
            Serial.println("Autorestart");
        #endif
        tape.Play();
    }
    

    // for easier debugging we can use the internal led to show states
    #ifdef DEBUG
     /*   if (digitalRead(24) == HIGH) {
            digitalWrite(PIN_BUILD_IN_LED, HIGH);
            delay(1000);
        }
        else {
            digitalWrite(PIN_BUILD_IN_LED, LOW);
        }*/
    #endif   

    input.pressedKeys = 0;
    tape.Update();
    cd.Update();
    tape.Update();

    cycle++;
    if (cycle > 1) {
        cycle = 0;


        #ifdef DEBUG
            if (input.pressedKeys != 0) {
                Serial.println(KeyToString(input.pressedKeys));
            }


            tape.GetState();
            int state = tape.GetCurrentState();
            if (state != OldState)
            {
                Serial.println(StateToString(state));
                OldState = state;
            }
            
        #endif   

        
        

        lcd.Update();
    }
  

    
    delay(20);

    if (tape.AutoRestart) {
        // a wee delay if user only hit forward/rewind during playback
        // otherwise white lever of mechanics is going crazy
        delay(200);
    }

 


}
