

#ifndef TTapeController_h
#define TTapeController_h


#include "TDisplay.h"
#include "TInputs.h"
#include "TPinLayout.h"



enum TReverseMode {rmNone, rmOneSide, rmBothSides, rmEndless};
enum TRecordMode {recNone, recOn, recSync, recAuto};


class TTapeController
{

    public:

        TTapeController(TDisplay *_display, TInputs *_inputs);
        
        
        
        void Play();
        void Stop();
        void Pause();
        
        void WindLeft();
        void WindRight();

        void NextTrack();
        void PreviousTrack();

        void ToggleDirection();
        void ToggleReverseMode();
        void ToggleHead();
        void ToggleRecordMode();
        void Update();
        
        void NewTrack();
        void ProgrammKeyPressed();
        void MoveRecPlaybackLever();
        
        bool IsOnRecord();
        bool IsOnSyncRecord();
        bool IsOnAutoRecord();
        bool Playing();
        bool Paused();
        int GetDirection();
        void ClearProgramm();
        void EndProgramm();
        bool HasProgramm();
        int ProgrammedTracks();
        bool AnyProgrammedTracksAfter(int x);
        void StartRecordMode();
        void Reset();
        void StateToString(); 

        int TrackNumber = 1;
        bool Programming = false;

        bool PlayProgramm = false;
        int ProgrammPosition = -1;
        
        bool SearchTrack = false;
        int SearchTrackNumber = 0;
        int Programm[21];
        int ProgrammCounter = 0;
        bool Recording;
        bool AutoRestart = false;

        bool TrackFound = false;
       
 


    private:
        TDisplay *lcd;
        TInputs *inputs;

        
  
        int direction = -1;
        bool StartUp = true;
        bool playing = false;
        bool paused = false;
        bool FastWinding = false;
        bool StateReelMotor = false;
        bool init = true;


        bool RewindOneSide = false;
        bool RepeatSecondSide = false;
        TReverseMode ReverseMode = rmNone;
        TRecordMode RecordMode = recNone;
        
 
        void GetState();
        void PushSlideServo();
        
        void StartProgramm();
        bool AddSearchTrackNumber(int value);
   
        int InRange(int value, int min, int max);
        int GetNextProgrammedTrack();
        

         bool StartedWithEmptyTape = false;


        int CurrentState = 0;
        bool StateSlideServoUp = false;
        bool StateRecord = false;
        bool StateHeadServo = false; // 1 wenn slide up und richtung rechts
        bool StateTapeReader = false;
        bool StatePause = false;
        bool StateCapstanMotor = false;
        bool RecordDiskEnd = false;
        bool RecordEnabledForSideA = true;
        bool RecordEnabledForSideB = true;
        
        

        const int DELAY_SLIDE_SERVO = 80;

        const int SET_CAPSTAN_MOTOR = PIN_PB3;
        const int SET_SLIDE_SERVO = PIN_PB4; 
        const int SET_WIND_LEFT = PIN_PA0;
        const int SET_WIND_RIGHT = PIN_PA1;
        const int SET_FAST_WIND = PIN_PB1;
        const int SET_FAST_WIND_RELAY = PIN_PB2;

        const int SET_MUTE = PIN_PB0;

        const int CD_INPUT_PAUSE = PIN_PA5;
        const int CD_INPUT_STOP = PIN_PA4;
        

};


#endif
