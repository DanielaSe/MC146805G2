

#ifndef TTapeController_h
#define TTapeController_h


#include "TDisplay.h"
#include "TInputs.h"
#include "TPinLayout.h"



enum TReverseMode {rmNone, rmOneSide, rmBothSides, rmEndless};
enum TRecordMode {recNone, recOn, recSync, recAuto};
enum TFastWind {fwNone, fwForward, fwRewind};


class TTapeController
{

    public:

        TTapeController(TDisplay *_display, TInputs *_inputs);
        
        bool IsOnRecord();
        bool IsOnSyncRecord();
        bool IsOnAutoRecord();
        bool IsPlaying();
        bool IsRecording();
        bool IsPaused();
        int GetDirection();
        int GetCurrentState();        
        
        void Play();
        void Stop();
        void Pause();
        
        void WindLeft();
        void WindRight();

        void NextTrack();
        void PreviousTrack();

        void ToggleDirection();
        void ToggleReverseMode();
        void ToggleRecordMode();

        void Update();
        
        void NewTrack();
        void ProgrammKeyPressed();
        void MoveRecPlaybackLever();
        
        void ClearProgramm();
        void EndProgramm();
        
        void StartRecordMode();
        void Reset();

        void StateToString(); 
        void NewAutoRecordTrackStarted();
        void GetState();
        

        bool PlayProgramm = false;
        bool Programming = false;
        bool AutoRestart = false;
        bool SearchTrack = false;
        

    private:

        const int MAX_PROGRAMM_LENGTH = 40;
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

        TDisplay *lcd;
        TInputs *inputs;

        int CurrentState = 0;
        int TrackNumber = 1;

        int ProgrammPosition = -1;
        
        
        int SearchTrackNumber = 0;
        int Programm[41];
        int ProgrammCounter = 0;
        bool Recording;
        

        bool TrackFound = false;
       
        bool StartedWithEmptyTape = false;
        bool StopWhenTapeStarts = false;

        int AutoRecordTrackNumber = 0;
        
        bool StateSlideServoUp = false;
        bool StateRecord = false;
        bool StateHeadServo = false; // 1 wenn slide up und richtung rechts
        bool StateTapeReader = false;
        bool StatePause = false;
        bool StateCapstanMotor = false;
        bool RecordDiskEnd = false;
        bool RecordEnabledForSideA = true;
        bool RecordEnabledForSideB = true;
        bool PrepareForProgramm = false;
        
        // 1 second delay for capstan to switch off
        const int CAPSTAN_OFF_DELAY = 1000; 

        // Give the motor some time to reach its final speed before enabling audio
        const int SWITCHON_MUSIC_DELAY = 150; 

        long SwitchOffCapstan = 0;
        long SwitchOnMusic = 0;
        long IgnoreStateTapeReader = 0;
        long ms = 0;        
  
        int direction = -1;
        bool StartUp = true;
        bool playing = false;
        bool paused = false;
        
        bool StateReelMotor = false;


        bool RewindOneSide = false;
        bool RepeatSecondSide = false;
        TReverseMode ReverseMode = rmNone;
        TRecordMode RecordMode = recNone;
        TFastWind FastWinding = fwNone;
 
        
        void PushSlideServo();
        void SetMute(bool value);
        
        void StartProgramm();
        bool AddSearchTrackNumber(int value);
   
        int InRange(int value, int min, int max);
        int GetNextProgrammedTrack();
        void GetTrackForProgramm(int value);
        bool CheckIfRecordingIsPossible();
        
        void StartCapstan(); 
        void StopCapstan(); 

        void ToggleHead();



        bool HasProgramm();
        int ProgrammedTracks();
        bool AnyProgrammedTracksAfter(int x);


        

};


#endif
