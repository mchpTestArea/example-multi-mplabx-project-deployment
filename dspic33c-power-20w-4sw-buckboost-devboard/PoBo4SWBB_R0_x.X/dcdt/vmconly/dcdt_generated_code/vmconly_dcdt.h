/**
  Summary:
  This header file was created using Microchip Digital Compensator Design Tool (DCDT)
  and includes system summary and digital compensator coefficients. This file
  should be included in the MPLAB X project directory.

  File Name:     vmconly_dcdt.h
  Project Name:  vmconly
  Date:          7/16/2020
  Time:          13:23.51

  Software License Agreement

  Copyright © 2020 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller, which is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).

  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.

  SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
 **/
#ifndef _VMCONLY_DCDT_DEFINES_H
#define _VMCONLY_DCDT_DEFINES_H

/**
  Compensator Type:  3P3Z
      Entry                Value  
    ---------            ---------
  Pole 0                 5.0000e+03 Hz
  Pole 2                 3.5000e+04 Hz
  Pole 3                 1.7500e+05 Hz
  Zero 1                 8.0000e+02 Hz
  Zero 2                 4.5000e+03 Hz
  Gain(Kdc)              1.000
  Warp                   false
  PWM Frequency          3.5000e+05
  PWM Sampling Ratio     10
  Sampling Frequency     3.5000e+04
  PWM Max Resolution     2.5000e-10
  Computational Delay    1.5000e-06
  Gate Drive Delay       1.5000e-07
  Control Output Min.    0
  Control Output Max.    32767
  Kuc Gain               9.2098e+00
  Use Kuc Gain           false


  PWM Calculations
      Name                Value  
    ---------           ---------
  Bits of Resolution    16.802
  Gain                  8.750e-06


  s-domain transfer function

               Wp0   Wp2(Wp3)(Wz1 + s)(Wz2 + s)
  H(s) = Kdc X --- X --------------------------
                s    Wz1(Wz2)(Wp2 + s)(Wp3 + s)

                 3.14e+04   2.20e+05(1.10e+06)(5.03e+03 + s)(2.83e+04 + s)
  H(s) = 1.000 X -------- X ----------------------------------------------
                    s       5.03e+03(2.83e+04)(2.20e+05 + s)(1.10e+06 + s)



  Digital Compensator Coefficients

  Name    Value      Normalized    Q15      Hex
  ----    -----      ----------    ---      ---
  a1      -0.397     -0.024        -784     0xFCF0
  a2      0.942      0.057         1859     0x0743
  a3      0.455      0.027         898      0x0382
  b0      16.604     1.000         32764    0x7FFC
  b1      -4.825     -0.291        -9521    0xDACF
  b2      -15.324    -0.923        -30238   0x89E2
  b3      6.105      0.368         12047    0x2F0F


  z-domain transfer function

         u(z)  B0 + B1z^(-1) + B2z^(-2) + B3z^(-3)
  H(z) = --- = -----------------------------------
         e(z)  A0 - A1z^(-1) - A2z^(-2) - A3z^(-3)

          (16.604) + (-4.825)z^(-1) + (-15.324)z^(-2) + (6.105)z^(-3)
  H(z) = -----------------------------------------------------------
          1 - (-0.397)z^(-1) - (0.942)z^(-2) - (0.455)z^(-3)

**/


// Compensator Coefficient Defines
#define VMCONLY_COMP_3P3Z_COEFF_A1      0xFCF0
#define VMCONLY_COMP_3P3Z_COEFF_A2      0x0743
#define VMCONLY_COMP_3P3Z_COEFF_A3      0x0382
#define VMCONLY_COMP_3P3Z_COEFF_B0      0x7FFC
#define VMCONLY_COMP_3P3Z_COEFF_B1      0xDACF
#define VMCONLY_COMP_3P3Z_COEFF_B2      0x89E2
#define VMCONLY_COMP_3P3Z_COEFF_B3      0x2F0F
#define VMCONLY_COMP_3P3Z_POSTSCALER    0x426C
#define VMCONLY_COMP_3P3Z_POSTSHIFT     0xFFFB
#define VMCONLY_COMP_3P3Z_PRESHIFT      0x0000


// Compensator Clamp Limits
#define VMCONLY_COMP_3P3Z_MIN_CLAMP    0x0000
#define VMCONLY_COMP_3P3Z_MAX_CLAMP    0x7FFF


#endif
