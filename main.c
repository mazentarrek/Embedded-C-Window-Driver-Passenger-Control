#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"
#include <semphr.h>
#include "queue.h"
#include "DIO.h"

#define MOTOR_CW     0
#define MOTOR_CCW    1
#define MOTOR_STOP   2

xSemaphoreHandle xBinarySemaphore;
xQueueHandle xAutomaticMotorCommandQueue;
xQueueHandle xManualMotorCommandQueue;

/* Define your Tasks Here */

void vUserInterface(void *pvParameters);
void vManualMotorTask(void *pvParameters);
void vAutomaticMotorTask(void *pvParameters);
void vObstacleHandlerTask(void *pvParameters);

int ManualDriverUpButton = 0;
int ManualDriverDownButton = 0;
int ManualPassengerUpButton = 0;
int ManualPassengerDownButton = 0;

int AutomaticDriverUpButton = 0;
int AutomaticDriverDownButton = 0;
int AutomaticPassengerUpButton = 0;
int AutomaticPassengerDownButton = 0;

int lockbutton = 0;
int limitswitchUp = 0;
int limitswitchDown=0;

int ObstacleButton = 0;

void Motor_Control(uint8_t dir);

void vManualMotorTask(void *pvParameters) {
	
	while(1) {
		ObstacleButton = DIO_ReadPin(&GPIO_PORTD_DATA_R,2);
		lockbutton = DIO_ReadPin(&GPIO_PORTD_DATA_R,1);
		
		int xReceivedStructure;

		int xValue = xQueueReceive(xManualMotorCommandQueue, &xReceivedStructure, portMAX_DELAY );
		
		if (lockbutton == 1) {
			
			if ( (xReceivedStructure == 1)) {              //manual driver up
				
				while ( (DIO_ReadPin(&GPIO_PORTD_DATA_R,6) == 1) && (DIO_ReadPin(&GPIO_PORTB_DATA_R,2)!=1) )  {
				
					if( DIO_ReadPin(&GPIO_PORTD_DATA_R,2)==1 ){
					xSemaphoreGive(xBinarySemaphore);
						break;
				}
				
				Motor_Control(MOTOR_CW); // delay to let motor be on 
	
				}
				
				Motor_Control(MOTOR_STOP);
			}
			
			else if (xReceivedStructure == 2)  {          //manual driver down	
				
				while ( (DIO_ReadPin(&GPIO_PORTD_DATA_R,7) == 1) && (DIO_ReadPin(&GPIO_PORTB_DATA_R,2)!=1) )  {
				
					Motor_Control(MOTOR_CCW); // delay to let motor be on 
				
				}
				Motor_Control(MOTOR_STOP);
			
		
	}
}
		
		if (lockbutton == 0) {
			
			if ( (xReceivedStructure == 1)) {              //manual driver up
				
				while ( (DIO_ReadPin(&GPIO_PORTD_DATA_R,6) == 1) && (DIO_ReadPin(&GPIO_PORTB_DATA_R,2)!=1) )  {
				
				Motor_Control(MOTOR_CW); // delay to let motor be on 	
				
				if( DIO_ReadPin(&GPIO_PORTD_DATA_R,2)==1 ){
					xSemaphoreGive(xBinarySemaphore);
						break;
					// give semaphore
				}
				
				}
				Motor_Control(MOTOR_STOP);
			}
			
			else if (xReceivedStructure == 2)  {          //manual driver down
				
				while ((DIO_ReadPin(&GPIO_PORTD_DATA_R,7) == 1 ) &&(DIO_ReadPin(&GPIO_PORTE_DATA_R,5) != 1)) {
	
					Motor_Control(MOTOR_CCW);
				}
					Motor_Control(MOTOR_STOP);
				}
			
			else if ( (xReceivedStructure == 3)) { 			//manual passenger up
				
				while ( (DIO_ReadPin(&GPIO_PORTB_DATA_R,1) == 1) && (DIO_ReadPin(&GPIO_PORTB_DATA_R,2)!=1) )  {
				
				Motor_Control(MOTOR_CW); // delay to let motor be on 	
				
				if( DIO_ReadPin(&GPIO_PORTD_DATA_R,2)==1 ){
					xSemaphoreGive(xBinarySemaphore);
						break;
					// give semaphore
				}
				
				Motor_Control(MOTOR_CW); // delay to let motor be on 	
				
				}
				Motor_Control(MOTOR_STOP);
			}
			
			else if (xReceivedStructure == 4)  {          //manual passenger down
				
				while ((DIO_ReadPin(&GPIO_PORTE_DATA_R,0) == 1 ) &&(DIO_ReadPin(&GPIO_PORTE_DATA_R,5) != 1)) {
	
					Motor_Control(MOTOR_CCW);
				}
							Motor_Control(MOTOR_STOP);
				}
			}
	
}
	}



