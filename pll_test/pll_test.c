#include <defs.h>
#include <stub.h>
#include <delay.h>
#include <pll.h>

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

void configure_io()
{

//  ======= Useful GPIO mode values =============

//      GPIO_MODE_MGMT_STD_INPUT_NOPULL
//      GPIO_MODE_MGMT_STD_INPUT_PULLDOWN
//      GPIO_MODE_MGMT_STD_INPUT_PULLUP
//      GPIO_MODE_MGMT_STD_OUTPUT
//      GPIO_MODE_MGMT_STD_BIDIRECTIONAL
//      GPIO_MODE_MGMT_STD_ANALOG

//      GPIO_MODE_USER_STD_INPUT_NOPULL
//      GPIO_MODE_USER_STD_INPUT_PULLDOWN
//      GPIO_MODE_USER_STD_INPUT_PULLUP
//      GPIO_MODE_USER_STD_OUTPUT
//      GPIO_MODE_USER_STD_BIDIRECTIONAL
//      GPIO_MODE_USER_STD_ANALOG


//  ======= set each IO to the desired configuration =============

    //  GPIO 0 is turned off to prevent toggling the debug pin; For debug, make this an output and
    //  drive it externally to ground.
    
    reg_mprj_io_0 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    
    // Changing configuration for IO[1-4] will interfere with programming flash. if you change them,
    // You may need to hold reset while powering up the board and initiating flash to keep the process
    // configuring these IO from their default values.

    reg_mprj_io_1 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_2 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_3 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;
    reg_mprj_io_4 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;

    // -------------------------------------------
    reg_mprj_io_8 =   GPIO_MODE_USER_STD_INPUT_NOPULL; // select
    reg_mprj_io_9 =   GPIO_MODE_USER_STD_INPUT_NOPULL; // restart
    reg_mprj_io_10 =  GPIO_MODE_USER_STD_INPUT_NOPULL; // rotary a
    reg_mprj_io_11 =  GPIO_MODE_USER_STD_INPUT_NOPULL; // rotary b 

    reg_mprj_io_12 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg 
    reg_mprj_io_13 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_14 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_15 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_16 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_17 =  GPIO_MODE_USER_STD_OUTPUT;
    reg_mprj_io_18 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg
    reg_mprj_io_19 =  GPIO_MODE_USER_STD_OUTPUT; // seven seg digit
    reg_mprj_io_20 =  GPIO_MODE_USER_STD_OUTPUT; // led flag
    reg_mprj_io_21 =  GPIO_MODE_USER_STD_OUTPUT; // sync

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}

void main()
{
	int i, j, k;

    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oeb = 1;

    configure_io();

    //reg_uart_enable = 1;

    // Configure All LA probes as inputs to the cpu
	//reg_la0_oenb = reg_la0_iena = 0x00000000;    // [31:0]
	//reg_la1_oenb = reg_la1_iena = 0x00000000;    // [63:32]
	////reg_la2_oenb = reg_la2_iena = 0x00000000;    // [95:64]
	//reg_la3_oenb = reg_la3_iena = 0x00000000;    // [127:96]

	// write data to la output
    //	reg_la0_data = 0x00;
    //	reg_la1_data = 0x00;
    //	reg_la2_data = 0x00;
    //	reg_la3_data = 0x00;

    // read data from la input
    //	data0 = reg_la0_data;
    //	data1 = reg_la1_data;
    //	data2 = reg_la2_data;
    //	data3 = reg_la3_data;

    pll_enable(0);
    pll_configure(0x3f, 6, 1);
    pll_change_clock(1);

	while (1) 
    {
        reg_gpio_out = 1; // OFF
		delay(2000000);
        reg_gpio_out = 0; // ON
		delay(4000000);
    }

}
