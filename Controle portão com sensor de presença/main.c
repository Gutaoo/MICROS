/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @authors Augusto Blauth Schneider e Manuela Schneider Gottschalck
  * @Turma 4411
  * @brief          : CONTROLE DE PORTÃO
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int bordas_OC[2]={1,13};
uint32_t borda_IC[2]={0,0};
enum {atual,anterior};
uint32_t distance  = 0;
int aux_IC=0;						//Declaração das variáveis usadas
int aux_OC=0;
int aux_p=1;
enum {
	Aberto,
	Fechado,
	Movimento,
	Parado
}Estado_port;
enum {Verde,
	  Vermelho,
	  Amarelo,
	  Violeta
}cor_led;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) 				//função Output Compare p/ geração do pulso de trigger do sensor
{															  				//o TIM5 possui PSC=(84-1) e ARR=(50000-1), resultando em um período de 100ms
	if(aux_OC++ >1){
		aux_OC = 0;															//controle das bordas do pulso
	}
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, bordas_OC[aux_OC]); 		//quando ocorre um toggle o pulse do OC muda (de 1 para 13, e de 13 para 1)
}																     		//assim temos um pulso ativo de 10us em um período de 100ms


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)	 				//função Input Capture p/ cálculo de distância com a saída do sensor
{
	aux_IC++;
	borda_IC[anterior] = borda_IC[atual];
	borda_IC[atual]  = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);			//captura e atualiza as bordas de subida e descida do pulso no vetor

	if(aux_IC >2){
		aux_IC=1;															//controle das bordas do pulso para cálcular apenas o tempo ativo
	}
	else{
		distance = (abs((borda_IC[atual]-borda_IC[anterior])* 0.034/2)); 	//cálculo da distância conforme o tempo ativo
	}
}


void controle_ponte_H(GPIO_PinState s1, GPIO_PinState s2)  					//função para controle da ponte H
{
	HAL_GPIO_WritePin(out1_GPIO_Port, out1_Pin, s1);
	HAL_GPIO_WritePin(out2_GPIO_Port, out2_Pin, s2);
}

void controle_led(int cor)
{
	if(cor==Verde){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);			//Led verde
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	}
	else if(cor==Vermelho){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);			//Led vermelho
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	}
	else if(cor==Amarelo){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);			//Led amarelo
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 600);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
	}
	else if(cor==Violeta){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 200);			//Led violeta
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 500);
	}
}
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
  MX_TIM5_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
 // ARR_IC =__HAL_TIM_GET_AUTORELOAD(&htim4);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);				//Liga o TIM2 e o canal de geração PWM
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);			//Led amarelo
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 500);

  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);					  //Liga o canal de Input Capture do TIM4
  HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_1);					  //Liga o canal de Output Compare do TIM5

  HAL_NVIC_SetPriority(TIM5_IRQn, 2, 0);						  //Define as prioridades das interrupts
  HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);

  aux_p=1;
  HAL_GPIO_WritePin(out1_GPIO_Port, out1_Pin, GPIO_PIN_RESET);	  //começa o motor parado com prioridade para fechar
  HAL_GPIO_WritePin(out1_GPIO_Port, out2_Pin, GPIO_PIN_RESET);

  int sw_controle_atual, sw_controle_antes=GPIO_PIN_RESET;		  //variáveis para detecção de borda de subida

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  sw_controle_atual=HAL_GPIO_ReadPin(acionamento_GPIO_Port, acionamento_Pin);

	  if(HAL_GPIO_ReadPin(SW_FECHADO_GPIO_Port, SW_FECHADO_Pin)){		//Se o botão está pressionado o portão está fechado
		  Estado_port=Fechado;
		  aux_p=2;
		  controle_led(cor_led=Vermelho);
	  }
	  else if(HAL_GPIO_ReadPin(SW_ABERTO_GPIO_Port, SW_ABERTO_Pin)){	//Se o botão está pressionado o portão está aberto
		  Estado_port=Aberto;
 		  controle_led(cor_led=Verde);
		  aux_p=1;
	  }																	//(if abaixo)Se nenhum dos botões estão pressionados o portão está em movimento ou parado
	  if(HAL_GPIO_ReadPin(SW_ABERTO_GPIO_Port, SW_ABERTO_Pin)==GPIO_PIN_RESET && HAL_GPIO_ReadPin(SW_FECHADO_GPIO_Port, SW_FECHADO_Pin)==GPIO_PIN_RESET){
		  if(HAL_GPIO_ReadPin(out1_GPIO_Port,out1_Pin)==HAL_GPIO_ReadPin(out2_GPIO_Port,out2_Pin)){
			  Estado_port=Parado;                                       //(if acima)Se os pinos de controle da ponte H tem o mesmo valor o portão está parado
			  controle_led(cor_led=Violeta);
		  }
	  	  else{															//Se não o portão está em movimento
	  		  Estado_port=Movimento;
	  		  controle_led(cor_led=Amarelo);
	  	  }
	  }

		  if(sw_controle_atual!=sw_controle_antes){						//Detecção de borda de subida do botão de acionamento
			  sw_controle_antes=sw_controle_atual;
			  if(sw_controle_atual){
				  switch(Estado_port){									//Dependendo do estado do portão o acionamento faz algo diferente
	  	  	  	  	  case Aberto:
	  	  	  	  	if (distance>=7&&distance<=8){                             //Se o caminho não esta obstruído (7 < distância padrão até o solo < 8)
	  	  	  	  		  controle_ponte_H(GPIO_PIN_SET,GPIO_PIN_RESET);       //FECHA o portão
	  	  	  	  	}
	  	  	  	  		  break;
	  	  	  	  	  case Fechado:
	  	  	  	  		  controle_ponte_H(GPIO_PIN_RESET,GPIO_PIN_SET);       //ABRE o portão
	  	  	  	  		  break;
	  	  	  	  	  case Movimento:
	  	  	  	  		  controle_ponte_H(GPIO_PIN_RESET,GPIO_PIN_RESET);     //PARA o portão
	  	  	  	  	  break;
	  	  	  	  	  case Parado:
	  	  	  	  		  if(aux_p==2){										   //Se estava fechado pela última vez
	  	  	  	  			  controle_ponte_H(GPIO_PIN_SET,GPIO_PIN_RESET);   //FECHA o portão
	  	  	  	  		  }
	  	  	  	  		  else{												   //Se estava aberto pela última vez
	  	  	  	  			  controle_ponte_H(GPIO_PIN_RESET,GPIO_PIN_SET);   //ABRE o portão
	  	  	  	  		  }
	  	  	  	  	  break;
				  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


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
