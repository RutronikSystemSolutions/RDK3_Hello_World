/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the RDK3 Hello World
*              Application for ModusToolbox.
*
* Related Document: See README.md
*
*
*  Created on: 2022-10-16
*  Company: Rutronik Elektronische Bauelemente GmbH
*  Address: Jonavos g. 30, Kaunas 44262, Lithuania
*  Author: GDR
*
*******************************************************************************
* (c) 2019-2021, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*
* Rutronik Elektronische Bauelemente GmbH Disclaimer: The evaluation board
* including the software is for testing purposes only and,
* because it has limited functions and limited resilience, is not suitable
* for permanent use under real conditions. If the evaluation board is
* nevertheless used under real conditions, this is done at one’s responsibility;
* any liability of Rutronik is insofar excluded
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/*Priority for button interrupts*/
#define BTN_IRQ_PRIORITY		5

/*Function prototypes used for this demo.*/
void handle_error(void);
void btn_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

/*A global variable for switching LEDs*/
_Bool led_select = false;
/* Variable for storing character read from terminal */
uint8_t uart_read_value;

cyhal_gpio_callback_data_t btn_data =
{
		.callback = btn_interrupt_handler,
		.callback_arg = NULL,

};

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
    	handle_error();
    }

    __enable_irq();

    /*Initialize LEDs*/
    result = cyhal_gpio_init( LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}
    result = cyhal_gpio_init( LED3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Initialize Buttons*/
    result = cyhal_gpio_init(USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
    if (result != CY_RSLT_SUCCESS)
    {handle_error();}

    /*Register callback functions */
    cyhal_gpio_register_callback(USER_BTN, &btn_data);

    /* Enable falling edge interrupt events */
    cyhal_gpio_enable_event(USER_BTN, CYHAL_GPIO_IRQ_FALL, BTN_IRQ_PRIORITY, true);

    /*Enable debug output via KitProg UART*/
    result = cy_retarget_io_init( KITPROG_TX, KITPROG_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        handle_error();
    }

    printf("\x1b[2J\x1b[;H");
    printf("Hello from the RDK3!\r\n");
    printf("Press the BTN1 Button or any key to change the blinking LEDs\r\n");

    for (;;)
    {
    	/* Toggle the LED*/
    	if(led_select)
    	{
    		cyhal_gpio_write(LED3, CYBSP_LED_STATE_OFF);
    		cyhal_gpio_toggle(LED2);
    	}
    	else
    	{
    		cyhal_gpio_write(LED2, CYBSP_LED_STATE_OFF);
    		cyhal_gpio_toggle(LED3);
    	}

        /* Check if 'Enter' key was pressed */
        if (cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 1) == CY_RSLT_SUCCESS)
        {
            if (uart_read_value)
            {
            	led_select = !led_select;
            }

            /* Declare LED status*/
            if(led_select)
            {
            	printf("LED2 is selected.\r\n");
            }
            else
            {
            	printf("LED3 is selected.\r\n");
            }
        }

    	/*Delay 500 milliseconds*/
    	CyDelay(500);
    }
}

/* Interrupt handler callback function */
void btn_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
	CY_UNUSED_PARAMETER(handler_arg);
    CY_UNUSED_PARAMETER(event);

    /*Changes the LEDs*/
    led_select = !led_select;
    if(led_select)
    {
    	printf("LED2 is selected.\r\n");
    }
    else
    {
    	printf("LED3 is selected.\r\n");
    }
}


/*If initialization fails, program ends up here.*/
void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}

/* [] END OF FILE */
