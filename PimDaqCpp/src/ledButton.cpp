/** Program to load a PRU program that flashes an LED until a button is
*   pressed. By Derek Molloy, for the book Exploring BeagleBone
*   based on the example code at:
*   http://processors.wiki.ti.com/index.php/PRU_Linux_Application_Loader_API_Guide
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "GPIO.h"

#define PRU_NUM	0   // using PRU0 for these examples

/*** test definitions ****/
#define HWREG(x) (*((volatile unsigned int * ) (x)))

using namespace exploringBB;
using namespace std;


int x1, iret1, iret2;

// function run in thread while waiting for input
void *threadInputDetect(void *value){
	 // Wait for event completion from PRU, returns the PRU_EVTOUT_0 number
	   int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
	   prussdrv_pru_disable(PRU_NUM);
	   prussdrv_exit ();
	   printf("EBB PRU program completed, event number %d.\n", n);
	   return 0;
}

/*******************************************/
int main (void)
{
	x1 = 0;
   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
    //  exit(EXIT_FAILURE);
   }

   	 GPIO outGPIO(49), inGPIO(115);
     inGPIO.setDirection(GPIO::INPUT);    //button is an input
     outGPIO.setDirection(GPIO::OUTPUT);  //LED is an output
     inGPIO.setEdgeType(GPIO::RISING);    //wait for rising edge
     outGPIO.streamOpen();          //fast write, ready file
     outGPIO.streamWrite(GPIO::LOW);      //turn the LED off
     cout << "Press the button:" << endl;
     inGPIO.waitForEdge();          //will wait forever
     outGPIO.streamWrite(GPIO::HIGH);     //button pressed, light LED
     outGPIO.streamClose();         //close the output stream

     pthread_t thread; //handle to thread
     const char *message1 = "thread 1";

	//RK set pin direction to output
	int GPIOPin = 48;
	FILE *myOutputHandle = NULL;
	char setValue[4], GPIOString[4], GPIOValue[64], GPIODirection[64];
	sprintf(GPIOString, "%d", GPIOPin);
	sprintf(GPIOValue, "/sys/class/gpio/gpio%d/value", GPIOPin);
	sprintf(GPIODirection, "/sys/class/gpio/gpio%d/direction", GPIOPin);
	//rk Export the pin
	if ((myOutputHandle = fopen("/sys/class/gpio/export", "ab")) == NULL)
	{
	printf("Unable to export GPIO Pin\n");
	return 1;
	}
	strcpy(setValue, GPIOString);
	fwrite(&setValue, sizeof(char),2, myOutputHandle);
	fclose(myOutputHandle);

	//set the direction of the output
	if ((myOutputHandle = fopen(GPIODirection, "rb+")) == NULL)
	{
	printf("Unable to open Direction handle\n");
	return 1;
	}
	strcpy(setValue, "out");
	fwrite(&setValue, sizeof(char), 3, myOutputHandle);
	fclose(myOutputHandle);

   // Initialize structure used by prussdrv_pruintc_intc
   // PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   // Allocate and initialize memory
   prussdrv_init();
   prussdrv_open(PRU_EVTOUT_0);

   // Map PRU's interrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);
   
   // Load and execute the PRU program on the PRU
   prussdrv_exec_program (PRU_NUM, "../home/debian/gpioLEDButton/gpioLEDButton/ledButton.bin");

   // Wait for event completion from PRU, returns the PRU_EVTOUT_0 number
  // int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
  // printf("EBB PRU program completed, event number %d.\n", n);
   //create thread, pass ref addr of function and data
     iret1 = pthread_create(&thread, NULL, threadInputDetect,(void*) message1);
     if(iret1)
     {
  	   printf( "failed to create thread");
  	   return 1;
     }
   // Disable PRU and close memory mappings
   //prussdrv_pru_disable(PRU_NUM);
  // prussdrv_exit ();
   return EXIT_SUCCESS;
}
