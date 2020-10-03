

#ifndef TInputs_h
#define TInputs_h



class TInputs 
{



    public:

        TInputs(void);

        void read();
        int GetCassetteState();
        bool IsDemoMode();

    
        int pressedKeys = 0;
        int cassetteState = 0;
        


        static constexpr int CASS_REVERSE_MODE = 0x0001;
        static constexpr int CASS_REC_MODE = 0x0002;
        static constexpr int CASS_DIRECTION = 0x0004;

        static constexpr int CASS_PLAY = 0x0008;
        static constexpr int CASS_REWIND = 0x0010;
        static constexpr int CASS_FORWARD = 0x0020;

        static constexpr int CASS_STOP = 0x0040;
        static constexpr int CASS_PAUSE = 0x0080;
        static constexpr int CASS_REC = 0x0100;

        static constexpr int CASS_NEXT = 0x0200;
        static constexpr int CASS_PREV = 0x0400;
        static constexpr int CASS_PROG = 0x0800;

        static constexpr int CD_PLAY = 0x1000;
        static constexpr int CD_STOP = 0x2000;
        static constexpr int CD_PAUSE = 0x4000;

        static constexpr int IN_SLIDE_SERVO = 0x0001;
        static constexpr int IN_REC = 0x0002;
        static constexpr int IN_HEAD_SERVO = 0x0004;

        static constexpr int IN_PAUSE_CASS = 0x0008;
        static constexpr int IN_TAPE_READER = 0x0010;
        static constexpr int IN_REEL_MOTOR = 0x0020;

        static constexpr int IN_REC_DISC_END = 0x0040;
        static constexpr int IN_REC_SIDE_A = 0x080;
        static constexpr int IN_REC_SIDE_B = 0x0100;

        int GetPressedKey();
        void ReadCassetteState();

    private:
        int getKey(int shl);
        int readKeyLine(int pin, int shl);
        

        int oldPressedKeys = 0;
        int LockKeyBoard = 0;
        int LockKeyBoardTime = 100;
        bool DemoMode = false;
        int DemoStep = 0;

        long ms = 0;
        long interval = 30;

        int mhz = 0;


};


#endif
