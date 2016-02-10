
#include "device.h"
#include "FreeRTOS.h"
#include "port.h"
#include "task.h"
#include "queue.h"

#define FOREVER (1 == 1)

/* Blink task parameters */
#define STACK_SIZE 240

static void SendTask(void *parm);
static void ReceiveTask1(void *parm);
static void ReceiveTask2(void *parm);
static void ReceiveTask3(void *parm);

/* Used to store handle for the three queues */
xQueueHandle xQueue1;
xQueueHandle xQueue2;
xQueueHandle xQueue3;

/*
 * Installs the RTOS interrupt handlers and starts the peripherals.
 */
static void prvHardwareSetup( void );
/*---------------------------------------------------------------------------*/


void vApplicationMallocFailedHook(void)
{
    /* The heap space has been execeeded. */
}

void vApplicationStackOverflowHook(void)
{
    /* The stack space has been overflowed */
}

int main( void )
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
	prvHardwareSetup();
    xQueue1 = xQueueCreate(3, sizeof(int));
    xQueue2 = xQueueCreate(3, sizeof(int));
    xQueue3 = xQueueCreate(3, sizeof(int));
    
    /* Create Task to send to queue */
    ( void ) xTaskCreate( SendTask, "Send", STACK_SIZE, NULL, 2, NULL );
        
    /* Create three tasks to receive from queue */
    ( void ) xTaskCreate( ReceiveTask1, "Receive1", STACK_SIZE, NULL, 1, NULL );
    ( void ) xTaskCreate( ReceiveTask2, "Receive2", STACK_SIZE, NULL, 1, NULL );
    ( void ) xTaskCreate( ReceiveTask3, "Receive3", STACK_SIZE, NULL, 1, NULL );
    
    /* Start the OS */
    vTaskStartScheduler();
   
    /*NOTREACHED*/
    for(;;);
}

void prvHardwareSetup( void )
{
    /* Port layer functions that need to be copied into the vector table. */
    extern void xPortPendSVHandler( void );
    extern void xPortSysTickHandler( void );
    extern void vPortSVCHandler( void );
    extern cyisraddress CyRamVectors[];

	/* Install the OS Interrupt Handlers. */
	CyRamVectors[ 11 ] = ( cyisraddress ) vPortSVCHandler;
	CyRamVectors[ 14 ] = ( cyisraddress ) xPortPendSVHandler;
	CyRamVectors[ 15 ] = ( cyisraddress ) xPortSysTickHandler;

	/* Start-up the peripherals. */
}
/*---------------------------------------------------------------------------*/

static void SendTask(void *parm)
{
    (void) parm;
    int Switch1_Send;
    int Switch2_Send;
    int Switch3_Send;

    while( FOREVER )
    {
        /* Read switches */
        Switch1_Send = !Switch1_Pin_Read();
        Switch2_Send = !Switch2_Pin_Read();
        Switch3_Send = !Switch3_Pin_Read();
        
        /* Send to queues.
        Third parameter is not 0 because the queue is expected to become full */
        xQueueSendToBack(xQueue1, &Switch1_Send, 100);
        xQueueSendToBack(xQueue2, &Switch2_Send, 100);
        xQueueSendToBack(xQueue3, &Switch3_Send, 100);
    }
}

static void ReceiveTask1(void *parm)
{
    (void) parm;
    int Switch1_Receive;

    while( FOREVER )
    {
        /* Receive from xQueue1 
        Third parameter is 0 because queue is not expected to empty */
        xQueueReceive(xQueue1, &Switch1_Receive, 0); 
        Blue_LED_Pin_Write( Switch1_Receive );
        
        taskYIELD();
    }
}

static void ReceiveTask2(void *parm)
{
    (void) parm;
    int Switch2_Receive;

    while( FOREVER )
    {
        /* Receive from xQueue2 
        Third parameter is 0 because queue is not expected to empty */
        xQueueReceive(xQueue2, &Switch2_Receive, 0); 
        Blue_LED_Pin_Write( Switch2_Receive );
        
        taskYIELD();
    }
}

static void ReceiveTask3(void *parm)
{
    (void) parm;
    int Switch3_Receive;

    while( FOREVER )
    {
        /* Receive from xQueue3 
        Third parameter is 0 because queue is not expected to empty */
        xQueueReceive(xQueue3, &Switch3_Receive, 0); 
        Blue_LED_Pin_Write( Switch3_Receive );
        
        taskYIELD();
    }
}
