/*
 * SOC.c
 *
 *  Created on: 2022. 12. 02.
 *      Author: BSKIM
 */
#include "main.h"
#include "SOC.h"
#include "System_Info.h"
#include "Parameter.h"

/* global variable START */
int SOC		=	-1;
int now_c	=	-1;
int max_c	=   -1;


#if(0)
#define len_OCV		40

uint16_t SOC_M50LT_410[40] =
{
		1000,  999,   994,   988,   980,
		967,   934,   910,   890,   856,
		827,   799,   760,   729,   704,
		673,   637,   597,   567,   532,
		503,   465,   428,   405,   378,
		344,   306,   270,   242,   217,
		190,   168,   130,   107,   90,
		68,    38,    16,    3,     0
};
uint16_t OCV_M50LT_410[40] =
{
		4169,   4129,   4138,   4125,   4116,
		4101,   4079,   4069,   4062,   4052,
		4041,   4027,   3994,   3961,   3934,
		3907,   3880,   3859,   3844,   3822,
		3802,   3765,   3727,   3706,   3682,
		3657,   3635,   3614,   3598,   3583,
		3564,   3550,   3519,   3498,   3480,
		3454,   3419,   3392,   3372,   3368
};

#else

#define len_OCV		97
//	주행시험 전
uint16_t SOC_M50LT_410[97]=
{
	1000, 989, 978, 968, 957, 946, 935, 925, 914, 903,
	892,  882, 871, 860, 849, 839, 828, 817, 806, 796,
	785,  774, 763, 753, 745, 739, 730, 720, 711, 697,
	685,  672, 658, 645, 630, 618, 603, 593, 578, 565,
	552,  535, 525, 515, 499, 480, 471, 460, 450, 440,
	434,  425, 417, 405, 396, 387, 375, 364, 354, 344,
	334,  307, 302, 296, 290, 285, 281, 269, 256, 245,
	235,  229, 220, 211, 202, 193, 184, 175, 162, 150,
	140,  127, 117, 106,  95,  82,  69,  61,  53,  42,
	36,    30,  24,  19,   7,   3,   0
};
//주행시험 전
uint16_t OCV_M50LT_410[97] =
{
	4075, 4067, 4060, 4052, 4043, 4034, 4024, 4014, 4003, 3993,
	3983, 3973, 3964, 3955, 3947, 3939, 3932, 3926, 3920, 3914,
	3908, 3902, 3896, 3890, 3883, 3876, 3869, 3861, 3852, 3843,
	3832, 3821, 3808, 3796, 3784, 3772, 3761, 3751, 3741, 3732,
	3723, 3714, 3706, 3698, 3690, 3683, 3677, 3670, 3664, 3659,
	3653, 3648, 3642, 3637, 3633, 3628, 3623, 3618, 3613, 3608,
	3603, 3599, 3594, 3588, 3583, 3578, 3572, 3567, 3561, 3554,
	3547, 3540, 3532, 3525, 3517, 3509, 3501, 3493, 3484, 3473,
	3462, 3452, 3441, 3428, 3416, 3407, 3399, 3391, 3384, 3375,
	3367, 3357, 3344, 3319, 3274, 3241, 3200,
};
#endif

/* global variable  END  */

/* static variable START */

/* static variable  END  */

void set_Max_Capacity()
{
	max_c= Parameter.s16_CAPACITY * 36000 ;
}

void get_SOC_from_OCVTable(int volt)
{
	if(volt >= OCV_M50LT_410[0] )
	{
		now_c = Parameter.s16_CAPACITY * 1000 * 36 ;
	}
	else if( volt < OCV_M50LT_410[len_OCV-1])
	{
		now_c = Parameter.s16_CAPACITY * 0 * 36 ;
	}
	else
	{
		int i;
		for(i=0; i<len_OCV ; i++)
		{
			if( volt >= OCV_M50LT_410[i])
			{
				break;
			}
		}
		now_c = Parameter.s16_CAPACITY * SOC_M50LT_410[i] * 36;
	}

	System_Info.Lifecycle.s32_now_c = now_c;
}

static int test_now_c;
void LB_driving_SOC(int volt)
{
	int j;

	for (j=0; j<len_OCV ; j++)
	{
		if(volt >= OCV_M50LT_410[j])
		{
			break;
		}
	}

	test_now_c = Parameter.s16_CAPACITY * SOC_M50LT_410[j] * 36;

	if ((now_c * (1.08) > test_now_c) && (now_c * (0.92) < test_now_c))
	{
		//pass;
	}
	else
	{
		get_SOC_from_OCVTable(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);
	}
}

uint16_t Idle_count = 0;
void Calc_SOC(int current)
{
	if((-200 < current) && (current < 200)) // -2.00A and 2.00A
	{
		Idle_count++;
		if(Idle_count > 300) //5Minute SOC compensation
		{
			Idle_count = 0;
			get_SOC_from_OCVTable(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);
		}
	}
	else
	{
		Idle_count = 0;

		now_c = now_c + current;
		if(now_c < 0)
		{
			now_c = 0;
		}
		else if(now_c > max_c)
		{
			now_c = max_c;
		}
	}

	static double temp_SOC;
	temp_SOC = ((double)now_c / (double)max_c);

	SOC =  (int)(temp_SOC * 1000);

	System_Info.Lifecycle.s32_max_c = max_c;
	System_Info.Lifecycle.s32_now_c = now_c;
	System_Info.Lifecycle.u16_SOC = SOC;
	System_Info.Lifecycle.u8_SOC = (char)(SOC/10);
	//log_debug("SOC : %d",SOC);
}
