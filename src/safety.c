#include <sbk/safety.h>

#include <sbk/joint.h>
#include <sbk/low_level.h>


void
position_protect(SbkLowCtrl  *ctrl,
				 SbkLeg      leg)
{
	float hipq, thighq, calfq;

	hipq   = ctrl->joint[leg+0].q;
	thighq = ctrl->joint[leg+1].q;
	calfq  = ctrl->joint[leg+2].q;

	if (SBK_HIP_MIN > hipq)
		ctrl->joint[leg+0].q = SBK_HIP_MIN;
	else if (SBK_HIP_MAX < hipq)
		ctrl->joint[leg+0].q = SBK_HIP_MAX;

	if (SBK_THIGH_MIN > thighq)
		ctrl->joint[leg+1].q = SBK_THIGH_MIN;
	else if (SBK_THIGH_MAX < thighq)
		ctrl->joint[leg+1].q = SBK_THIGH_MAX;

	if (SBK_CALF_MIN > calfq)
		ctrl->joint[leg+2].q = SBK_CALF_MIN;
	else if (SBK_CALF_MAX < calfq)
		ctrl->joint[leg+2].q = SBK_CALF_MAX;
}
