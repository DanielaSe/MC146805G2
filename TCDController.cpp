




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
    pinMode(PIN_PC4, INPUT);
    digitalWrite(PIN_PA5, HIGH);
    digitalWrite(PIN_PA4, HIGH);
    digitalWrite(PIN_PA6, HIGH);

    /* DEBUG ??? */
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
        DoAutoStart = 0;
        Play();
    }

    if(now - ms > interval) {
        ms = now;
        // set all control pins to high
        digitalWrite(PIN_PA6, HIGH);
        digitalWrite(PIN_PA5, HIGH);
        digitalWrite(PIN_PA4, HIGH);
    }

    // read the input states and raise an event if they changed
    int r =  GetOutputPins() & 0x18;
    if (state != r) {
        state = r;
        if (state > 0) OnStateChangedEvent(state);
    }
}



/************************************************************************
 * Play
 * 
 * *********************************************************************/
void TCDController::Play()
{
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
    paused = true;    
    digitalWrite(PIN_PA5, LOW);
    ms = millis();
    if (AutoStart) {
        DoAutoStart = ms + 4000;
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
    playing = false;
    paused = false;
    digitalWrite(PIN_PA4, LOW);
    ms = millis();
}



/************************************************************************
 * GetOutputPins
 * get the current states of the CD-CPU
 * *********************************************************************/
int TCDController::GetOutputPins()
{
    int r = 0;
    if (digitalRead(PIN_PA5) == HIGH) { r += 0x01; }; 
    if (digitalRead(PIN_PA4) == HIGH) { r += 0x02; }; 
    if (digitalRead(PIN_PA6) == HIGH) { r += 0x04; };
    if (digitalRead(PIN_PA7) == HIGH) { r += 0x08; };
    if (digitalRead(PIN_PC4) == HIGH) { r += 0x10; };
    return r;
}
