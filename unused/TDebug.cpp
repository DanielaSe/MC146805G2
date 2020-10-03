#include "TInputs.h"
#include "Arduino.h"
#include "TPinLayout.h"
#include "TDebug.h"
#include "TDisplay.h"
#include "TCDController.h"
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include <Keypad.h>

byte CharPlay[8] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01100,
  B01000,
  B00000,
};
byte CharStop[8] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B00000,
};
byte CharPause[8] = {
  B00000,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B00000,
};






TDebug::TDebug(TInputs *_Inputs, TCDController *_CD, TTapeController *_Tape, TDisplay *_Display) {

    inputs = _Inputs;
    cd = _CD;
    tape = _Tape;
    display = _Display;


    keyboard = new Keypad(makeKeymap(KEYS), KeyRowPins, KeyColPins, KeyMatrix, KeyMatrix);
}

void TDebug::Begin()
{
    lcd = new LiquidCrystal_I2C(0x27, 20, 4);
    lcd->init();

    lcd->createChar(0, CharPlay);
    lcd->createChar(1, CharStop);
    lcd->createChar(2, CharPause);
    lcd->backlight();

    lcd->clear();
    lcd->setCursor(0,0);
    lcd->print("Ready.");
}

void TDebug::IRQDetected()
{
  /*  lcd->setCursor(0,3);
    lcd->print("IRQ"); 
    delay(500);*/
}


