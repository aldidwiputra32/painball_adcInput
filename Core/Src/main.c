/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define VREFINT_CAL_ADDR	0x1FF80078  /* datasheet p. 19 */
#define VREFINT_CAL 		((uint16_t*) VREFINT_CAL_ADDR)
#define CH1 				2
#define CH2 				0
#define CH3 				1
#define VDDA_CH 			3
#define NUM_SAMPLE 			200
#define NUM_CHANNEL 		4
//#define NUM_CHANNEL 		3
#define TRESHOLD1			2580
#define TRESHOLD2			2553
#define TRESHOLD3			2553
#define TRESHOLD_GENERAL	10
#define INTERVAL  			1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t vdda = 0;
//uint32_t adcBuffer[NUM_SAMPLE * NUM_CHANNEL];
uint32_t adcBuffer[4];
uint32_t vrms[3];
uint32_t vAdcVrmsOff[3];
uint32_t voltBuffer1[200];
uint32_t voltBuffer2[200];
uint32_t voltBuffer3[200];
double sum1;
double sum2;
double sum3;
uint32_t timer = 0;
uint8_t stateEvent = 0;
uint8_t bufferString[100];
uint16_t counterLog = 0;
volatile uint8_t stateFinishADC = 0;
uint8_t stateCH[3];
uint8_t stateCHOld[3];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC_Init();
	MX_TIM2_Init();
	MX_LPUART1_UART_Init();

	/* USER CODE BEGIN 2 */
	timer = HAL_GetTick();
	// CALBRATION ADC BEFORE STARTING
	if(HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED) != HAL_OK){
		Error_Handler();
	}else __asm__("nop");

	// STARTING ADC USING DMA METHOD
	if(HAL_ADC_Start_DMA(&hadc, adcBuffer, 4) != HAL_OK){
		Error_Handler();
	}else __asm__("nop");

	// INIT OFF VALUE VRMS
	while(stateFinishADC==0){}
	for(uint8_t i=0;i<3;i++){
		if(stateFinishADC){
			stateFinishADC = 0;
			sum1 = sum2 = sum3 = 0;
			for(uint8_t i=0;i<200;i++){
				vdda = (3000UL * *VREFINT_CAL) / adcBuffer[VDDA_CH];
				voltBuffer1[i] = (adcBuffer[0] * vdda) / 4095;
				sum1 += voltBuffer1[i] * voltBuffer1[i];
				voltBuffer2[i] = (adcBuffer[1] * vdda) / 4095;
				sum2 += voltBuffer2[i] * voltBuffer2[i];
				voltBuffer3[i] = (adcBuffer[2] * vdda) / 4095;
				sum3 += voltBuffer3[i] * voltBuffer3[i];
				HAL_Delay(1);
			}
			vAdcVrmsOff[0] = sqrt(sum1/200);
			vAdcVrmsOff[1] = sqrt(sum2/200);
			vAdcVrmsOff[2] = sqrt(sum3/200);
		}
	}
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		if(stateFinishADC){
			stateFinishADC = 0;
			sum1 = sum2 = sum3 = 0;
			for(uint8_t i=0;i<200;i++){
				vdda = (3000UL * *VREFINT_CAL) / adcBuffer[VDDA_CH];
				voltBuffer1[i] = (adcBuffer[0] * vdda) / 4095;
				sum1 += voltBuffer1[i] * voltBuffer1[i];
				voltBuffer2[i] = (adcBuffer[1] * vdda) / 4095;
				sum2 += voltBuffer2[i] * voltBuffer2[i];
				voltBuffer3[i] = (adcBuffer[2] * vdda) / 4095;
				sum3 += voltBuffer3[i] * voltBuffer3[i];
				HAL_Delay(1);
			}
			vrms[0] = sqrt(sum1/200);
			vrms[1] = sqrt(sum2/200);
			vrms[2] = sqrt(sum3/200);
		}

		// CLEARING DATA BUFFER STRING
		for(uint8_t index=0;index>30;index++){bufferString[index]=0;}
		// GETTING STATE CHANNEL
		if(vrms[CH1] > vAdcVrmsOff[CH1]+TRESHOLD_GENERAL){stateCH[CH1] = 1;}
		else{stateCH[CH1] = 0;}
		if(vrms[CH2] > vAdcVrmsOff[CH2]+TRESHOLD_GENERAL){stateCH[CH2] = 1;}
		else{stateCH[CH2] = 0;}
		if(vrms[CH3] > vAdcVrmsOff[CH3]+TRESHOLD_GENERAL){stateCH[CH3] = 1;}
		else{stateCH[CH3] = 0;}

		// SEND STATUS CHANNEL >> ROUTINE
		if(((HAL_GetTick()-timer)>=INTERVAL)){
			uint16_t sizeString = 0;
			// RESET TIMER AND STATE
			timer = HAL_GetTick();
			// GENERATE TEXT >> STATE
			sprintf(bufferString,"[%d],CH1:%d,CH2:%d,CH3:%d;\r\n",counterLog,stateCH[CH1],stateCH[CH2],stateCH[CH3]);
			// GENERATE TEXT >> FLOAT VOLTAGE
//			sprintf(bufferString,"[%d],CH1:%d(%d)=%d,CH2:%d(%d)=%d,CH3:%d(%d)=%d;\r\n",counterLog,vrms[CH1],vAdcVrmsOff[CH1],stateCH[CH1],vrms[CH2],vAdcVrmsOff[CH2],stateCH[CH2],vrms[CH3],vAdcVrmsOff[CH3],stateCH[CH3]);

			// COUNTING SIZE OF FRAM STRING
			for(uint8_t i=0;i<100;i++){
				if(bufferString[i]==0){
					sizeString = i;
					break;
				}
			}
			// TRANSMIT DATA USING UART PERIPHERAL
			HAL_UART_Transmit(&hlpuart1, bufferString, sizeString, 1000);
			// COUNTER LOG
			counterLog += 1;
			if(counterLog  >= 0xffff){
				counterLog = 0;
			}
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc){
	if(hadc->Instance == ADC1){
		stateFinishADC = 1;
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
