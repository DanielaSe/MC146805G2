
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




TDisplay::TDisplay(void) {

    pinMode(PIN_DISPLAY_CLOCK, OUTPUT);
    pinMode(PIN_DISPLAY_DATA, OUTPUT);   
    Clear();
};



void TDisplay::pulseClock()
{
    digitalWrite(PIN_DISPLAY_CLOCK, LOW); 
    digitalWrite(PIN_DISPLAY_CLOCK, HIGH); 
}


void TDisplay::ShowDigit(int value)
{
    digit = value;
}

void TDisplay::Programm(int value, int m)
{
    programm = true;
    programmPlace = m;
    programmFlashCount = 10;
    digit = value;
}

bool TDisplay::WaitingForInput()
{
    return programm;
}


void TDisplay::ShowError()
{
    blinkError = 4;
}




void TDisplay::Clear()
{
    programm = false;
    paused = false;
    display = _ARROW + _REP_M + _DIR_R;
    digit = 0;
}


void TDisplay::Play()
{
    paused = false;
    display &= _FULL - _WIND_RIGHT - _WIND_LEFT;
    display |= _PLAY;
}



void TDisplay::Stop()
{
    programm = false;
    paused = false;
    display &= _FULL - _PLAY - _WIND_RIGHT - _WIND_LEFT - _PRG_ALL;
    digit = 0;
}



void TDisplay::Pause()
{
    paused = true;
    display &= _FULL - _WIND_RIGHT - _WIND_LEFT;
    if (!blink) {
        display &= _FULL - _PLAY;  
    }
    else {
        display |= _PLAY;
    }
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
        case 3 : display |= _REC + _SYNC + _AUTO; break;
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



void TDisplay::Update()
{

    blink = !blink;
    if (paused) {
        if (blink) {
            display |= _PLAY;
        }
        else {
            display &= _FULL - _PLAY;  
        }
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

    display &= _FULL - _ERROR;
    if (blinkError > 0) {     
        if (blink) {   
            blinkError--;
            display |= _ERROR;
        } 
    }



    long value = display;

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
    }




    digitalWrite(PIN_DISPLAY_DATA, HIGH);
    pulseClock();

    long bit = 1;
    for (int x = 1; x <= segmentCount; x++) {

        if ((value & bit) != 0) {
            digitalWrite(PIN_DISPLAY_DATA, HIGH);   
        }
        else {
            digitalWrite(PIN_DISPLAY_DATA, LOW);
        }
        bit = bit << 1;        
        pulseClock();  
    }
}
