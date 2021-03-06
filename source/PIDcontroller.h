/*
 * PIDcontroller.h
 *
 *  Created on: 21 mar. 2020
 *      Author: leandro
 */

#ifndef PIDCONTROLLER_H_
#define PIDCONTROLLER_H_


#include "structs.h"

/*************************************************************************
 * Definitions
 ************************************************************************/
#define PITCH_REFERENCE	0U
#define ROLL_REFERENCE	0U

#define KP_PITCH	1.0
#define KI_PITCH	1.0
#define KD_PITCH	1.0

#define KP_ROLL		1.2
#define KI_ROLL		0.0
#define KD_ROLL		0.0



/*********************************************************************************************
 * @brief PID controller for pitch angle
 *
 * @param Pitch data structure
 *
 * @return action control for pitch angle
 *********************************************************************************************/
float getPitchPID(struct pitchStruct *pitchData);


/*********************************************************************************************
 * @brief PID controller for roll angle
 *
 * @param Roll data structure
 *
 * @return action control for roll angle
 *********************************************************************************************/
float getRollPID(struct rollStruct *rollData);



/*********************************************************************************************
 * @brief Estimates the proportional error
 *
 * @param input angle
 * @param output angle
 *
 * @return proportional error
 *********************************************************************************************/
float pError(float input, float output);



/*********************************************************************************************
 * @brief Estimates the integral error
 *
 * @param last integral error
 * @param proportional error
 * @param differential time between errors
 *
 * @return integral error
 *********************************************************************************************/
float iError(float last, float p_error, float dt);


/*********************************************************************************************
 * @brief Estimates the derivative error
 *
 * @param proportional error
 * @param last proportional error
 *
 * @return derivative error
 *********************************************************************************************/
float dError(float p_error, float last_p_error);




#endif /* PIDCONTROLLER_H_ */