void vAutomaticMotorTask(void *pvParameters)
{
	
	while(1) {
		ObstacleButton = DIO_ReadPin(&GPIO_PORTD_DATA_R,2);
	
		int xReceivedStructure;
		

		int limitswitch = 0;
		
		lockbutton = DIO_ReadPin(&GPIO_PORTD_DATA_R,1);
		
		
		int xValue = xQueueReceive(xAutomaticMotorCommandQueue, &xReceivedStructure, portMAX_DELAY );
		
		if (lockbutton == 1) {
		
		if ( (xReceivedStructure == 1)) {  //automatic driver up
					while(1){
					if( DIO_ReadPin(&GPIO_PORTD_DATA_R,2)==1 ){
					xSemaphoreGive(xBinarySemaphore);
						break;
					// give semaphore
				}
					Motor_Control(MOTOR_CW);
				
					if(DIO_ReadPin(&GPIO_PORTB_DATA_R,2)==1){ //limit up
				 	Motor_Control(MOTOR_STOP);
						//xSemaphoreGive(xBinarySemaphore); // was obstacle 
						break;
					}
				}
					
				Motor_Control(MOTOR_STOP);
				
			}
			
			else if (xReceivedStructure == 2)  { //automatic driver down
				
				Motor_Control(MOTOR_STOP);
				
				while(1){

					GPIO_PORTB_DATA_R |= 0xE0;
					Motor_Control(MOTOR_CCW);
			  	for(int i =0 ;i<1000;i++);
					Motor_Control(MOTOR_CCW);
				
					if(DIO_ReadPin(&GPIO_PORTE_DATA_R,5)==1){ //limit up
				 	Motor_Control(MOTOR_STOP);
						//xSemaphoreGive(xBinarySemaphore); // was obstacle 
						break;
					}	
					
					
				}
				
				Motor_Control(MOTOR_STOP);
			
	}
}
		
		else if (lockbutton == 0) {
			
			if ( (xReceivedStructure == 1) || (xReceivedStructure == 3) ) {  //auto window up (driver or passenger)
				Motor_Control(MOTOR_STOP);
          //Motor_Control(MOTOR_CW);
				
				while(1){
					if( DIO_ReadPin(&GPIO_PORTD_DATA_R,2)==1 ){
					xSemaphoreGive(xBinarySemaphore);
						break;
					// give semaphore
				}
					Motor_Control(MOTOR_CW);
					for(int i =0 ;i<1000;i++);
					Motor_Control(MOTOR_CW);
					if(DIO_ReadPin(&GPIO_PORTB_DATA_R,2)==1){ //limit up
				 	Motor_Control(MOTOR_STOP);
						//xSemaphoreGive(xBinarySemaphore); // was obstacle 
						break;
					}
				}
			}
			
			else if ((xReceivedStructure == 2) || (xReceivedStructure == 4))   {  //window down (driver or passenger)
				Motor_Control(MOTOR_STOP);
				//Motor_Control(MOTOR_CCW);
				while(1){
				//	for(int i =0 ;i<1000;i++);

					Motor_Control(MOTOR_CCW);
			  	for(int i =0 ;i<1000;i++);
					Motor_Control(MOTOR_CCW);
					if(DIO_ReadPin(&GPIO_PORTE_DATA_R,5)==1){ //limit up
				 	Motor_Control(MOTOR_STOP);
						//xSemaphoreGive(xBinarySemaphore); // was obstacle 
						break;
					}
				
		}
		

	}
	
}
}
	}

