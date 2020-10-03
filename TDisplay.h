

#ifndef TDisplay_h
#define TDisplay_h


class TDisplay
{

    public:

        TDisplay(void);

        
 
        void Update();
        void Clear();
        void Play();
        void Stop();
        void Pause();
        void WindLeft();
        void WindRight();
        void StopWinding();
        void ReverseMode(int rm);
        void RecordMode(int rec);
        void DirectionLeft();
        void DirectionRight();
        void Programm(int value, int m);
        void ShowDigit(int value);
        void ShowDigit9(int value);
        void ShowAll();
        void ShowNone();
        void ShowError();
        void ShowMinus(bool value);
        void BlinkPlay(bool value);

        bool WaitingForInput();
    private:

        void pulseClock();
        int segmentCount = 36;
        long display = 0;
        long digit = 0;
        bool blink = false;
        bool paused = false;
        bool FreePlace = false;
        bool programm = false;
        int programmPlace = 0;
        int programmFlashCount = 0;
        int blinkError = 0;
        int blinkDelay = 0;
        bool PlayMustBlink = false;
        bool minus = false;

        int init = 0;

        const int BLINK_DELAY = 3;

            //                                              7           0           c           d           5           5           5
        const int scroll[11] = { 0b00000000, 0b00000000, 0b00000111, 0b00111111, 0b01011000, 0b01011110, 0b01101101, 0b01101101, 0b01101101, 0b00000000, 0b00000000 };


        const int segmentCode[10] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
                             0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 };
        const long _FULL = 0xffffffff;  
        const int _LINE = 0b01000000;

        const long _D1_A =       0x00000001;  // Bit 1
        const long _D1_B =       0x00000002;  // Bit 2
        const long _D1_C =       0x00000004;  // Bit 3
        const long _D1_D =       0x00000008;  // Bit 4
        const long _D1_E =       0x00000010;  // Bit 5
        const long _D1_F =       0x00000020;  // Bit 6
        const long _D1_G =       0x00000040;  // Bit 7
        const long _ERROR =      0x00000080;  // Bit 8

        const long _D2_A =       0x00000100;  // Bit 9
        const long _D2_B =       0x00000200;  // Bit 10
        const long _D2_C =       0x00000400;  // Bit 11
        const long _D2_D =       0x00000800;  // Bit 12
        const long _D2_E =       0x00001000;  // Bit 13
        const long _D2_F =       0x00002000;  // Bit 14
        const long _D2_G =       0x00004000;  // Bit 15
        const long _ARROW =      0x00008000;  // Bit 16
        
        const long _REP_L =      0x00010000;  // Bit 17
        const long _REP_M =      0x00020000;  // Bit 18
        const long _REP_R =      0x00040000;  // Bit 19
        const long _REPEAT =     0x00080000;  // Bit 20
        const long _DIR_L =      0x00100000;  // Bit 21
        const long _WIND_RIGHT = 0x00200000;  // Bit 22
        const long _PLAY =       0x00400000;  // Bit 23
        const long _WIND_LEFT =  0x00800000;  // Bit 24

        const long _DIR_R =      0x01000000;  // Bit 25
        const long _SYNC =       0x02000000;  // Bit 26
        const long _AUTO =       0x04000000;  // Bit 27
        const long _REC =        0x08000000;  // Bit 28
        const long _PROGRAMM =   0x70000000;  // Bit 29,30,31
        const long _BLINK =      0x80000000;  // Bit 32
        const long _PRG_CLEAR =  0x90000000;  // Bit 29,32
        const long _PRG_ALL   =  0xf0000000;  // Bit 29,30,31,32
        


};


#endif
