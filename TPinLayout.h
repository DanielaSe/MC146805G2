/****************************************************************************************************************
 * 
 * 
 * 
 *                                   MC146805G2
 *                              ________    ________
 *                             | ( )    |__|        |
 *                    !RESET --| 1               40 |-- VDD
 *      CD Pause State  !IRQ --| 2               39 |-- OSC IN
 *                       NUM --| 3               38 |-- OSC OUT
 *     Auto Pause ( ? )  PA7 --| 4               37 |-- TIMER  HI
 *        Play CD (OUT)  PA6 --| 5               36 |-- PD7  (OUT) Rec (-, Rec. Enable Side A, B)
 *       Pause CD (OUT)  PA5 --| 6               35 |-- PD6  (OUT) Cass Act ??? (Reel Motor, Tape Reader, Pause Cass)
 *        Stop CD (OUT)  PA4 --| 7               34 |-- PD5  (OUT) Cass Keys ??? (Head Servo, Rec, Slide Servo)
 *      LCD Clock (OUT)  PA3 --| 8               33 |-- PD4  (OUT) Read Keys 4 (Next, Previous, Prog)
 *       LCD Data (OUT)  PA2 --| 9               32 |-- PD3  (OUT) Read Keys 5 (CD Stop, CD Play, CD Pause)
 *        Wind << (OUT)  PA1 --| 10              31 |-- PD2  (OUT) Read Keys 3 (Stop, Pause, Rec)
 *        Wind >> (OUT)  PA0 --| 11              30 |-- PD1  (OUT) Read Keys 2 (Play, <<, >>)
 *  CD Mute State ( ? )  PB0 --| 12              29 |-- PD0  (OUT) Read Keys 1 (Reverse Mode, Rec Mode, Direction)
 *       Wind Dir (OUT)  PB1 --| 13              28 |-- PC0  
 * Relay FastWind (OUT)  PB2 --| 14              27 |-- PC1
 *        Capstan (OUT)  PB3 --| 15              26 |-- PC2
 *    Servo Relay (OUT)  PB4 --| 16              25 |-- PC3
 *                       PB5 --| 17              24 |-- PC4  ( ? ) Disc Space
 *                       PB6 --| 18              23 |-- PC5  (IN) Read Input 1
 *                       PB7 --| 19              22 |-- PC6  (IN) Read Input 2
 *                       VSS --| 20              21 |-- PC7  (IN) Read Input 3
 *                             |____________________|
 * 
 * 
 * 
 * Notes:
 * 
 *      Unused I/O pins are set to low
 * 
 *      Reset pin has a delay on start-up because of 2523 4.7uF -> rebuild width 10ms delay
 * 
 *      PB2, Relay Head/Dir switches the direction on the head only if cassette is down. The turining of the head
 *      is done by B63 on CASS.PART through the red lever of the mechanic. 
 *      
 *      SK60 Slide Servo Switch = head is up (1) or down (0)
 * 
 * **************************************************************************************************************/


#ifndef TPinLayout_h
#define TPinLayout_h



    // AT1284P Pinout
    const int _PA0 = 24;
    const int _PA1 = 25;
    const int _PA2 = 26;
    const int _PA3 = 27;
    const int _PA4 = 28;
    const int _PA5 = 29;
    const int _PA6 = 30;
    const int _PA7 = 31;  

    const int _PB0 = 0;
    const int _PB1 = 1;
    const int _PB2 = 2;
    const int _PB3 = 3;
    const int _PB4 = 4;
    const int _PB5 = 5;
    const int _PB6 = 6;
    const int _PB7 = 7;

    const int _PC0 = 16;
    const int _PC1 = 17;
    const int _PC2 = 18;
    const int _PC3 = 19;
    const int _PC4 = 20;
    const int _PC5 = 21;
    const int _PC6 = 22;
    const int _PC7 = 23;  

    const int _PD0 = 8;
    const int _PD1 = 9;
    const int _PD2 = 10;
    const int _PD3 = 11;
    const int _PD4 = 12;
    const int _PD5 = 13;
    const int _PD6 = 14;
    const int _PD7 = 15;


    const int PIN_BUILD_IN_LED = _PB0; 
  
    // PIN Assignment for Arduino

    const int PIN_DISPLAY_CLOCK = _PC6;    //
    const int PIN_DISPLAY_DATA = _PC5;     //

    const int PIN_IRQ = _PB1;    // 1 Play, 0 Pause                     (?)

    const int PIN_PA0 = _PD3;    // Forward                            (?)
    const int PIN_PA1 = _PD2;    // Rewind                             (?)
 //   const int PIN_PA2 = 13;    // Data                          
//    const int PIN_PA3 = 11;    // LCD Clock                          
    const int PIN_PA4 = _PC7;     // Stop CD 
    const int PIN_PA5 = _PB4;     // Pause CD
    const int PIN_PA6 = _PB3;     // Play CD
    const int PIN_PA7 = _PB2;     // PB2 Auto Pause

    const int PIN_PB0 = _PD4;    // Play/Rec                           (?)
    const int PIN_PB1 = _PD5;    // 1 Rewind, 0 Wind                   (?)
    const int PIN_PB2 = _PC4;    // Wind/Rewind slope                  (?)
    const int PIN_PB3 = _PD6;    // Capstan Motor 1 on, 0 off
    const int PIN_PB4 = _PC3;     // Cass. Key                          (?)
//    const int PIN_PB5 = 6;     // GND - unused
//    const int PIN_PB6 = 4;     // GND - unused
//    const int PIN_PB7 = 2;     // GND - unused

//    const int PIN_PC0 = ;     // GND - unused
//    const int PIN_PC1 = ;     // GND - unused
//    const int PIN_PC2 = ;     // GND - unused
//    const int PIN_PC3 = ;     // GND - unused

    const int PIN_PC4 = _PC2;    // Pause CD (w. Play on cass.)
    const int PIN_PC5 = _PC1;    // Read Keys 1 (Keys/cass. status)
    const int PIN_PC6 = _PC0;    // Read Keys 2 (Keys/cass. status)
    const int PIN_PC7 = _PD7;    // Read Keys 3 (Keys/cass. status)

    const int PIN_PD0 = _PA7;    // Key Group 1
    const int PIN_PD1 = _PA6;    // Key Group 2
    const int PIN_PD2 = _PA5;    // Key Group 3
    const int PIN_PD3 = _PA4;    // Key Group 5 CD
    const int PIN_PD4 = _PA3;    // Key Group 4
    const int PIN_PD5 = _PA2;     // Cass Key                           (?)
    const int PIN_PD6 = _PA1;     // Cass Act                           (?)
    const int PIN_PD7 = _PA0;     // Rec                                (?)


    
    


#endif
