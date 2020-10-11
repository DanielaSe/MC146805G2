/****************************************************************************************************************
 * deprecated
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

    // PIN Assignment for Arduino

    const int PIN_DISPLAY_CLOCK = 11;    //
    const int PIN_DISPLAY_DATA = 13;     //


    const int PIN_IRQ = 19;    // 1 Play, 0 Pause                     (?)

    const int PIN_PA0 = 18;    // Forward                            (?)
    const int PIN_PA1 = 15;    // Rewind                             (?)
 //   const int PIN_PA2 = 13;    // LCD Data                          
//    const int PIN_PA3 = 11;    // LCD Clock                          
    const int PIN_PA4 = 9;     // Stop CD 
    const int PIN_PA5 = 7;     // Pause CD
    const int PIN_PA6 = 5;     // Play CD
    const int PIN_PA7 = 3;     // Auto Pause

    const int PIN_PB0 = 16;    // Play/Rec                           (?)
    const int PIN_PB1 = 14;    // 1 Rewind, 0 Wind                   (?)
    const int PIN_PB2 = 12;    // Wind/Rewind slope                  (?)
    const int PIN_PB3 = 10;    // Capstan Motor 1 on, 0 off
    const int PIN_PB4 = 8;     // Cass. Key                          (?)
//    const int PIN_PB5 = 6;     // GND - unused
//    const int PIN_PB6 = 4;     // GND - unused
//    const int PIN_PB7 = 2;     // GND - unused

//    const int PIN_PC0 = ;     // GND - unused
//    const int PIN_PC1 = ;     // GND - unused
//    const int PIN_PC2 = ;     // GND - unused
//    const int PIN_PC3 = ;     // GND - unused
    const int PIN_PC4 = 35;    // Pause CD (w. Play on cass.)
    const int PIN_PC5 = 33;    // Read Keys 1 (Keys/cass. status)
    const int PIN_PC6 = 34;    // Read Keys 2 (Keys/cass. status)
    const int PIN_PC7 = 32;    // Read Keys 3 (Keys/cass. status)

    const int PIN_PD0 = 45;    // Key Group 1
    const int PIN_PD1 = 47;    // Key Group 2
    const int PIN_PD2 = 46;    // Key Group 3
    const int PIN_PD3 = 44;    // Key Group 5 CD
    const int PIN_PD4 = 42;    // Key Group 4
    const int PIN_PD5 = 40;     // Cass Key                           (?)
    const int PIN_PD6 = 38;     // Cass Act                           (?)
    const int PIN_PD7 = 36;     // Rec                                (?)


    
    


#endif
