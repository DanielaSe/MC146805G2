

#ifndef TCDController_h
#define TCDController_h
#include "TPinLayout.h"


class TCDController
{

    public:

        TCDController(void);


        void Update();

        void Play();
        void Pause(bool AutoStart);
        void Stop();

        bool IsPlaying();
        bool IsPaused();
        void OnStateChanged(void (*_OnStateChangedEvent)(const int)) { OnStateChangedEvent = _OnStateChangedEvent; }

        
    private:

        const int STATE_PAUSE_CD = PIN_PA5;
        const int STATE_STOP_CD = PIN_PA4;
        const int STATE_PLAY_CD = PIN_PA6;
        const int STATE_AUTO_PAUSE = PIN_PA7;
        const int STATE_CD_PAUSE = PIN_PC4;

        void (*OnStateChangedEvent)(const int);

        long ms = 0;
        long interval = 40;
        long DoAutoStart = 0;
        bool playing = false;
        bool paused = false;
        int state = 0;

        int GetOutputPins();        
};


#endif
