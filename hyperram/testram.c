#include <defs.h>
#include <stub.h>
#include <delay.h>
#include <serial.h>

#include "hyperram.h"

static void configure_io();

void blink()
{
    reg_gpio_out = 1; // OFF

    delay(10000);

    reg_gpio_out = 0;  // ON

    delay(10000);
}

void main()
{
	int i;

    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oeb = 0;

    configure_io();

    // reset IP
    reg_la0_iena = 0; // input enable off
    reg_la0_oenb = 0xFFFFFFFF; // enable all of bank0 logic analyser outputs (ignore the name, 1 is on, 0 off)
    reg_la0_data = 0xFFFFFFFF;
    blink();
    reg_la0_data = 0xFFFFFFFE;

    // enable wishbone
    reg_wb_enable  = 1;

    // enable uart (RX & TX)
    reg_uart_enable = 1;

    uint8_t led = 0;
    uint8_t cmd;
    uint32_t addr = 0;
    uint32_t value_w = 0x12005634;
    uint32_t value_r = 0;
	while (1) 
    {
        value_r = 0;
        hyperram_mem(addr) = value_w | (addr << 16);
        value_r = hyperram_mem(addr);
        if (value_r != value_w)
        {
            delay(50);
        }
        else
        {
            delay(20);
        }

        addr++;
        if (addr > 0xff)
        {
            addr = 0;
        }
/*        
        uint8_t cmd = serial_getc();
        switch (cmd)
        {
        case '?':
            // loopback
            print("!\n");
            break;
        case 'R':
            // read i.e. "R00:" -> "12345678"
            addr = serial_gethex('\n');
            print_hex(read_from_ram(addr), 8);
            serial_putc('\n');
            break;
        case 'W':
            // write i.e. "W01:deadbeef:" then read back -> "deadbeef"
            addr = serial_gethex(':');
            value = serial_gethex('\n');
            write_to_ram(addr, value);
            print_hex(read_from_ram(addr), 8);
            serial_putc('\n');
            break;
        case 'L':
            // set latencies i.e. "L03030303:" -> "03030303"
            set_ram_latencies_w(serial_gethex('\n'));
            print_hex(read_ram_latencies_w(), 8);
            serial_putc('\n');
            break;
        case 'l':
            // read latencies
            print_hex(read_ram_latencies_w(), 8);
            serial_putc('\n');
            break;
        case 'M':
            // set mgmt core as RW
            set_ram_port_config(RAM_CONF__MGMT_RW__UPRJ_RO);
            print("!\n");
            break;
        case 'U':
            // set uprj as RW
            set_ram_port_config(RAM_CONF__MGMT_RO__UPRJ_RW);
            print("!\n");
            break;
        default:
            break;
        }        
        reg_gpio_out = led; // toggle LED
        led ^= 1;
*/        
    }
}

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

static void configure_io()
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

    reg_mprj_io_5 = GPIO_MODE_MGMT_STD_INPUT_NOPULL;     // UART Rx
    reg_mprj_io_6 = GPIO_MODE_MGMT_STD_OUTPUT;           // UART Tx

    // -------------------------------------------

	reg_mprj_io_8 =  GPIO_MODE_USER_STD_OUTPUT;			    // rstn
	reg_mprj_io_9 =  GPIO_MODE_USER_STD_OUTPUT;			    // csn
	reg_mprj_io_10 =  GPIO_MODE_USER_STD_OUTPUT;		    // clk
	reg_mprj_io_11 =  GPIO_MODE_USER_STD_OUTPUT;		    // clkn
	reg_mprj_io_12 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;		// rwds
	reg_mprj_io_13 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;		// dq0
	reg_mprj_io_14 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_15 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_16 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_17 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_18 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_19 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;
	reg_mprj_io_20 =  GPIO_MODE_USER_STD_BIDIRECTIONAL;		// dq7

    // -------------------------------------------

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}