
#include "defines.h"
#include "TInputs.h"
#include "Arduino.h"
#include "TPinLayout.h"






TInputs::TInputs(void) {

   DemoMode = 0;
   pinMode(PIN_PD0, OUTPUT); 
   pinMode(PIN_PD1, OUTPUT);
   pinMode(PIN_PD2, OUTPUT);
   pinMode(PIN_PD3, OUTPUT);
   pinMode(PIN_PD4, OUTPUT);
   pinMode(PIN_PD5, OUTPUT);
   pinMode(PIN_PD6, OUTPUT);
   pinMode(PIN_PD7, OUTPUT);

   pinMode(PIN_PC5, INPUT);
   pinMode(PIN_PC6, INPUT);
   pinMode(PIN_PC7, INPUT);

   digitalWrite(PIN_PD0, LOW);
   digitalWrite(PIN_PD1, LOW);
   digitalWrite(PIN_PD2, LOW);
   digitalWrite(PIN_PD3, LOW);
   digitalWrite(PIN_PD4, LOW);
   digitalWrite(PIN_PD5, LOW);
   digitalWrite(PIN_PD6, LOW);
   digitalWrite(PIN_PD7, LOW);

   

}



void TInputs::read() 
{



   long now = millis();
   if (LockKeyBoard > now) {
      return;
   }
   pressedKeys = 0; 



   // ensure the pins are down!
   digitalWrite(PIN_PD0, LOW);
   digitalWrite(PIN_PD1, LOW);
   digitalWrite(PIN_PD2, LOW);
   digitalWrite(PIN_PD3, LOW);
   digitalWrite(PIN_PD4, LOW);
   digitalWrite(PIN_PD5, LOW);
   digitalWrite(PIN_PD6, LOW);
   digitalWrite(PIN_PD7, LOW);

   int x = 0;

   x |= readKeyLine(PIN_PD0,0);
   x |= readKeyLine(PIN_PD1,3);
   x |= readKeyLine(PIN_PD2,6);
   x |= readKeyLine(PIN_PD4,9);
   x |= readKeyLine(PIN_PD3,12);

   if (x != oldPressedKeys) {
      pressedKeys = x;
      oldPressedKeys = x;
      LockKeyBoard = now + LockKeyBoardTime;
   }
   else {
      // we always need fast forward and rewind 
      pressedKeys |= x & CASS_REWIND;
      pressedKeys |= x & CASS_FORWARD;
   }

  if (x == 56) {// (CASS_REWIND || CASS_FORWARD || CASS_PLAY) 
      DemoMode = 1;
      #ifdef DEBUG
         Serial.println("> Entering demo mode 1");
      #endif
  }


  if (DemoMode > 0 && x == CASS_PLAY) {
     pressedKeys = 0;
     DemoMode += 1;
           #ifdef DEBUG
         Serial.print("> Entering demo mode ");
         Serial.println(DemoMode);
      #endif
     if (DemoMode >= 4) { 
         #ifdef DEBUG
            Serial.println("> Entering demo mode 1");
         #endif
         DemoMode = 1; 
        }
  }
  if (DemoMode > 0 && x == CASS_STOP) {
         #ifdef DEBUG
            Serial.println("> Disable demo mode");
         #endif
         DemoMode = 4; 
  }



}

void TInputs::DisableDemoMode() {
   DemoMode = 0;
}

int TInputs::GetDemoMode() {

   return DemoMode;
}

int TInputs::GetCassetteState()
{
   ReadCassetteState();
   return cassetteState;
}


void TInputs::ReadCassetteState()
{
   // ensure the pins are down!
   digitalWrite(PIN_PD0, LOW);
   digitalWrite(PIN_PD1, LOW);
   digitalWrite(PIN_PD2, LOW);
   digitalWrite(PIN_PD3, LOW);
   digitalWrite(PIN_PD4, LOW);
   digitalWrite(PIN_PD5, LOW);
   digitalWrite(PIN_PD6, LOW);
   digitalWrite(PIN_PD7, LOW); // Record only

   int x = 0;
   x |= readKeyLine(PIN_PD5, 0);
   x |= readKeyLine(PIN_PD6, 3);
   x |= readKeyLine(PIN_PD7, 6);

   if (x != cassetteState) cassetteState = x;

}




int TInputs::readKeyLine(int pin, int shl) 
{
   digitalWrite(pin, HIGH);
   int r = getKey(shl); 
   digitalWrite(pin, LOW);
   delay(5); // Affects the next readkeyline and results into flickering. Why? No Idea!
   return r;
}


int TInputs::getKey(int shl) 
{
   int x = 0;
 
   if (digitalRead(PIN_PC5) == HIGH) { x |= 0x01; }
   if (digitalRead(PIN_PC6) == HIGH) { x |= 0x02; }
   if (digitalRead(PIN_PC7) == HIGH) { x |= 0x04; }
   return x << shl;

}

int TInputs::ReadKeySet1()
{
   if (pressedKeys == CASS_NEXT) return 1;
   if (pressedKeys == CASS_PREV) return -1;
   return 0;
}
int TInputs::ReadKeySet2()
{
   if (pressedKeys == CASS_FORWARD) return 1;
   if (pressedKeys == CASS_REWIND) return -1;   
   return 0;
}


int TInputs::GetPressedKey()
{
   return oldPressedKeys;
}
