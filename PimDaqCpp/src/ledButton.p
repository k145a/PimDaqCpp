// PRUSS program to flash a LED on P9_11 (GPIO0_30) until a button 
// that is connected to P9_13 (GPIO0_31) is pressed. This program 
// was writen by Derek Molloy for the book Exploring BeagleBone
//
// Please note that it is a coincidence that I chose GPIO0_30 and GPIO0_31
// this choice is nothing to do with the PRU registers r30 and r31 but rather
// relates to the randomly chosen P9_11 and P9_13 GPIO numbers

.origin 0                        // start of program in PRU memory
.entrypoint START                // program entry point (for a debugger)

#define GPIO0 0x44e07000         // GPIO Bank 0, See the AM335x TRM
#define GPIO1 0x4804c000         // GPIO Bank 1, Table 2.2 Peripheral Map
#define GPIO2 0x481ac000         // GPIO Bank 2, 
#define GPIO3 0x481ae000         // GPIO Bank 3,
// See the TRM section 25.4.1 (i.e., Chapter 25)
#define GPIO_CLEARDATAOUT 0x190  // for clearing the GPIO registers
#define GPIO_SETDATAOUT   0x194  // for setting the GPIO registers
#define GPIO_DATAIN       0x138  // to read the register data read from GPIO pins
//#define GPIO0_30 1<<30           // P9_11 gpio0[30] Output - bit 30 RS485TX
//#define GPIO0_31 1<<31           // P9_13 gpio0[31] Input - bit 31 RS485RC

#define GPIO0_26 1<<26           // P8_14 gpio0[26] Output - bit 26 VOL OUT PULSE
#define GPIO2_01 1<<1            // P8_18 gpio2[01] Output - bit 1 LED RED
#define GPIO2_00 1<<0	         // P-na  gpio2[00] Output - bit 0 LED GRN
#define GPIO1_28 1<<28           // P9_12 gpio1[28] Output - bit 28 CLUTCH
#define GPIO1_29 1<<29           // P8_26 gpio1[29] Output - bit 29 MOTOR
#define GPIO1_24 1<<24           // usr3  gpio1[24] Output - bit 24 WD SAMPLE 
#define GPIO1_23 1<<23           // usr2  gpio1[23] Output - bit 23 WD WASTE
#define GPIO1_16 1<<16           // P9_15 gpio1[16] Output - bit 16 HYDRO ALRM

#define INS_PER_US   200         // 5ns per instruction
#define INS_PER_DELAY_LOOP 2     // two instructions per delay loop
                                 // set up a 50ms delay
#define DELAY 10 // 50 * 1000 * (INS_PER_US / INS_PER_DELAY_LOOP) 

#define PRU0_R31_VEC_VALID 32    // allows notification of program completion
#define PRU_EVTOUT_0    3        // the event number that is sent back

START:
	// Enable the OCP master port
	LBCO    r0, C4, 4, 4     // load SYSCFG reg into r0 (use c4 const addr)
	CLR     r0, r0, 4        // clear bit 4 (STANDBY_INIT)
	SBCO    r0, C4, 4, 4     // store the modified r0 back at the load addr
MAINLOOP:
	MOV	r1, GPIO1 | GPIO_SETDATAOUT  // load addr for GPIO Set data  r1
	MOV	r2, GPIO1_16     // write GPIO1_16 to r2
	SBBO	r2, r1, 0, 4     // write r2 to the r1 address value - LED ON
	MOV	r0, DELAY        // store the length of the delay in REG0
DELAYON:
	SUB	r0, r0, 1        // Decrement REG0 by 1
	QBNE	DELAYON, r0, 0   // Loop to DELAYON, unless REG0=0
LEDOFF: 
	MOV	r1, GPIO1 | GPIO_CLEARDATAOUT // load addr for GPIO Clear data
	MOV	r2, GPIO1_16     // write GPIO_16 to r2
	SBBO	r2, r1, 0, 4     // write r2 to the r1 address - LED OFF
	MOV	r0, DELAY        // Reset REG0 to the length of the delay
DELAYOFF:
	SUB	r0, r0, 1        // decrement REG0 by 1
	QBNE	DELAYOFF, r0, 0  // Loop to DELAYOFF, unless REG0=0

	//MOV	r5, GPIO1 | GPIO_DATAIN      // load read addr for DATAIN
	//LBBO	r6, r5, 0, 4     // Load the value at r5 into r6
	QBBC	MAINLOOP, r31.t5 // is the button pressed? If not, loop
END:                             // notify the calling app that finished
	MOV	R31.b0, PRU0_R31_VEC_VALID | PRU_EVTOUT_0
	HALT                     // halt the pru program