void vUserInterface(void *pvParameters) {

	
		while(1) {
			lockbutton = DIO_ReadPin(&GPIO_PORTD_DATA_R,1);
			
			ManualDriverUpButton = DIO_ReadPin(&GPIO_PORTD_DATA_R,6);
			ManualDriverDownButton = DIO_ReadPin(&GPIO_PORTD_DATA_R,7);
			ManualPassengerUpButton = DIO_ReadPin(&GPIO_PORTB_DATA_R,1);
			ManualPassengerDownButton = DIO_ReadPin(&GPIO_PORTE_DATA_R,0);
			
			limitswitchUp = DIO_ReadPin(&GPIO_PORTB_DATA_R,3);
			limitswitchDown = DIO_ReadPin(&GPIO_PORTE_DATA_R,5);
			
			
			AutomaticDriverUpButton = DIO_ReadPin(&GPIO_PORTB_DATA_R,0);
			AutomaticDriverDownButton = DIO_ReadPin(&GPIO_PORTD_DATA_R,3);
		  AutomaticPassengerUpButton = DIO_ReadPin(&GPIO_PORTE_DATA_R,2);
		  AutomaticPassengerDownButton = DIO_ReadPin(&GPIO_PORTE_DATA_R,3);
			

			int MOTOR_COMMAND_Driver_UP = 1;
			int MOTOR_COMMAND_Driver_DOWN = 2;
			int MOTOR_COMMAND_Passenger_UP = 3;
			int MOTOR_COMMAND_Passenger_DOWN = 4;
			

			if (AutomaticDriverUpButton == 1) {
			xQueueSend(xAutomaticMotorCommandQueue, &MOTOR_COMMAND_Driver_UP, portMAX_DELAY );
			}

			if (AutomaticDriverDownButton == 1) {
			xQueueSend(xAutomaticMotorCommandQueue, &MOTOR_COMMAND_Driver_DOWN, 0);
			}

			if (AutomaticPassengerUpButton == 1) {
			xQueueSend(xAutomaticMotorCommandQueue, &MOTOR_COMMAND_Passenger_UP, 0);
			}

			if (AutomaticPassengerDownButton == 1) {
			xQueueSend(xAutomaticMotorCommandQueue, &MOTOR_COMMAND_Passenger_DOWN, 0);
			}
			
			if (ManualDriverUpButton == 1) {
			xQueueSend(xManualMotorCommandQueue, &MOTOR_COMMAND_Driver_UP, portMAX_DELAY );
			}

			if (ManualDriverDownButton == 1) {
			xQueueSend(xManualMotorCommandQueue, &MOTOR_COMMAND_Driver_DOWN, 0);
			}

			if (ManualPassengerUpButton == 1) {
			xQueueSend(xManualMotorCommandQueue, &MOTOR_COMMAND_Passenger_UP, 0);
			}

			if (ManualPassengerDownButton == 1) {
			xQueueSend(xManualMotorCommandQueue, &MOTOR_COMMAND_Passenger_DOWN, 0);
			}

	}
		
}

