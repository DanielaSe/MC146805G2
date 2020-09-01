/***************************************************************************
 * 
 * Replacement for uP MC146805G2 in Philips 70CD555
 * 
 * Direct replacement for item 7501 - no changes on the uP PCB needed
 * CPU runs on 8.7MHz - Arduino is twice as fast, thus timing is different
 * and different as described in the service manual.
 * 
 * 
 * Todo: find a way to enable the oscillator without the original CPU
 * 
 * 
 * 
 **************************************************************************/


#define DEBUG;

#include "TPinLayout.h"


#include "TInputs.h"
#include "TCDController.h"
#include "TTapeController.h"
#include "TDisplay.h"

#ifdef DEBUG
    #include "TDebug.h"
#endif



TDisplay lcd = TDisplay();
TInputs input = TInputs();
TCDController cd = TCDController();
TTapeController tape = TTapeController(&lcd, &input);
TDebug debug = TDebug(&input, &cd, &tape, &lcd);

bool blink = false;
int cycle = 0;

void NewTrack()
{
    tape.NewTrack();
//    debug.IRQDetected();
}

void setup() {

    pinMode(PIN_IRQ, INPUT);
//    attachInterrupt(digitalPinToInterrupt(PIN_IRQ), NewTrack, FALLING);

    pinMode(PIN_BUILD_IN_LED, OUTPUT);

 //   debug.Begin();
    delay(10);
}




String KeyToString(int value) 
{
    switch (value)
    {

        case 0x0001 : return "Reverse Mode    ";
        case 0x0002 : return "Rec Mode        "; 
        case 0x0004 : return "Direction       "; 

        case 0x0008 : return "Play           ";
        case 0x0010 : return "Rewind         "; 
        case 0x0020 : return "Forward        "; 

        case 0x0040 : return "Stop/Clear     "; 
        case 0x0080 : return "Pause          "; 
        case 0x0100 : return "Rec            "; 

        case 0x0200 : return "Next           "; 
        case 0x0400 : return "Previous       ";
        case 0x0800 : return "Program        "; 

        case 0x1000 : return "CD Play        "; 
        case 0x2000 : return "CD Stop        "; 
        case 0x4000 : return "CD Pause       "; 

    }
    return "               "; 
}


void loop() {
//
//    digitalWrite(PIN_BUILD_IN_LED, HIGH);

  
    // Read Keys
  
    input.read();


    // CD Player
    if (input.pressedKeys == input.CD_PLAY) {
        cd.Play();
    }
    if (input.pressedKeys == input.CD_PAUSE) {
        cd.Pause();
    }
    if (input.pressedKeys == input.CD_STOP) {       
        cd.Stop();
        if (tape.Programming) tape.Pause();
    }


    // Tape Deck
    if (input.pressedKeys == input.CASS_PLAY) {
        tape.Play();
    }
    if (input.pressedKeys == input.CASS_PAUSE) {
        if (tape.Programming) lcd.ShowError();
        else tape.Pause();
    }
    if (input.pressedKeys == input.CASS_STOP) {
        if (tape.Programming) tape.ClearProgramm();
        else if (tape.PlayProgramm) tape.EndProgramm();
        else tape.Stop();
    }
    if (input.pressedKeys == input.CASS_NEXT) {
        tape.NextTrack();
    }
    if (input.pressedKeys == input.CASS_PREV) {
        tape.PreviousTrack();
    }
    if (input.pressedKeys == input.CASS_FORWARD) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else if (tape.GetDirection() > 0 ) {tape.WindLeft(); }
        else {tape.WindRight();}
    }
    if (input.pressedKeys == input.CASS_REWIND) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else if (tape.GetDirection() < 0 ) {tape.WindLeft(); }
        else {tape.WindRight();}
    }
    if (input.pressedKeys == input.CASS_REVERSE_MODE) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else tape.ToggleReverseMode();
    }   
    if (input.pressedKeys == input.CASS_PROG) {
        if (tape.Recording) lcd.ShowError();
        else tape.ProgrammKeyPressed();
    } 
    if (input.pressedKeys == input.CASS_DIRECTION) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else tape.ToggleDirection();
    }
    if (input.pressedKeys == input.CASS_REC_MODE) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else tape.ToggleRecordMode();
    }
    if (input.pressedKeys == input.CASS_REC) {
        if (tape.Programming || tape.Recording) lcd.ShowError();
        else tape.StartRecordMode();
    }
   
   if (input.cassetteState & input.IN_SLIDE_SERVO != 0) {
        digitalWrite(PIN_BUILD_IN_LED, HIGH);

   }
   else {
       digitalWrite(PIN_BUILD_IN_LED, LOW);
   }

    // Update objects

    
    cd.Update();
    
    // Timing, frames & write to screen
    delay(20);

    cycle++;
    if (cycle > 1) {
        cycle = 0;


        tape.Update();
        lcd.Update();
    }
  

        #ifdef DEBUG
     //       debug.update();
        #endif

//   delay(500);

  //  digitalWrite(PIN_BUILD_IN_LED, LOW);
    
    delay(20);


 


}
