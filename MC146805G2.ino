/***************************************************************************
 * 
 * Replacement for uP MC146805G2 in Philips 70CD555 
 * Philips Code 4822 157 50967
 * 
 * 
 * 
 *      www.70cd555.com         
 *      (c) Daniela Sefzig, Dec.2020
 * 
 * 
 * 
 * Replacement for item 7501. Solves bugs in the original firmware and adds more features.
 * Use at your own risk!
 * 
 * 
 * 
 * Requires the following changings on the uProc panel:
 * 
 * - put in a DIP-40 socket instead of the MC146805G2 and insert replacement PCB
 * - no other changes required
 * 
 * 
 * 
 * Revisions
 * - Revision A
 *   First release
 * 
 * - Revision A 
 *   requires an additional small clock PCB for the CD-Player CPU with 4MHz crystal
 *   (powering the clock with inverters was not stable, maybe retry with the next revision)
 *   Remove the bridge close to pin 15 when using the replacement clock
 * 
 * - Revision B
 * 
 * - Revision C
 *   Never built
 * 
 * - Revision D
 *   Clock for CD CPU was prepared by build-in timer 3. Unfortunately it was not working stable
 * 
 * - Revision E
 *   Digital counter added and SMD 1206 components used
 * 
 * - Revision F
 *   PCB of digital counter slightly Changed
 *   Replaced timer 3 for the CD CPU clock with a hardware based solution with two D-FlipFlops 
 *   to reduce the 16MHz to 4MHz  
 * 
 * 
 * 
 * Error Codes
 *   Since the original firmware hangs when failed to deal with the mechanics the new firmware
 *   contains error routines and show a specific error code as well as do not block the execution
 *   of the application
 *      E0  wrong user input
 *      E1  failed to initialise the tape mechanics
 *      E2  failed to turn the head during autoreverse
 *      E3  failed to switch to record mode
 *      E4  failed to switch servo head
 * 
 * The original CPU runs on 5.7MHz - The AT1284P is way faster with 16MHz, 
 * thus the timing has to be different. Only one key press at a time is supported, except the demo keys.
 * 
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
 * - The button "record" can be used to enable or disable record mode. In addition, the Stop key 
 *   always disables the record mode.
 *   Original behaviour: Record button only activates record mode
 * 
 * - Pause just stops the motor
 * 
 * 
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
 *   to avoid being on a splice and the tape reader does not recogn‚ise the transparent tape
 * 
 * - Auto-Record mode shows the number of recorded tracks. Always wondered why Philips was not using
 *   the two digits and always displays 00?
 * 
 * 
 * 
 * 
 * Using the digital counter adds the following features:
 * - OLED Display with minutes and seconds
 * - Automatic reset to 0:00 when rewind to the beginning of the tape
 * - Select the tape length C30, C60 or C90
 * - Jump to position 0
 * - Store up to three positions
 * - Select with previous key to select the position to jump to and click
 *      a) the program key to go to the position and stop or
 *      b) the play key to go to the position and play
 * - Save the selected repeat mode
 * - Save the last position
 * - Display side A or B
 * - Philips logo from the eighties on startup
 * - Screensaver, swith off the display after 5 minutes of no action
 * - Set the screen coordinates with help of the demo mode 02
 * - Optional clock (not implemented yet)
 * 
 * 
 * Demo Modes
 * 01 - Check buttons, same as original firmware
 * 02 - Set screen position of digital counter
 * 03 - Reserved
 * Leave the demo mode by pressing the STOP key
 * 
 * 
 * 
 * Known limitations:
 * - Demo mode only supports to check the keys (eq. demo mode 0)
 * 
 * - In some cases on some cd's the new track irq does not fire. Maybe depends on Audio CD? 
 *   Tests with revision E have shown that with the more or less synched clock the signal
 *   gets fired correctly
 * 
 * 
 **************************************************************************/



#include "TPinLayout.h"
#include "PinChangeInterrupt.h"
#include "TInputs.h"
#include "TCDController.h"
#include "TTapeController.h"
#include "TDisplay.h"
#include "TCounter.h"
#include "defines.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



