/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    FRDM-K64F_FreeRTOS_Quadcopter.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

// My own includes
#include "UART_Tasks.h"
#include "SensorData_Tasks.h"
#include "MotorControl_Tasks.h"
#include "structs.h"







/*******************************************************************************
 * Globals
 ******************************************************************************/
// Definition of handle to update motors
QueueHandle_t motors_queue = NULL;


/*********************************************************************
 * @brief   Main function
 ********************************************************************/
int main(void)
{
  	// Initialization of board hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	// Initialization of FSL debug console.
    BOARD_InitDebugConsole();

    // FTM0 module initialization
	FTM0_init(FTM_MODULE);
	PRINTF("FTM0 module initialized!!\r\n");


	// LEDs initialization
	GPIO_PortSet(BOARD_LED_RED_GPIO, 1u << BOARD_LED_RED_PIN);
	GPIO_PortSet(BOARD_LED_GREEN_GPIO, 1u << BOARD_LED_GREEN_PIN);
	GPIO_PortSet(BOARD_LED_BLUE_GPIO, 1u << BOARD_LED_BLUE_PIN);


	// Motor queue creation
	motors_queue = xQueueCreate(10, sizeof(Attitude_Joystick_Data_t));


    // Variables
    BaseType_t pass_or_nopass;

    /*********************************************
    * Task creation goes here
    *********************************************/
    // Task for UART4 module
    NVIC_SetPriority(UART_RX_TX_IRQn, 2);
    pass_or_nopass = xTaskCreate(UART_Rx_Task,
    		"UART4 Task",
			configMINIMAL_STACK_SIZE + 100,
			NULL,
			configMAX_PRIORITIES - 1,
			NULL);
    if (pass_or_nopass != pdPASS)
    {
    	PRINTF("UART_Rx_Task creation failed.\r\n");
    	while (1);
    }

    // Task to acquire Accel/Gyro/Magnet data from sensors
    NVIC_SetPriority(I2C1_IRQN, 5);
    pass_or_nopass = xTaskCreate(SensorData_task,
    		"Sensors Task",
			configMINIMAL_STACK_SIZE + 100,
			NULL,
			configMAX_PRIORITIES - 2,
			NULL);

    // Task to control change motors throttle
    pass_or_nopass = xTaskCreate(ControllingMotors_task,
    		"Motors Task",
			configMINIMAL_STACK_SIZE + 100,
			NULL,
			configMAX_PRIORITIES - 1,
			NULL);


    /*********************************************
    * Initialization of task scheduler
	*********************************************/
    vTaskStartScheduler();

    /********************************************/
    for (;;);
    return 0 ;
}

