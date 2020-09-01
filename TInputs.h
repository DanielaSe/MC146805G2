

#ifndef TInputs_h
#define TInputs_h



class TInputs 
{

    

    public:

        TInputs(void);

        void read();
        int GetCassetteState();

    
        int pressedKeys = 0;
        int cassetteState = 0;


        const int CASS_REVERSE_MODE = 0x0001;
        const int CASS_REC_MODE = 0x0002;
        const int CASS_DIRECTION = 0x0004;

        const int CASS_PLAY = 0x0008;
        const int CASS_REWIND = 0x0010;
        const int CASS_FORWARD = 0x0020;

        const int CASS_STOP = 0x0040;
        const int CASS_PAUSE = 0x0080;
        const int CASS_REC = 0x0100;

        const int CASS_NEXT = 0x0200;
        const int CASS_PREV = 0x0400;
        const int CASS_PROG = 0x0800;

        const int CD_PLAY = 0x1000;
        const int CD_STOP = 0x2000;
        const int CD_PAUSE = 0x4000;

        const int IN_SLIDE_SERVO = 0x0001;
        const int IN_REC = 0x0002;
        const int IN_HEAD_SERVO = 0x0004;

        const int IN_PAUSE_CASS = 0x0008;
        const int IN_TAPE_READER = 0x0010;
        const int IN_REEL_MOTOR = 0x0020;

        const int IN_REC_DISC_END = 0x0040;
        const int IN_REC_SIDE_A = 0x080;
        const int IN_REC_SIDE_B = 0x0100;

        int GetPressedKey();

    private:
        int getKey(int shl);
        int readKeyLine(int pin, int shl);
        void readCassetteState();

        int oldPressedKeys = 0;
        int LockKeyBoard = 0;
        int LockKeyBoardTime = 100;

        long ms = 0;
        long interval = 30;

        int mhz = 0;


};


#endif