void vObstacleHandlerTask(void *pvParameters){
	
	xSemaphoreTake(xBinarySemaphore, 0);
	
	for(;;){
		
		//TickType_t startTime = xTaskGetTickCount();
    //const TickType_t delayTicks = (500/portTICK_RATE_MS);
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		Motor_Control(MOTOR_STOP);
		for(int i =0 ;i<999999;i++);
		Motor_Control(MOTOR_CCW);
		for(int j =0 ;j<999999;j++);
		Motor_Control(MOTOR_STOP);
	}
	
} 


void Port_init() {
    // Enable the GPIO port B clock
    SYSCTL_RCGCGPIO_R |= 0x02;
    while((SYSCTL_PRGPIO_R & 0x02) == 0);
		GPIO_PORTB_LOCK_R = 0x4C4F434B;
    GPIO_PORTB_CR_R = 0xFF;
		GPIO_PORTB_PDR_R |= 0xFF;
    GPIO_PORTB_DIR_R = 0xE0;
    GPIO_PORTB_DEN_R = 0xFF;
	
	  SYSCTL_RCGCGPIO_R |= 0x10;
    while((SYSCTL_PRGPIO_R & 0x10) == 0);
	  GPIO_PORTE_LOCK_R = 0x4C4F434B;
    GPIO_PORTE_CR_R = 0xFF;
		GPIO_PORTE_PDR_R |= 0xFF;
    GPIO_PORTE_DIR_R = 0x00;
    GPIO_PORTE_DEN_R = 0xFF;
		
	
		SYSCTL_RCGCGPIO_R |= 0x08;
    while((SYSCTL_PRGPIO_R & 0x08) == 0);
	  GPIO_PORTD_LOCK_R = 0x4C4F434B;
    GPIO_PORTD_CR_R = 0xFF;
		GPIO_PORTD_PDR_R |= 0xFF;
    GPIO_PORTD_DIR_R = 0x00;
    GPIO_PORTD_DEN_R = 0xFF;
	
} 
void Motor_Control(uint8_t dir)
{
		switch(dir)
		{
								case MOTOR_CCW:
										DIO_WritePin(&GPIO_PORTB_DATA_R, 5, 1); //Motor EN
										DIO_WritePin(&GPIO_PORTB_DATA_R, 7, 0);
										DIO_WritePin(&GPIO_PORTB_DATA_R, 6, 1);
				break;
								case MOTOR_CW:
										DIO_WritePin(&GPIO_PORTB_DATA_R, 5, 1); //Motor EN
										DIO_WritePin(&GPIO_PORTB_DATA_R, 6, 0);
								    DIO_WritePin(&GPIO_PORTB_DATA_R, 7, 1);
										
				break;
								case MOTOR_STOP:
										DIO_WritePin(&GPIO_PORTB_DATA_R, 5, 0); //Motor EN
										DIO_WritePin(&GPIO_PORTB_DATA_R, 7, 0);
										DIO_WritePin(&GPIO_PORTB_DATA_R, 6, 0);
				break;
		}
}
int main()
{

	
	Port_init();
	
	vSemaphoreCreateBinary( xBinarySemaphore );
	
	xAutomaticMotorCommandQueue = xQueueCreate( 1, sizeof( int ) );
	xManualMotorCommandQueue = xQueueCreate( 1, sizeof( int ) );
	
	//GPIO_PORTB_DATA_R = 0x40;

	/* Create you Tasks Here using xTaskCreate()  */
	
	xTaskCreate(vUserInterface, "User Interface Task",240, NULL, 1, NULL);
	xTaskCreate(vAutomaticMotorTask, "Motor Task",240, NULL, 2, NULL);
	xTaskCreate(vManualMotorTask, "Motor Task",240, NULL, 2, NULL);
	xTaskCreate(vObstacleHandlerTask,"Obstacle Handler task",240,NULL,3,NULL);
	
	// Startup of the FreeRTOS scheduler.  The program should block here.  
	vTaskStartScheduler();
	
	// The following line should never be reached.  
	//Failure to allocate enough memory from the heap could be a reason.
	for (;;);
	
}