TDisplay lcd = TDisplay();
TInputs input = TInputs();
TCDController cd = TCDController();
TCounter counter = TCounter();
TTapeController tape = TTapeController(&lcd, &input, &counter);



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
    #ifdef USE_BUILD_IN_LED
        pinMode(PIN_BUILD_IN_LED, OUTPUT);
  //      digitalWrite(PIN_BUILD_IN_LED, HIGH);
    #else
        pinMode(PIN_BTN_COUNTER, INPUT);
    #endif

    delay(10);
    #ifdef DEBUG
        Serial.begin(9600); 
        Serial.println("");
        Serial.println("");
        Serial.println("Ready.");
    #endif

    counter.Init();
    
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

    int dm = input.GetDemoMode();
    // demo mode gives several options to check the function of the tape deck
    // this method is currently not equal to the original code
    if (dm > 0) {
     
        switch (dm) { 
           case 1: 
                tape.Reset();          
                if (input.pressedKeys != 0 && input.pressedKeys != input.CD_PLAY) {
                    lcd.ShowNone();
                    lcd.ShowNone();
                    lcd.ShowNone();
                    delay(500);
                }
                else lcd.ShowAll();
                tape.Update();
                delay(100);
                return;

            case 2:
                delay(150);
                lcd.ShowDigit(2);
                lcd.Update();
                counter.ConfigScreenPosition(true, input.ReadKeySet1(), input.ReadKeySet2());
                break;
            case 3:
                delay(150);
                counter.ConfigScreenPosition(false, 0, 0);
                lcd.ShowDigit(3);
                lcd.Update();               
                break;
            case 4:
                counter.Reset();
                lcd.Clear();
                input.DisableDemoMode();
                break;

        }
        input.pressedKeys = 0;
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
            if (tape.IsOnAutoRecord() || tape.IsOnSyncRecord()) tape.PauseAfterFourSeconds();    

            break;
            
        case input.CD_STOP:
            cd.Stop();
            if (tape.IsOnAutoRecord() || tape.IsOnSyncRecord()) tape.PauseAfterFourSeconds();      
            break;

        // Tape Deck
        case input.CASS_PLAY:
            if (tape.PlayProgramm || tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break;
            tape.Play(); 
            break;

        case input.CASS_PAUSE:
            if (tape.IsOnSyncRecord() || tape.IsOnAutoRecord()) break; 
            if (tape.PlayProgramm || tape.Programming) lcd.ShowError(0);
            else tape.Pause();
            break;

        case input.CASS_STOP:
            
            if (tape.Programming) tape.ClearProgramm();
            else if (tape.PlayProgramm) tape.EndProgramm();
            else tape.Stop();
            break;

        case input.CASS_REC_MODE:
            if (tape.PlayProgramm || tape.Programming || tape.IsRecording()) lcd.ShowError(0);
            else tape.ToggleRecordMode();
            break;
            
        case input.CASS_REC:
            if (tape.PlayProgramm || tape.Programming || tape.IsRecording()) lcd.ShowError(0);
            else tape.StartRecordMode();
            break;

        case input.CASS_REVERSE_MODE:
            if (tape.PlayProgramm || tape.Programming) lcd.ShowError(0);
            else tape.ToggleReverseMode();
            break;

        case input.CASS_NEXT:
        case input.CASS_PREV:
        case input.CASS_FORWARD:
        case input.CASS_REWIND:
        case input.CASS_PROG:
        case input.CASS_DIRECTION:
            if (tape.IsOnRecord()) lcd.ShowError(0);
            break;
    }


    // other tape deck keys only if record is not active
   if (tape.ReadyForInput()) {
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
                if (tape.Programming || tape.IsRecording()) lcd.ShowError(0);
                else if (tape.GetDirection() < 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_REWIND:
                if (tape.Programming || tape.IsRecording()) lcd.ShowError(0);
                else if (tape.GetDirection() > 0 ) tape.WindLeft(); 
                else tape.WindRight();
                break;

            case input.CASS_PROG:
                if (tape.IsRecording()) lcd.ShowError(0);
                else tape.ProgrammKeyPressed(); 
                break;

            case input.CASS_DIRECTION:
                if (tape.Programming || tape.IsRecording()) lcd.ShowError(0);
                else tape.ToggleDirection();
                break;

        }
    }
    else {
        // Todo: figure out which keys should show an error
        switch(input.pressedKeys) {
      //      case input.CASS_PREV:
      //          lcd.ShowError(0);
        }
    }
      



    if (tape.AutoRestart && !tape.SearchTrack && input.pressedKeys != input.CASS_FORWARD && input.pressedKeys != input.CASS_REWIND) {
        #ifdef DEBUG
            Serial.println(">Autorestart");
        #endif
        tape.Play();
    }
    

    // for easier debugging we can use the internal led to show states
    #ifdef DEBUG
    
   /*    if (digitalRead(_PD3) == HIGH) {
            digitalWrite(PIN_BUILD_IN_LED, HIGH);
            delay(250);
        }
        else {
            digitalWrite(PIN_BUILD_IN_LED, LOW);
         //   delay(1000);
        }*/
    #endif   

    input.pressedKeys = 0;
    tape.Update();
    cd.Update();
    tape.Update();
    

    cycle++;
    if (cycle > 1) {
        cycle = 0;

  //      if (tape.GetTapeReader()) digitalWrite(PIN_BUILD_IN_LED, HIGH);
  //      else digitalWrite(PIN_BUILD_IN_LED, LOW);


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
    counter.Update();
  


}
