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

#define PRU_NUM	0   // using PRU0 for these examples

int main33 (void)
{
   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
      exit(EXIT_FAILURE);
   }
	//RK set pin direction to output
	int GPIOPin = 48;
	FILE *myOutputHandle = NULL;
	char setValue[4], GPIOString[4], GPIOValue[64], GPIODirection[64];
	sprintf(GPIOString, "%d", GPIOPin);
	sprintf(GPIOValue, "/sys/class/gpio/gpio%d/value", GPIOPin);
	sprintf(GPIODirection, "/sys/class/gpio/gpio%d/direction", GPIOPin);
	// Export the pin
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
   prussdrv_exec_program (PRU_NUM, "./ledButton.bin");

   // Wait for event completion from PRU, returns the PRU_EVTOUT_0 number
   int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
   printf("EBB PRU program completed, event number %d.\n", n);

   // Disable PRU and close memory mappings
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return EXIT_SUCCESS;
}
