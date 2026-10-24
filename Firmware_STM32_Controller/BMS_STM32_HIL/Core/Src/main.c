/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Embedded BMS Controller (HIL Node)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Controller_BMS.h"
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
CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN PV */
volatile uint32_t dwt_cycles = 0;
volatile float execution_time_us = 0.0f;
CAN_RxHeaderTypeDef rxHeader;
uint8_t rxData[8];
volatile uint32_t last_can_rx_tick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */

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
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();

  /* USER CODE BEGIN 2 */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  // 1. Initialisation du modèle Simulink (Stateflow + ANN)
  Controller_BMS_initialize();

  // 2. Configuration du Filtre CAN (Accepter ID 0x100)
  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = (0x100 << 5);
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = (0x7FF << 5);
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  // 3. Forçage matériel de l'interruption NVIC (Sécurité si omis dans CubeMX)
  HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

  // 4. Démarrage du contrôleur CAN
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  // 5. Activation des notifications de réception FIFO 0
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

  // 6. État initial : Relais armé (LED Allumée)
  HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Initialize leds - DÉSACTIVÉ pour éviter le reset conflictuel de PA5 */
  // BSP_LED_Init(LED_GREEN);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Watchdog FTTI : Coupure du relais si perte du bus CAN > 300 ms
    if ((HAL_GetTick() - last_can_rx_tick) > 300)
    {
      HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_RESET);
    }

    HAL_Delay(10);
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

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 10;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = RELAY_SAFETY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RELAY_SAFETY_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
  {
    if (rxHeader.StdId == 0x100)
    {
      // 1. Rafraîchissement du timer Watchdog
      last_can_rx_tick = HAL_GetTick();

      // 2. Décodage des octets bruts
      uint16_t v_raw = (uint16_t)((rxData[0] << 8) | rxData[1]);
      int16_t  i_raw = (int16_t)((rxData[2] << 8) | rxData[3]);
      int8_t   t_raw = (int8_t)rxData[4];

      // 3. Injection dans les entrées du modèle Simulink
      Controller_BMS_U.V_cell = (real32_T)(v_raw / 1000.0f); // Conversion mV -> Volts
      Controller_BMS_U.I      = (real32_T)(i_raw / 100.0f);  // Conversion cA -> Ampères
      Controller_BMS_U.T      = (real32_T)t_raw;             // Température en °C
      uint32_t t_start = DWT->CYCCNT;

            // 1. Exécution du modèle Simulink (Stateflow + ANN)
            Controller_BMS_step();

            // 2. Décision matérielle sur le relais
            if (Controller_BMS_Y.relay == true)
            {
              HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_SET);
            }
            else
            {
              HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_RESET);
            }

            // ==========================================
            // FIN DE LA MESURE
            // ==========================================
            uint32_t t_end = DWT->CYCCNT;

            dwt_cycles = t_end - t_start;
            execution_time_us = (float)dwt_cycles / 80.0f;
      // 4. Calcul du pas algorithmique (Stateflow + ANN)
      Controller_BMS_step();

      // 5. Commande physique du relais selon la décision du modèle
      if (Controller_BMS_Y.relay == true)
      {
        HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_SET);   // Relais Armé (LED LD2 Allumée)
      }
      else
      {
        HAL_GPIO_WritePin(RELAY_SAFETY_GPIO_Port, RELAY_SAFETY_Pin, GPIO_PIN_RESET); // Safe State (LED LD2 Éteinte)
      }
    }
  }
}
/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
