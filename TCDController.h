

#ifndef TCDController_h
#define TCDController_h


class TCDController
{

    public:

        TCDController(void);


        void Update();

        void Play();
        void Pause();
        void Stop();

        bool Playing();
        bool Paused();

        int GetOutputPins();
    private:

        long ms = 0;
        long interval = 40;
        bool playing = false;
        bool paused = false;

        

        
};


#endif
