/**
   @file
   @brief Unitree Go1 safety function definitions
  */

#ifndef __SBK_SAFETY_H
#define __SBK_SAFETY_H


#include <sbk/low_level.h>


/**
   @brief Constrains joint angles to their maximum and minimum values
   @details This function uses the constants for Unitree Go1. If you're willing to use sbk for other models, redefine SBK_HIP_MIN, SBK_HIP_MAX, etc macros (see joint.h)

   @param[in]      leg  Leg number (see joint.h)
   @param[in, out] ctrl SbkLowCtrl instance address
 */

void sbk_position_protect(SbkLeg leg, SbkLowCtrl * ctrl);


#endif // !__SBK_SAFETY_H
