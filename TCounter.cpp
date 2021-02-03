

#include "TCounter.h"
#include "Arduino.h"
#include "defines.h"
#include "TPinLayout.h"
#include <SPI.h>
#include <Wire.h>
#include "images.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <MD_DS1307.h>



#define  MAX_READ_BUF  (DS1307_RAM_MAX / 8)

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
MD_DS1307 rtc;

union BufferLong
{
    unsigned long value;
    uint8_t bytes[4];
};

union BufferByte
{
    byte value;
    uint8_t bytes[1];
};




/************************************************************************
 * Constructor
 * 
 * *********************************************************************/
TCounter::TCounter()
{

    Reset();
}

byte TCounter::InRange(byte value)
{
    if (value > 50) return 50;
    return value;

}




/************************************************************************
 * RestorePosition
 * 
 * *********************************************************************/
void TCounter::RestorePosition()
{
   
    #ifdef DEBUG
        Serial.println(":RestorePosition");
    #endif   
    #ifdef COUNTER
        Position = 0;
        BufferLong buffer;
        buffer.value = 0;
        rtc.readRAM(8, buffer.bytes, 4);
        Position = buffer.value;
        Modified = true;
    #endif
}




/************************************************************************
 * StorePosition
 * 
 * *********************************************************************/
void TCounter::StorePosition(unsigned long s)
{
    #ifdef DEBUG
        Serial.println(":StorePosition");
    #endif
    #ifdef COUNTER
        BufferLong buffer;
        buffer.value = s; 
        rtc.writeRAM(8, buffer.bytes, 4);
    #endif
}


/************************************************************************
 * StoreReverseMode
 * 
 * *********************************************************************/
void TCounter::StoreReverseMode(int value)
{
    #ifdef DEBUG
        Serial.println(":StoreReverseMode");
    #endif
    #ifdef COUNTER
        BufferByte buffer;
        buffer.value = value & 0xff; 
        rtc.writeRAM(15, buffer.bytes, 1);
    #endif
}

/************************************************************************
 * RestoreReverseMode
 * 
 * *********************************************************************/
int TCounter::RestoreReverseMode()
{
    #ifdef DEBUG
        Serial.println(":RestoreReverseMode");
    #endif
    #ifdef COUNTER
        BufferByte a;
        rtc.readRAM(15, a.bytes, 1);
        return a.value;
    #else  
      return 0;
    #endif
}


int TCounter::GetWhiteLeverReleaseTime()
{
    return WhiteLeverReleaseTime;
}

/************************************************************************
 * SetWhiteLeverReleaseTime
 * 
 * 
 * *********************************************************************/
void TCounter::SetWhiteLeverReleaseTime(bool active, int deltaX)
{
    if (!active) {
        ScreenMode = smDefault;
        return;
    }
    ScreenMode = smWhiteLever;
    if (deltaX == 0) return;
    if (WhiteLeverReleaseTime + deltaX >= 0 && WhiteLeverReleaseTime + deltaX < 100) { WhiteLeverReleaseTime += deltaX; }

    Serial.print("WhiteLeverReleaseTime: ");
    Serial.println(WhiteLeverReleaseTime);

    BufferByte a;
    a.value = WhiteLeverReleaseTime;
    rtc.writeRAM(16, a.bytes, 1);
}


/************************************************************************
 * ConfigScreenPosition
 * 
 * 
 * *********************************************************************/
void TCounter::ConfigScreenPosition(bool active, int deltaX, int deltaY)
{
    if (!active) {
        ScreenMode = smDefault;
        return;
    }
    ScreenMode = smConfigPosition;
    if (deltaX == 0 && deltaY == 0) return;
    if (SCREEN_OFFSET_X + deltaX >= 0 && SCREEN_OFFSET_X + deltaX < 100) { SCREEN_OFFSET_X += deltaX; }
    if (SCREEN_OFFSET_Y + deltaY >= 0 && SCREEN_OFFSET_Y + deltaY < 100) { SCREEN_OFFSET_Y += deltaY; }
    BufferByte a;
    a.value = SCREEN_OFFSET_X;
    rtc.writeRAM(13, a.bytes, 1);
    a.value = SCREEN_OFFSET_Y;
    rtc.writeRAM(14, a.bytes, 1);
}



