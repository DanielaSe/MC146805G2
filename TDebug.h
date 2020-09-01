#ifndef TDEbug_h
#define TDEbug_h


#include "TInputs.h"
#include "TDisplay.h"
#include "TCDController.h"
#include "TTapeController.h"
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include <Keypad.h>

class TDebug
{

    public:

        TDebug(TInputs *_Inputs, TCDController *_CD, TTapeController *_Tape, TDisplay *_Display);

        void update();
        void Begin();

        void IRQDetected();


    private:
        TInputs *inputs;
        TTapeController *tape;
        TCDController *cd;
        LiquidCrystal_I2C *lcd;
        TDisplay *display;
        Keypad *keyboard;
  
        
        String cmd = "";


        char KEYS[4][4] = {

            {'1','2','3','A'},
            {'4','5','6','B'},
            {'7','8','9','C'},
            {'*','0','#','D'}
        };
        byte KeyColPins[4] = {22,24,26,28}; 
        byte KeyRowPins[4] = {23,25,27,29}; 
        const byte KeyMatrix = 4;
  
        
};


#endif
