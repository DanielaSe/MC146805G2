


#ifndef TCounter_h
#define TCounter_h


#include "Arduino.h"
#include <Adafruit_SSD1306.h>


enum TTapeLength {tlC30, tlC60, tlC90};
enum TScreenMode {smDefault, smSplashScreen, smConfigPosition, smDateTime, smDateTimeConfig, smScreenSaver, smWhiteLever};

class TCounter
{

    public:
        TCounter();
       
        void Reset();
        void Play();
        void Stop();
        void FastWind(int winddir);
        void Update();
        void Init();
        void SetDirection(int direction);
        bool Overflow();

        int GetCurrentPosition();
        int GetMemoryPosition(int pos);
        bool IsAtPosition(int pos);
        void WakeUp();

        void ToggleTapeLength();

        void ConfigScreenPosition(bool active, int deltaX, int deltaY);
        void SetWhiteLeverReleaseTime(bool active, int deltaX);

        void StoreReverseMode(int value);
        int RestoreReverseMode();

        int GetWhiteLeverReleaseTime();

    private:
        void StorePosition(unsigned long s);
        void RestorePosition();
        byte InRange(byte value);

        int WhiteLeverReleaseTime = 0;
        
        enum TCountState {csStopped, csPlaying, csFastWinding};
        
        TScreenMode ScreenMode = smDefault;

        byte SCREEN_OFFSET_X = 12;
        byte SCREEN_OFFSET_Y = 39;
        const long SCREEN_SAVER_DELAY = 120000L; // 2 minutes

        float CassetteLengthInSeconds = 2700.0;
        
        const float FF_MULTIPLIER = 8.5;
        const long MAX_COUNTER_VALUE = 3599999L;
        const long BTN_RESET_TIME = 500;
        const int MAX_MEMORY = 3;
        int count = 1;
        bool btnPressed = false;
        bool btnPreviousFramePressed = false;
        long btnPressTime = 0;
        long ShowTapeLength = 0;
        long SplashScreen = 0;

        long ScreenSaver = 0;
        bool overflow = false;
        int WindDirection = 0;
        int Direction = 1;
        int Seconds;
        bool Modified = false;
        TCountState State = csStopped;
        TTapeLength TapeLength = tlC90;

        unsigned long ms = 0;
        long Position;
        String time;

        int Memory[3];
        int MemoryPosition = 0;


};


#endif