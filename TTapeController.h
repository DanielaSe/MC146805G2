

#ifndef TTapeController_h
#define TTapeController_h


#include "TDisplay.h"
#include "TInputs.h"
#include "TPinLayout.h"
#include "TCounter.h"



enum TReverseMode {rmNone, rmOneSide, rmBothSides, rmEndless};
enum TRecordMode {recNone, recOn, recSync, recAuto};
enum TFastWind {fwNone, fwForward, fwRewind};


class TTapeController
{

    public:

        TTapeController(TDisplay *_display, TInputs *_inputs, TCounter *_counter);
        
        bool IsOnRecord();
        bool IsOnSyncRecord();
        bool IsOnAutoRecord();
        bool IsPlaying();
        bool IsRecording();
        bool IsPaused();
        int GetDirection();
        int GetCurrentState();      

        TFastWind IsFastWinding();  
        
        void Play();
        void Stop();
        void Pause();
        void StopKeyPressed();
        
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
        void PauseAfterFourSeconds();
        bool ReadyForInput();

        bool GetTapeReader();

        
        bool PlayProgramm = false;
        bool Programming = false;
        bool AutoRestart = false;
        bool SearchTrack = false;
        
        int WhiteLeverReleaseTime = 0;

    private:


        const int MAX_PROGRAMM_LENGTH = 40;
        const int DELAY_SLIDE_SERVO = 80;
        const int WAIT_FOR_SERVO = 100;
        const int WAIT_BEFORE_FAST_WIND = 150;

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
        TCounter *counter;

        int CurrentState = 0;
        int TrackNumber = 1;

        int ProgrammPosition = -1;
        
        
        int SearchTrackNumber = 0;
        int Programm[41];
        int ProgrammCounter = 0;
        bool Recording;
        bool DesiredCapstanState = false;
        

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

        long GotoPosition = -1;
        int GotoPositionDir = 0;
        bool GotoPositionAutoPlay = false;
        
        // 5 seconds delay for capstan to switch off
        const int CAPSTAN_OFF_DELAY = 5000; 

        // Give the motor some time to reach its final speed before enabling audio
        const int SWITCHON_MUSIC_DELAY = 150; 

        long PauseTimer = 0;
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
        void MoveToPosition(bool autoplay, int pos);

        

};


#endif
