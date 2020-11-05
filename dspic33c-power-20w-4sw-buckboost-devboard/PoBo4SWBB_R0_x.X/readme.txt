7.4.20
Start
--------------------------------------------------------------------------------
8.4.20:
DT reduce to 50ns       --> DONE
PWM off at beginning
naming change           --> DONE

Current sensing  --> filter C changed to 5.6nF --> DONE

Buck PWMH rise = Boost PWML rise  --> Start Of Cycle Trigger, PG2CONH.SOCS triggered by PG1 --> DONE

R146 not polulated  --> placed 10 Ohm DONE
--------------------------------------------------------------------------------
20.04.2020:
SW Code: changed AD-ch naming
         pinout was wrong
         changed open loop handling with poti
         measured timing
--------------------------------------------------------------------------------
22.04.2020
SW Code: ADC changed to Integer
         pwr_cntrl poti handling changed
         UART receive added
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
13.07.2020
check timing with DC write update instead of software-update UPDREQ/UPDATE
VMode 3p3z
use context saving register page FALTREG CTXTx
all faults add: Iin-OC + CMP
                Iout-OC
                Vaux OoR
Vin<->Iin feedforward
Check poles and zero settings based on real switching frequency
fixed Vout setting without poti connection
check if poti connected, pull-up, pull-down and measure ADC
