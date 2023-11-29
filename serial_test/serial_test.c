#include <defs.h>
#include <stub.h>
#include <serial.h>
#include <delay.h>

static void configure_io();

void blink()
{
    reg_gpio_out = 1; // OFF

    delay(2000000);

    reg_gpio_out = 0;  // ON

    delay(4000000);
}

void main()
{
	int i, j, k;

    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oeb = 0;

    configure_io();

    // enable uart (RX & TX)
    reg_uart_enable = 1;

    /*
    see https://github.com/efabless/caravel_SI_testing/blob/chipignite-dev3-C-tests/caravel_board/firmware_vex/mpw8_tests/uart_recieve_back/uart_recieve_back.c
    and https://github.com/efabless/caravel_SI_testing/blob/chipignite-dev3-C-tests/caravel_board/firmware_vex/mpw8_tests/uart_reception/uart_reception.c
    for examples of serial tests
    */

    int count = 0;
	while (1) 
    {
        char c = serial_getc();
        count ++;
        switch(c) {
            case 'a':
                print("Hello World !!\n");
                break;
            case 'b':
                print_hex(count, 4);
                print("\n");
                break;
            case 'c':
                print("blinking...\n");
                blink();
                break;
            default:
                print("a: hello\nb: count\nc: blink\n");
                break;
        }     
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

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}