void TDebug::update()
{
    lcd->setCursor(0,0);

    int key = inputs->GetPressedKey();

    if (key == 0) { 
        String t = String(millis() / 1000,DEC); 
        while (t.length() < 16) { t += " ";}
        lcd->print( t); 
        }
    if (key == inputs->CASS_REVERSE_MODE)   { lcd->print( "Key:Reverse Mode"); }
    if (key == inputs->CASS_REC_MODE)       { lcd->print( "Key:Rec Mode    "); }
    if (key == inputs->CASS_DIRECTION)      { lcd->print( "Key:Direction   "); }
    if (key == inputs->CASS_PLAY)           { lcd->print( "Key:Play        "); }
    if (key == inputs->CASS_REWIND)         { lcd->print( "Key:Rewind      "); }
    if (key == inputs->CASS_FORWARD)        { lcd->print( "Key:Forward     "); }
    if (key == inputs->CASS_STOP)           { lcd->print( "Key:Stop        "); }
    if (key == inputs->CASS_PAUSE)          { lcd->print( "Key:Pause       "); }
    if (key == inputs->CASS_REC)            { lcd->print( "Key:Rec         "); }
    if (key == inputs->CASS_NEXT)           { lcd->print( "Key:Next        "); }
    if (key == inputs->CASS_PREV)           { lcd->print( "Key:Previous    "); }
    if (key == inputs->CASS_PROG)           { lcd->print( "Key:Program     "); }
    if (key == inputs->CD_PLAY)             { lcd->print( "Key:CD Play     "); }
    if (key == inputs->CD_STOP)             { lcd->print( "Key:CD Stop     "); }
    if (key == inputs->CD_PAUSE)            { lcd->print( "Key:CD Pause    "); }


    // CD functions
    lcd->setCursor(0,1);
    String a = String(cd->GetOutputPins(),BIN) + " ";
    while (a.length() <= 5) { a = "0" + a;}
    lcd->print("CD:" + a);
/*
    int s = -1;
    lcd->setCursor(19,1);
    if (cd->Playing()) {
        s = 0;
    }
    if (cd->Paused()) {
        s = 1;
    }
    if (s >= 0) {
        lcd->print(char(s));
    }
    else {
        lcd->print(" ");
    }
*/
/*
    lcd->setCursor(0,3);  
    String prg = String(tape->ProgrammPosition,DEC);
    prg += " " + String(tape->ProgrammedTracks(),DEC);
    if (tape->Programming == true) {
        prg += " PRG";
    }
    if (tape->PlayProgramm == true) {
        prg += " PLY";
    }
    if (display->WaitingForInput()) {
        prg += " W";
    }
    if (tape->SearchTrack == true) {
        prg += " S";
    }
    prg += "   ";
    lcd->print(prg);   

    String pl = "";
    for (byte x = 1; x<=20;x++) {
        if (tape->Programm[x]) pl += "1";
        else pl += "0";
    }
    lcd->setCursor(0,2); 
    lcd->print(pl);    

    lcd->cursor();
    int p = tape->ProgrammPosition - 1;
    if (p >= 0) lcd->setCursor(p,2); 

    return;
*/

    // tape functions

    lcd->setCursor(0,2);
    String b = String(inputs->cassetteState,BIN);
    while (b.length() <= 6) { b = "0" + b;}
    lcd->print("TAPE:" + b);
/*
    s = -1;
    lcd->setCursor(19,2);
    if (tape->Playing()) {
        s = 0;
    }
    if (tape->Paused()) {
        s = 1;
    }
    if (s >= 0) {
        lcd->print(char(s));
    }
    else {
        lcd->print(" ");
    }

*/

    char k = keyboard->getKey();

    if (k != NO_KEY){

        if (cmd.length() > 3 && k == '#') 
        {
            
            bool s = cmd[3]== '1';
            if (cmd[0]== 'A') {
                switch(cmd[1])
                {
                    case '0': digitalWrite(PIN_PA0, s); break;
                    case '1': digitalWrite(PIN_PA1, s); break;
            //        case '2': digitalWrite(PIN_PA2, s); break;
            //        case '3': digitalWrite(PIN_PA3, s); break;
                    case '4': digitalWrite(PIN_PA4, s); break;
                    case '5': digitalWrite(PIN_PA5, s); break;
                    case '6': digitalWrite(PIN_PA6, s); break;
                    case '7': digitalWrite(PIN_PA7, s); break;
                }
            }
            if (cmd[0]== 'B') {
                switch(cmd[1])
                {
                    case '0': digitalWrite(PIN_PB0, s); break;
                    case '1': digitalWrite(PIN_PB1, s); break;
                    case '2': digitalWrite(PIN_PB2, s); break;
                    case '3': digitalWrite(PIN_PB3, s); break;
                    case '4': digitalWrite(PIN_PB4, s); break;
    //                case '5': digitalWrite(PIN_PB5, s); break;
    //                case '6': digitalWrite(PIN_PB6, s); break;
    //                case '7': digitalWrite(PIN_PB7, s); break;
                }
            }            
            if (cmd[0]== 'C') {
                switch(cmd[1])
                {
    //                case '0': digitalWrite(PIN_PC0, s); break;
    //                case '1': digitalWrite(PIN_PC1, s); break;
    //                case '2': digitalWrite(PIN_PC2, s); break;
    //                case '3': digitalWrite(PIN_PC3, s); break;
    //                case '4': digitalWrite(PIN_PC4, s); break;
    //                case '5': digitalWrite(PIN_PC5, s); break;
    //                case '6': digitalWrite(PIN_PC6, s); break;
    //                case '7': digitalWrite(PIN_PC7, s); break;
                }
            }
            if (cmd[0]== 'D') {
                switch(cmd[1])
                {
                    case '0': digitalWrite(PIN_PD0, s); break;
                    case '1': digitalWrite(PIN_PD1, s); break;
                    case '2': digitalWrite(PIN_PD2, s); break;
                    case '3': digitalWrite(PIN_PD3, s); break;
                    case '4': digitalWrite(PIN_PD4, s); break;
                    case '5': digitalWrite(PIN_PD5, s); break;
                    case '6': digitalWrite(PIN_PD6, s); break;
                    case '7': digitalWrite(PIN_PD7, s); break;
                }
            }
            cmd = "";
        }
        else {
            if (k == '#') {
                cmd = "";
            }
            else {
                cmd = cmd + k;      
            }
            
        }

        

    }

    String c = ">" + cmd;
    while (c.length() < 6) { c += " ";}
    lcd->setCursor(15,0);
    lcd->print(c);

    

}