/************************************************************************
 * Init
 * 
 * *********************************************************************/
void TCounter::Init()
{
    #ifdef COUNTER
        oled.setRotation(1);
        if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
            #ifdef DEBUG
                Serial.println(F("OLED allocation failed"));
            #endif
        }
        SplashScreen = millis() + 2000;
        ScreenMode = smSplashScreen;
        
        if (!rtc.isRunning()) rtc.control(DS1307_CLOCK_HALT, DS1307_OFF);
        #ifdef DEBUG
            Serial.println("read screen coordinates");
        #endif
        BufferByte a;
        rtc.readRAM(13, a.bytes, 1);
        SCREEN_OFFSET_X = InRange(a.value);
        rtc.readRAM(14, a.bytes, 1);
        SCREEN_OFFSET_Y = InRange(a.value);
        rtc.readRAM(16, a.bytes, 1);
        WhiteLeverReleaseTime = InRange(a.value);

        #ifdef DEBUG
            Serial.print("X:");
            Serial.print(SCREEN_OFFSET_X);
            Serial.print(" Y:");
            Serial.println(SCREEN_OFFSET_Y);
        #endif

        oled.clearDisplay();
        oled.drawBitmap(SCREEN_OFFSET_X - 2, SCREEN_OFFSET_Y + 10, ImageLogo, 34, 44, 1);
        oled.display();
        Modified = true;
        #ifdef DEBUG
            Serial.println("+OLED initialised");
        #endif   

        
        RestorePosition();
    #endif
}


/************************************************************************
 * Play
 * Start counter
 * *********************************************************************/
void TCounter::Play()
{
    #ifdef DEBUG
        Serial.println(":Counter.Play");
    #endif
    State = csPlaying;
}



/************************************************************************
 * Reset
 * Reset to default values
 * *********************************************************************/
void TCounter::Reset()
{
    #ifdef DEBUG
        Serial.println(":Counter.Reset");
    #endif
    ScreenMode = smDefault;
    Position = 0;
    MemoryPosition = 0;
    Modified = true;
    for (int i = 0; i < MAX_MEMORY; i++) Memory[i] = -1;
}



/************************************************************************
 * Stop
 * Stop counter
 * *********************************************************************/
void TCounter::Stop()
{
    #ifdef DEBUG
        Serial.println(":Counter.Stop");
    #endif
    State = csStopped;
    StorePosition(Position);
    Modified = true;
}



/************************************************************************
 * ToggleTapeLength
 * 
 * *********************************************************************/
void TCounter::ToggleTapeLength()
{
    // Set the duration of a cassette (one side)
    switch(TapeLength)
    {
        case tlC30: TapeLength = tlC60; CassetteLengthInSeconds = 1800.0; break;
        case tlC60: TapeLength = tlC90; CassetteLengthInSeconds = 2700.0; break;
        case tlC90: TapeLength = tlC30; CassetteLengthInSeconds = 900.0; break;
    }
    ShowTapeLength = millis() + 2000;
}



/************************************************************************
 * FastWind
 * Fast forward/rewind active
 * > 0 forward, < 0 rewind
 * *********************************************************************/
void TCounter::FastWind(int winddir)
{

    WindDirection = winddir;
    State = csFastWinding;
    Modified = true;
    #ifdef DEBUG
        Serial.print(" WindDir: ");
        Serial.print(winddir);
        Serial.print(" WindDirection: ");
        Serial.print(WindDirection);
        Serial.println(" :Counter.FastWind"); 
    #endif
}



/************************************************************************
 * GetCurrentPosition
 * 
 * *********************************************************************/
int TCounter::GetCurrentPosition()
{
    return Seconds;
}



/************************************************************************
 * GetMemoryPosition
 * 
 * *********************************************************************/
int TCounter::GetMemoryPosition(int pos)
{
    pos--;
    if (pos >= 0 && pos <= 3) return Memory[pos];
    return -1;
}



/************************************************************************
 * Overflow
 * 
 * *********************************************************************/
bool TCounter::Overflow()
{
    bool a = overflow;
    overflow = false;
    return a;

}



/************************************************************************
 * IsAtPosition
 * 
 * *********************************************************************/
