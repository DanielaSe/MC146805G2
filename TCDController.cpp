




#include "TCDController.h"
#include "Arduino.h"
#include "TPinLayout.h"
#include "defines.h"




/************************************************************************
 * constructor
 * 
 * *********************************************************************/
TCDController::TCDController(void) {

    playing = false;
    paused = false;

    pinMode(PIN_PA5, OUTPUT); 
    pinMode(PIN_PA4, OUTPUT);
    pinMode(PIN_PA6, OUTPUT);
    pinMode(PIN_PA7, INPUT);
  //  pinMode(PIN_PC4, INPUT);
    digitalWrite(PIN_PA5, HIGH);
    digitalWrite(PIN_PA4, HIGH);
    digitalWrite(PIN_PA6, HIGH);

    pinMode(PIN_PA7, OUTPUT);
    digitalWrite(PIN_PA7, HIGH);


}




/************************************************************************
 * states/getter
 * 
 * *********************************************************************/
bool TCDController::IsPlaying() { return playing; }
bool TCDController::IsPaused() { return paused; }



/************************************************************************
 * Update
 * 
 * *********************************************************************/
void TCDController::Update()
{
    
    long now = millis();
    if (DoAutoStart > 0 && DoAutoStart < now) {
        #ifdef DEBUG
            Serial.println("+ DoAutoStart restart...");
        #endif
        DoAutoStart = 0;
        Play();
    }

    if(now - ms > BUTTON_INTERVAL) {
        ms = now;
        // set all control pins to high
        digitalWrite(PIN_PA6, HIGH);
        digitalWrite(PIN_PA5, HIGH);
        digitalWrite(PIN_PA4, HIGH);
    }


}



/************************************************************************
 * Play
 * 
 * *********************************************************************/
void TCDController::Play()
{
    #ifdef DEBUG
        Serial.println(":CD:Play()");
    #endif
    playing = true;
    paused = false;
    digitalWrite(PIN_PA6, LOW); 
    ms = millis();   
}



/************************************************************************
 * Pause
 * Use autostart for a 4 seconds pause - used for auto-rec mode
 * *********************************************************************/
void TCDController::Pause(bool AutoStart) 
{
    #ifdef DEBUG
        Serial.println(":CD:Pause()");
    #endif
    paused = true;    
    digitalWrite(PIN_PA5, LOW);
    if (AutoStart) {
        DoAutoStart = millis() + 4000;
        #ifdef DEBUG
            Serial.println("Auto Paused for 4 seconds");
        #endif
    }
}



/************************************************************************
 * Stop
 * 
 * *********************************************************************/
void TCDController::Stop()
{  
    #ifdef DEBUG
        Serial.println(":CD:Stop()");
    #endif
    playing = false;
    paused = false;
    digitalWrite(PIN_PA4, LOW);
    ms = millis();
}



