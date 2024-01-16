/**
 * @file cmu.c
 * @author Shambaditya Tarafder
 * @date 9/23/2021
 * @brief This file is responsible for enabling all the oscillators and clock
 *        tree.
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *It is responsible for enabling all the oscillators and routing the clock tree
 *for the applications
 *
 *
 * @details
 *CMU is going to enable the oscillators.The LFRCO AND LFXO is going to be
 *disabled.Routing the LF clock to the LF clock tree and enabling clock tree
 *onto LE branches.
 *
 *
 * @note
 *This should always be run/worked on before toucking any other things
 *
 ******************************************************************************/

void cmu_open(void){

    CMU_ClockEnable(cmuClock_HFPER, true);

    // By default, LFRCO is enabled, disable the LFRCO oscillator
    // Disable the LFRCO oscillator
    // What is the enumeration required for LFRCO?
    // It can be found in the online HAL documentation
    CMU_OscillatorEnable(cmuOsc_LFRCO , false, false);

    // Disable the LFXO oscillator
    // What is the enumeration required for LFXO?
    // It can be found in the online HAL documentation
    CMU_OscillatorEnable(cmuOsc_LFXO, false, false);

    // No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H1

    // Route LF clock to the LF clock tree
    // What is the enumeration required to placed the ULFRCO onto the proper clock branch?
    // It can be found in the online HAL documentation
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);    // routing ULFRCO to proper Low Freq clock tree

    // What is the proper enumeration to enable the clock tree onto the LE clock branches?
    // It can be found in the Assignment 2 documentation
     CMU_ClockEnable(cmuClock_CORELE, true);

     CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

}

