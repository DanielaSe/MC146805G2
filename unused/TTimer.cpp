
#include "TTimer.h"
#include "TimerOneThree.h"


TimerThree Timer3;



TTimer::TTimer()
{
    Timer3.initialize(1);
    Timer3.pwm(7, 350);
}