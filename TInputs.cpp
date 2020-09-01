

#include "TInputs.h"
#include "Arduino.h"
#include "TPinLayout.h"






TInputs::TInputs(void) {

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

   pressedKeys = 0; 
   long now = millis();
   if (LockKeyBoard > now) {
      return;
   }
/*
   // Skip one cycle to simulate 8MHz (Arduino = 16MHz)
   mhz += 1;
   if (mhz < 2) {
      return;
   }
   mhz = 0;
*/

   

   // ensure the pins are down!
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


   


   

  // readCassetteState();



}

int TInputs::GetCassetteState()
{
   readCassetteState();
   return cassetteState;
}


void TInputs::readCassetteState()
{
   // ensure the pins are down!
   digitalWrite(PIN_PD0, LOW);
   digitalWrite(PIN_PD1, LOW);
   digitalWrite(PIN_PD2, LOW);
   digitalWrite(PIN_PD3, LOW);
   digitalWrite(PIN_PD4, LOW);

   int x = 0;
   x |= readKeyLine(PIN_PD5, 0);

   x |= readKeyLine(PIN_PD6, 3);

   x |= readKeyLine(PIN_PD7, 6);

   if (x != cassetteState) {

      cassetteState = x;
   }
}


int TInputs::readKeyLine(int pin, int shl) 
{
   digitalWrite(pin, HIGH);
   int r = getKey(shl); 
   digitalWrite(pin, LOW);
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


int TInputs::GetPressedKey()
{
   return oldPressedKeys;
}
