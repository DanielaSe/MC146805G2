
/***************************************************************
 * 
 *                                             SYNC  AUTO  REC
 *                                             (26)  (27)  (28)
 * 
 *  (17)  (18)  (19)        (09)             (01)      
 *   |_\  <--   /_         ######           ######
 *     /  -->   \ |       #      #         #      #        (29)
 *       REPEAT(20)   (14)#      #(10) (06)#      #(02)    ***
 *                        # (15) #         # (07) #       *   #(30)
 *                         ######           ######        *###(31)
 *                        #      #         #      #       *  
 *                    (13)#      #(11) (05)#      #(03)    ###
 *                        #      #         #      #        (32)
 *    /__________\(16)     ######           ######
 *   /            \         (12)             (04)       ERROR (08)
 *   \ __________ / 
 *    \          /
 * 
 *   <   << PLAY >>  >
 *  (21)(22)(23)(24)(25)
 * 
 * ************************************************************/


#include "TDisplay.h"
#include "Arduino.h"
#include "TPinLayout.h"
#include "defines.h"




TDisplay::TDisplay(void) {

    pinMode(PIN_DISPLAY_CLOCK, OUTPUT);
    pinMode(PIN_DISPLAY_DATA, OUTPUT);  

    ShowNone();

    Clear();
    #ifdef START_WITH_SCROLLING
        init = 1;
    #endif
};


void TDisplay::BlinkRecord(bool value) {
    RecordMustBlink = value;
}

void TDisplay::BlinkPlay(bool value) {
    PlayMustBlink = value;
}

void TDisplay::pulseClock()
{
    digitalWrite(PIN_DISPLAY_CLOCK, LOW); 
    digitalWrite(PIN_DISPLAY_CLOCK, HIGH); 
}

void TDisplay::ShowMinus(bool value)
{
    minus = value;
}

void TDisplay::ShowDigit9(int value)
{
    minus = false;
    if (value < 0) {
        value *= -1;
        minus = true;
    }
    digit = value;
    RemoveText();
}

void TDisplay::ShowDigit(int value)
{
    digit = value;
    RemoveText();
}

void TDisplay::Programm(int value, int m)
{
    programm = true;
    programmPlace = m;
    programmFlashCount = 10;
    digit = value;

    switch(value) 
    {
        case 0: ShowText("C0"); break;
        case -1: ShowText("P1"); break;
        case -2: ShowText("P2"); break;
        case -3: ShowText("P3"); break;
        default:
            RemoveText();              
    }

}

bool TDisplay::WaitingForInput()
{
    return programm;
}


void TDisplay::ShowError(short code)
{
    ErrorCode = code;
    blinkError = 4;
    #ifdef debug
        Serial.print("Error ");
        Serial.println(code);
    #endif
}




void TDisplay::Clear()
{
    programm = false;
    paused = false;
    display = _ARROW + _REP_M + _DIR_R;
    digit = 0;
    minus = false;
    PlayMustBlink = false;
    RemoveText();
}


void TDisplay::Play()
{
    paused = false;
    display &= _FULL - _WIND_RIGHT - _WIND_LEFT;
    display |= _PLAY;
    PlayMustBlink = false;
}



void TDisplay::Stop()
{
    programm = false;
    paused = false;
    display &= _FULL - _PLAY - _WIND_RIGHT - _WIND_LEFT - _PRG_ALL;
    digit = 0;
    minus = false;
    PlayMustBlink = false;
    RemoveText();
}



void TDisplay::Pause()
{
    paused = true;
    display &= _FULL - _WIND_RIGHT - _WIND_LEFT;
    PlayMustBlink = true;

}



void TDisplay::WindLeft()
{
    display &= _FULL - _WIND_RIGHT;  
    display |= _WIND_LEFT;
}



void TDisplay::WindRight()
{
    display &= _FULL - _WIND_LEFT;  
    display |= _WIND_RIGHT;
}

void TDisplay::DirectionLeft()
{
    display &= _FULL - _DIR_R;  
    display |= _DIR_L;
}
void TDisplay::DirectionRight()
{
    display &= _FULL - _DIR_L;  
    display |= _DIR_R;
}


void TDisplay::StopWinding()
{
    display &= _FULL - _WIND_RIGHT - _WIND_LEFT;  
}


void TDisplay::RecordMode(int rec)
{
    display &= _FULL - _SYNC - _AUTO - _REC;
    switch(rec) 
    {
        case 1 : display |= _REC; break;
        case 2 : display |= _REC + _SYNC; break;
        case 3 : display |= _REC + _AUTO; break;
    }  
}


