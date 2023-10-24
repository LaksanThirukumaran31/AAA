/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include <stdio.h>
#include <string.h>
#include "tim.h"
#include "adc.h"

#define PWM_MAX 100
#define DUTY_MAX 4250
#define PWM_MIN 0

uint8_t prompt[]="user@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;

void Shell_Init(void){
	memset(argv, NULL, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, NULL, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, NULL, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"help")==0){
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Print all available functions here\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"speed")==0){
			float speedVal=atoi(argv[1]);
			if (speedVal> PWM_MAX){
				speedVal=PWM_MAX;
			}
			if (speedVal< PWM_MIN){
				speedVal=PWM_MIN;
			}

			speedVal=(DUTY_MAX*speedVal)/100; //PWM en pourcentage
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,speedVal);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,DUTY_MAX - speedVal);

		}
		/*
		 * start -> Activation des PWM avec un rapport cyclique de 50%
		 * stop  -> Désactivation des PWM
		 * adc   -> On affiche la mesure du courant
		 */
		else if(strcmp(argv[0],"start")==0){

			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0.5*DUTY_MAX);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0.5*DUTY_MAX);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

		}
		else if(strcmp(argv[0],"stop")==0){
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		}
		else if(strcmp(argv[0],"adc")==0){

			HAL_ADC_Start(&hadc1);
			//HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);
			uint16_t val;
			val= HAL_ADC_GetValue(&hadc1);
			float u_V,I_mes ;
			u_V = (val/4096.0)*3.3;
			I_mes = (u_V-1.65)/0.05;
			char buff[30];
			sprintf(buff, "Courant : %f A\r\n", I_mes);
			HAL_UART_Transmit(&huart2,(uint8_t*)buff,strlen(buff),HAL_MAX_DELAY);

		}


		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}
