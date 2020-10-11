

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



    private:



        void (*OnStateChangedEvent)(const int);
        long ms = 0;
        const int BUTTON_INTERVAL = 80;
        long DoAutoStart = 0;
        bool playing = false;
        bool paused = false;
       
};


#endif