void TDisplay::ReverseMode(int rm)
{
    display &= _FULL - _REP_L - _REP_R - _REPEAT;
    switch(rm) 
    {
        case 1 : display |= _REPEAT; break;
        case 2 : display |= _REP_R; break;
        case 3 : display |= _REP_R + _REP_L ; break;
    }  
}

void TDisplay::ShowAll() {
    digitalWrite(PIN_DISPLAY_DATA, HIGH);
    pulseClock();
    digitalWrite(PIN_DISPLAY_DATA, HIGH);
    for (int x = 1; x <= segmentCount; x++) {
                 
        pulseClock();  
    }
}

void TDisplay::ShowNone() {
    digitalWrite(PIN_DISPLAY_DATA, HIGH);
    pulseClock();
    digitalWrite(PIN_DISPLAY_DATA, LOW);    
    for (int x = 1; x <= segmentCount; x++) {
             
        pulseClock();  
    }
}


void TDisplay::RemoveText()
{
    HasText = false;   
}


void TDisplay::ShowText(char s[2])
{
    text[0] = s[0];
    text[1] = s[1];
    HasText = true;
}

int TDisplay::GetTextSegment(char c)
{
    switch (c)
    {
        case '0': return segmentCode[0];
        case '1': return segmentCode[1];
        case '2': return segmentCode[2];
        case '3': return segmentCode[3];
        case '4': return segmentCode[4];
        case '5': return segmentCode[5];
        case '6': return segmentCode[6];
        case '7': return segmentCode[7];
        case '8': return segmentCode[8];
        case '9': return segmentCode[9];
        case 'C': return segmentChars[0];
        case 'P': return segmentChars[1];
    }
    return 0;
}



void TDisplay::Update()
{
    int blinkChanged = false;
    blinkDelay--;
    if (blinkDelay < 0) {
        blinkDelay = BLINK_DELAY;
        blink = !blink;
        blinkChanged = true;
    }
    
    if (paused) {
        if (blink)  display |= _PLAY;
        else  display &= _FULL - _PLAY;  
    }
    if (programm) {
        display &= _FULL - _PRG_ALL;  
        if (blink) {    
            if (programmFlashCount > 1) {
                programmFlashCount--;
                switch (programmPlace)
                {
                    case 0: display |= _BLINK; break;
                    case 1: display |= _PROGRAMM; break;
                    case 2: display |= _PRG_CLEAR; break;                  
                }
            }
            else {
                programm = false;
            }

        }
    }





    long value = display;

    display &= _FULL - _ERROR;
    

    if (init > 0) {
        value &= _FULL - (segmentCode[8] << 8) - segmentCode[8];

        int pos = init - 1;
        if (pos < 11) value += (scroll[pos] << 8);  

        pos++;
        if (pos < 11)  value += scroll[pos];

        init++;
        if (init >= 12)  init = 0;

        delay(150);
    }
    else {
        if (digit < 0) {
            value &= _FULL - (segmentCode[8] << 8) - segmentCode[8];
            value += _LINE + (_LINE << 8);
        }
        else {
            int d1 = (digit / 10);
            int d2 = digit - (d1 * 10);

            if (d1 == 0 && d2 > 0) {
                value &= _FULL - (segmentCode[8] << 8);
                value += segmentCode[d2];
            }
            else {
                value += (segmentCode[d1] << 8);
                value += segmentCode[d2];
            }

            if (minus) value += _D2_G;
        }


    }


    if (blinkError > 0) {  
        if (blinkChanged) blinkError--; 
        if (blink) display |= _ERROR;
        if (ErrorCode > 0) {
            value &= _FULL - (segmentCode[8] << 8) - segmentCode[8];
            value += segmentCode[ErrorCode] + (ErrorSegment << 8);
        }
    }

    if (HasText) {

        value &= _FULL - (segmentCode[8] << 8) - segmentCode[8];
        value += GetTextSegment(text[1]) + (GetTextSegment(text[0]) << 8);

    }

    if (RecordMustBlink) {
        if (!blink) value &= _FULL - _REC;  
        else  value |= _REC;
    }

    if (PlayMustBlink) {
        if (!blink)  value &= _FULL - _PLAY;  
        else  value |= _PLAY;
    }
    value |= _ARROW;

    
  





    digitalWrite(PIN_DISPLAY_DATA, HIGH);
    pulseClock();

    
    long bit = 1;
    for (int x = 1; x <= segmentCount; x++) {

        if ((value & bit) != 0)  digitalWrite(PIN_DISPLAY_DATA, HIGH);   
        else  digitalWrite(PIN_DISPLAY_DATA, LOW);
        bit = bit << 1;        
        pulseClock();  
    }
}
