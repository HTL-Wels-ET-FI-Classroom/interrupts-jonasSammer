/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

GPIO_InitTypeDef pa0;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "copyright xyz", CENTER_MODE);




	pa0.Alternate=0;
	pa0.Mode=GPIO_MODE_IT_RISING;
	pa0.Pull = GPIO_NOPULL;
	pa0.Pin = GPIO_PIN_0;
	pa0.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &pa0);

	HAL_NVIC_EnableIRQ());


	int cntTimer1 = 0;
	int cntTimer2 = 0;
	int toggleUserbutton=0;
	int toggle_hlfm=1;
	/* Infinite loop */
	while (1)
	{
		//execute main loop every 100ms
		HAL_Delay(100);

		// display timer


		LCD_SetFont(&Font20);
		LCD_SetTextColor(LCD_COLOR_BLUE);
		LCD_SetPrintPosition(3, 0);
		printf("   EXTI Interupt");


		if(toggleUserbutton==0){
			cntTimer1++;
			LCD_SetPrintPosition(5, 0);
			printf("  Timer1: %.1f", cntTimer1/10.0);
		}else if(toggleUserbutton==1){
			cntTimer2++;
			LCD_SetPrintPosition(6, 0);
			printf("  Timer2: %.1f", cntTimer2/10.0);
		}


		if(GetUserButtonPressed()){
			if(toggleUserbutton==0 && toggle_hlfm==1){
				toggleUserbutton=1;
				toggle_hlfm=0;
			}else if(toggleUserbutton==1 && toggle_hlfm==1){
				toggleUserbutton=0;
				toggle_hlfm=0;
			}
		}else{
		toggle_hlfm=1;
		}





		// test touch interface















		int x, y;
		if (GetTouchState(&x, &y)) {
			LCD_FillCircle(x, y, 5);
		}











	}
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


