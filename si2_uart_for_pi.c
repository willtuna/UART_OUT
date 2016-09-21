#ifndef SI2_UART_FOR_PI
#define SI2_UART_FOR_PI

#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>            //Used for UART
#include <fcntl.h>            //Used for UART
#include <termios.h>        //Used for UART
#include <stdint.h>

#define GET_CHAR_MODE 2
#define AVAILABLE_MODE 1



int uart0_open(){

    static int is_close = 1;
    static int uart0_filestream = -1;

    if (is_close){
        uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);        //Open in non blocking read/write mode
        if (uart0_filestream == -1)
        {
            //ERROR - CAN'T OPEN SERIAL PORT
            printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
            system("PAUSE");
            exit(-1);
        }
        is_close = 0;

        struct termios options;
        tcgetattr(uart0_filestream, &options);// tetch attribute of the uart0_filestream 
        options.c_cflag = B57600 | CS8 | CLOCAL | CREAD;        //<Set baud rate
		// control flag
		//

        options.c_iflag &= ~(IGNPAR | BRKINT | ICRNL |INLCR |PARMRK |INPCK |ISTRIP |IXON);// input flag
		// BRKINT : generate SIGINT on BREAK
		// ICRNL : map Carriage Return to new list 
		// IGNPAR : ignore characters with paity error
		// INLCR map NL to CR on inut
		// INPCK : enable pairity checking
		// ISTRIP : strip 8 bit off input charaters
		// IXON : enable star/stop  output flow control

        options.c_oflag &= ~(OCRNL | ONLCR | ONLRET |ONOCR |OFILL |OPOST);// output flag
        options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN| ISIG );// local flag
		options.c_cflag &= ~(CSIZE | PARENB);
		options.c_cflag |= CS8;
        tcflush(uart0_filestream, TCIFLUSH);
        tcsetattr(uart0_filestream, TCSANOW, &options);
    }

    return uart0_filestream;
}

int uart0_send(uint8_t *buf, uint16_t len){

    int uart0_filestream = -1;
    int i, count;
    unsigned char tx_buffer[264];
    unsigned char *p_tx_buffer;

    uart0_filestream = uart0_open();
    p_tx_buffer = &tx_buffer[0];

    for (i = 0; i < len; ++i){
        *p_tx_buffer++ = buf[i];
    }

    if (uart0_filestream != -1)
    {
        count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));        //Filestream, bytes to write, number of bytes to write
        if (count < 0)
        {
            printf("UART TX error\n");
        }
    }

	return uart0_filestream;
}





// Already Defined above
//#define GET_CHAR_MODE 2
//#define AVAILABLE_MODE 1
char my_read(int mode ,int uart0_filestream){
		
		static char rx_buffer[256]={'\0',0};// initialized rx_buffer
		static char *read_ptr = rx_buffer;
		static int rx_length;

		int current_pos = read_ptr - rx_buffer +1 ;
		if(current_pos == rx_length){ // need to refresh the buffer !!!
				rx_length =	read(uart0_filestream, (void*)rx_buffer, 255); // also refresh rx_length
				read_ptr = rx_buffer;// refresh read_ptr
		}// if no data rx_length may be 0 or smaller than 0
		
		
		if(mode == AVAILABLE_MODE)
				return (char)(rx_length > 0) ; // means still could get char 
		else if(mode == GET_CHAR_MODE)
				return *read_ptr++;
		else
				printf("You Enter the wrong mode ~~ run time error");
}



int uart0_char_available()
{
		return (int)my_read(AVAILABLE_MODE,uart0_open());
}

char uart0_get_char()
{
		return my_read(GET_CHAR_MODE,uart0_open());
}




#endif
