#include <sbk/safety.h>

#include <sbk/joint.h>
#include <sbk/low_level.h>


void
sbk_position_protect(SbkLeg      leg,
					 SbkLowCtrl  *ctrl)
{
	float hipq, thighq, calfq;

	hipq   = ctrl->joint[leg+HIP].q;
	thighq = ctrl->joint[leg+THIGH].q;
	calfq  = ctrl->joint[leg+CALF].q;

	if (SBK_HIP_MIN > hipq)
		ctrl->joint[leg+HIP].q = SBK_HIP_MIN;
	else if (SBK_HIP_MAX < hipq)
		ctrl->joint[leg+HIP].q = SBK_HIP_MAX;

	if (SBK_THIGH_MIN > thighq)
		ctrl->joint[leg+THIGH].q = SBK_THIGH_MIN;
	else if (SBK_THIGH_MAX < thighq)
		ctrl->joint[leg+THIGH].q = SBK_THIGH_MAX;

	if (SBK_CALF_MIN > calfq)
		ctrl->joint[leg+CALF].q = SBK_CALF_MIN;
	else if (SBK_CALF_MAX < calfq)
		ctrl->joint[leg+CALF].q = SBK_CALF_MAX;
}
