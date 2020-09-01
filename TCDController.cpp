




#include "TCDController.h"
#include "Arduino.h"
#include "TPinLayout.h"





TCDController::TCDController(void) {

    playing = false;
    paused = false;

    pinMode(PIN_PA5, OUTPUT); 
    pinMode(PIN_PA4, OUTPUT);
    pinMode(PIN_PA6, OUTPUT);
    pinMode(PIN_PA7, INPUT);
    pinMode(PIN_PC4, INPUT);

    /* DEBUG */
    pinMode(PIN_PA7, OUTPUT);        
    digitalWrite(PIN_PA7, HIGH);


    digitalWrite(PIN_PA5, HIGH);
    digitalWrite(PIN_PA4, HIGH);
    digitalWrite(PIN_PA6, HIGH);
}

void TCDController::Update()
{
   
    long now = millis();
    if(now - ms > interval) {
        ms = now;
        // set all control pins to high
        digitalWrite(PIN_PA6, HIGH);
        digitalWrite(PIN_PA5, HIGH);
        digitalWrite(PIN_PA4, HIGH);
    }

}

bool TCDController::Playing()
{
    return playing;
}

bool TCDController::Paused()
{
    return paused;
}


void TCDController::Play()
{
 //   if (playing && !paused)
 //   {
 //       return;
 //   }
    playing = true;
    paused = false;
    digitalWrite(PIN_PA6, LOW); 
    ms = millis();   
}


void TCDController::Pause() 
{
 //   if (paused || !playing) 
 //   {
 //       return;
  //  }
    paused = true;    
    digitalWrite(PIN_PA5, LOW);
    ms = millis();
}


void TCDController::Stop()
{  
    playing = false;
    paused = false;
    digitalWrite(PIN_PA4, LOW);
    ms = millis();
}



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
