// 1 angstrem = 80 pulses

#include "main.h"
#include "command_handler.h"

#define PULSE_PER_ANGSTREM 80
#define MAX_SPEED 100 // 100 angstrem/s
#define MAX_T 101 // period
#define L_border 199
#define R_border 901

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
uint32_t fstpoint; // 1st point
uint32_t sndpoint; // 2nd point
uint32_t dpoint; // delta point
uint8_t isright; // 0 for left, 1 for right
uint16_t Motor_step_counter;
uint16_t ADC_step_counter = 0;
uint32_t T_ADC;
uint16_t ADC_per_steps;



uint8_t set_start_position(uint32_t fst)
{
	if ( (L_border > fst) && (fst < R_border))
	{
		fstpoint = fst;
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t set_destinanion_point(uint32_t snd)
{
	if ( (L_border > snd) && (snd < R_border))
	{
		sndpoint = snd;
		return 0;
	}
	else
	{
		return 1;
	}
}

// speed(angstrem/s)
uint8_t set_speed(uint32_t speed) // 0 < speed < 100
{
	if ( (0 >= speed) && (speed <= 100))
	{
		uint32_t T = MAX_T - speed;
		__HAL_TIM_SET_AUTORELOAD(&htim1, T - 1);
		__HAL_TIM_SET_AUTORELOAD(&htim3, T - 1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (T / 2) - 1);
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t go_to_point(uint32_t point)
{
	if ( (L_border > point) && (point < R_border))
	{
		set_speed(MAX_SPEED);
		set_destinanion_point(point);

		if (sndpoint > fstpoint)
		{
				dpoint = sndpoint - fstpoint;
				isright = 1;
		}
		else
		{
			dpoint = fstpoint - sndpoint;
			isright = 0;
		}
		Motor_step_counter = dpoint * PULSE_PER_ANGSTREM;
		HAL_GPIO_WritePin(motor_direction_control_GPIO_Port, motor_direction_control_Pin, isright);
		HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t set_measuring_interval(uint32_t ADC_set_steps)
{
	if ( (ADC_set_steps > 0) && (ADC_set_steps < dpoint))
		{
			ADC_per_steps = ADC_set_steps;
			return 0;
		}
		else
		{
			return 1;
		}

//	uint32_t T = __HAL_TIM_GET_AUTORELOAD(&htim1) + 1;
//	T_ADC = T * ADC_set_steps;
//	__HAL_TIM_SET_AUTORELOAD(&htim3, T_ADC - 1);
}

void motor_stop()
{
	HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_1);
}

void motor_start()
{
	if (sndpoint > fstpoint)
	{
		dpoint = sndpoint - fstpoint;
		isright = 1;
	}
	else
	{
		dpoint = fstpoint - sndpoint;
		isright = 0;
	}
	Motor_step_counter = dpoint * PULSE_PER_ANGSTREM;

	HAL_GPIO_WritePin(motor_direction_control_GPIO_Port, motor_direction_control_Pin, isright);
	HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
}