bool TCounter::IsAtPosition(int pos)
{
    bool found = false;
    if (pos == 0) {

        if (WindDirection < 0) found = Seconds <= 1;
        else found = Seconds >= -1;
        if (Overflow()) found = true;

    } 
    else
    {
        if (WindDirection < 0) found = Seconds < pos + 1;
        else found = (Seconds > pos - 1);
    }
    #ifdef DEBUG
        if (found) Serial.println("+Position found");
    #endif
    return found;

}



/************************************************************************
 * SetDirection
 * 
 * *********************************************************************/
void TCounter::SetDirection(int direction)
{
    if (Direction != direction) {
        Direction = direction;
        Modified = true;
    }
}



/************************************************************************
 * WakeUp
 * 
 * *********************************************************************/
void TCounter::WakeUp()
{
    ms = millis();
}



/************************************************************************
 * Update
 * 
 * *********************************************************************/
void TCounter::Update()
{
    #ifdef COUNTER
    long lastms = ms;
    
    ms = millis();

    

    switch (ScreenMode) {

        case smSplashScreen: 
            if (SplashScreen > ms) return;
            ScreenMode = smDefault;
            break;

        case smConfigPosition:
            oled.clearDisplay();
            oled.drawRect(SCREEN_OFFSET_X - 1, SCREEN_OFFSET_Y - 1, 33, 86, 1);
            oled.display();  
            return;
        
        case smScreenSaver:
            oled.clearDisplay();
            oled.display();  
            break;

        case smWhiteLever:
            oled.clearDisplay();
            oled.setFont();
            oled.setTextSize(1); 
            oled.setTextColor(SSD1306_WHITE);
            
            oled.setCursor(SCREEN_OFFSET_X + 2, SCREEN_OFFSET_Y + 0);
            oled.println(F("Rel.Time"));
            oled.setCursor(SCREEN_OFFSET_X + 6, SCREEN_OFFSET_Y + 20);
            char s[5];
            sprintf(s, "%d", WhiteLeverReleaseTime * 10);
            oled.println(s);
            oled.display();  
            return;
        
    }

    if (lastms == 0) { lastms = ms; }
    long elapsed = ms - lastms;
    int PreviousSeconds = Seconds;


    switch (State) 
    {
        case csPlaying:
            Position += elapsed;
            Modified = true;
            break;

        case csFastWinding:
            // length of 90min tape = 128.250mm
            // get current position (msec) in mm
            float TapePosMM = (Position / 1000.0) * 47.6;

            // beginning of the tape = 71mm
            // end of tape 150mm
            // millimeter per full rotation
            float mmpfr = 71.0 + (79.0 * (Seconds / CassetteLengthInSeconds));

            if (WindDirection < 0) mmpfr = 150 - (79.0 * (Seconds / CassetteLengthInSeconds));

            // mm in elapsed time
            float mmet = FF_MULTIPLIER * (elapsed / 1000.0);
            
            if (WindDirection > 0) TapePosMM += mmpfr * mmet;
            else TapePosMM -= mmpfr * mmet;

            // new position
            Position = (TapePosMM / 47.6) * 1000.0;    
            Modified = true;      
            break;
    }
    overflow = false;
    
    // no negative numbers. we simply have no place for a minus
    if (Position < 0) {
        Position += MAX_COUNTER_VALUE;
        overflow = true;
        #ifdef DEBUG
            Serial.println("+Counter.Overflow detected < 0");
        #endif
    }
    if (Position > MAX_COUNTER_VALUE) {

        #ifdef DEBUG
            Serial.print("+Counter.Overflow detected ");
            Serial.print(Position);
            Serial.print(" > ");
            Serial.println(MAX_COUNTER_VALUE);
        #endif
        while (Position > MAX_COUNTER_VALUE) Position -= MAX_COUNTER_VALUE;
        overflow = true;
        Modified = true;    
    }

    Seconds = abs(round(Position / 1000));

    // Counter Reset/Memory button
    bool b = digitalRead(PIN_BTN_COUNTER) == LOW;

    #ifdef DEBUG
        if (b) Serial.println("+Counter Button Pressed");
    #endif;

    if (btnPressed && !b && btnPressTime > 0) {

        Memory[MemoryPosition] = Seconds;
        MemoryPosition++;
        if (MemoryPosition >= MAX_MEMORY) MemoryPosition = 0;
        Modified = true;
        #ifdef DEBUG
            Serial.print("+Add to Memory Position: ");
            Serial.print(MemoryPosition);
            Serial.print(" Time: ");
            Serial.println(Seconds);
        #endif;
    }

    if (!btnPressed && b) btnPressTime = ms;

    if (b) {
        if (ms < ShowTapeLength) {
            if (!btnPreviousFramePressed) ToggleTapeLength();
        }
        else {
            btnPressed = true;
            if (btnPressTime > 0 && ms > btnPressTime + BTN_RESET_TIME) {
                Reset();
                ShowTapeLength = ms + 2000;
                btnPressTime = 0;
                Modified = true;
            }
        }         

    } 
    else btnPressed = false;

    btnPreviousFramePressed = b;

    if (PreviousSeconds != Seconds) Modified = true;


    if (ms < ShowTapeLength) {
        oled.clearDisplay();
        oled.drawBitmap(SCREEN_OFFSET_X, SCREEN_OFFSET_Y + 2, ImageCassette, 32, 21, 1);
        oled.setFont();
        oled.setFont(&FreeMonoBold12pt7b);
        oled.setTextSize(1);
        oled.setTextColor(SSD1306_WHITE);
        oled.setCursor(SCREEN_OFFSET_X + 10, SCREEN_OFFSET_Y + 40);
        oled.println(F("C"));
        oled.setCursor(SCREEN_OFFSET_X + 3, SCREEN_OFFSET_Y + 60);
        switch(TapeLength)
        {
            case tlC30: oled.println(F("30")); break;   
            case tlC60: oled.println(F("60")); break;   
            case tlC90: oled.println(F("90")); break;   
        }     
        oled.display();   
        return;
    }




    // Check if ScreenSaver should be activated
    if (!Modified) {
        if (ScreenMode != smScreenSaver) {
            ScreenSaver -= (ms - lastms);
            if (ScreenSaver < 0) {
                #ifdef DEBUG
                    Serial.println("+Screensaver active");
                #endif
                ScreenMode = smScreenSaver;  
            }
        }
        return;
    }


    // update display
    

    if (ScreenMode == smScreenSaver) ScreenMode = smDefault;

    Modified = false;    
    ScreenSaver = SCREEN_SAVER_DELAY;
    int min = trunc(Seconds / 60);
    int sec = Seconds - (min * 60);

    oled.clearDisplay();

    // Visible area - Height: 90px, Width:35px    

    // Small Fonts
    oled.setFont();
    oled.setTextSize(1); 
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(SCREEN_OFFSET_X + 4, SCREEN_OFFSET_Y + 0);
    oled.println(F("Side"));

    // Large Fonts
    oled.setFont(&FreeMonoBold12pt7b);
    oled.setTextSize(1);

    oled.setCursor(SCREEN_OFFSET_X + 9, SCREEN_OFFSET_Y + 25);
    if (Direction > 0) oled.println(F("A"));
    else  oled.println(F("B"));


    char s[2];
    sprintf(s, "%02d", min);
    oled.setCursor(SCREEN_OFFSET_X + 1, SCREEN_OFFSET_Y + 52);
    oled.println(s);

    sprintf(s, "%02d", sec);
    oled.setCursor(SCREEN_OFFSET_X + 1, SCREEN_OFFSET_Y + 70);
    oled.println(s);

    // Memory Positions
    if (Memory[0] < 0) oled.drawCircle(SCREEN_OFFSET_X + 4, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);
    else oled.fillCircle(SCREEN_OFFSET_X + 4, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);

    if (Memory[1] < 0) oled.drawCircle(SCREEN_OFFSET_X + 14, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);
    else oled.fillCircle(SCREEN_OFFSET_X + 14, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);

    if (Memory[2] < 0) oled.drawCircle(SCREEN_OFFSET_X + 24, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);
    else oled.fillCircle(SCREEN_OFFSET_X + 24, SCREEN_OFFSET_Y + 80, 2, SSD1306_WHITE);

    oled.display();  


    #endif
}


