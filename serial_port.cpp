#include "serial_port.h"

Serial_Port::
Serial_Port()
{
	// Initialize attributes
	debug  = false;
	fd     = -1;
	status = SERIAL_PORT_CLOSED;

	uart_name = (char*)"/dev/ttyAMA0";
	baudrate  = 57600;

	// Start mutex
	int result = pthread_mutex_init(&lock, NULL);
	if ( result != 0 )
	{
		printf("\n mutex init failed\n");
		throw 1;
	}
}

Serial_Port::~Serial_Port()
{
	// destroy mutex
	pthread_mutex_destroy(&lock);
}


// ------------------------------------------------------------------------------
//   Read from Serial
// ------------------------------------------------------------------------------
int Serial_Port::read_message(mavlink_message_t &message)
{
	uint8_t          cp;
	mavlink_status_t status;
	uint8_t          msgReceived = false;

	int result = _read_port(cp);

	if (result > 0)
	{
		// the parsing
		msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);

		// check for dropped packets
		if ( (lastStatus.packet_rx_drop_count != status.packet_rx_drop_count) && debug )
		{
			printf("ERROR: DROPPED %d PACKETS\n", status.packet_rx_drop_count);
			unsigned char v=cp;
			fprintf(stderr,"%02x ", v);
		}
		lastStatus = status;
	}
	else
	{
		fprintf(stderr, "ERROR: Could not read from fd %d\n", fd);
	}



	if(msgReceived && debug)
	{
		// Report info
		printf("Received message from serial with ID #%d (sys:%d|comp:%d):\n", message.msgid, message.sysid, message.compid);

		fprintf(stderr,"Received serial data: ");
		unsigned int i;
		uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

		// check message is write length
		unsigned int messageLength = mavlink_msg_to_send_buffer(buffer, &message);

		// message length error
		if (messageLength > MAVLINK_MAX_PACKET_LEN)
		{
			fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE\n");
		}

		// print out the buffer
		else
		{
			for (i=0; i<messageLength; i++)
			{
				unsigned char v=buffer[i];
				fprintf(stderr,"%02x ", v);
			}
			fprintf(stderr,"\n");
		}
	}

	// Done!
	return msgReceived;
}

// ------------------------------------------------------------------------------
//   Write to Serial
// ------------------------------------------------------------------------------
int Serial_Port::write_message(const mavlink_message_t &message)
{
	char buf[300];

	// Translate message to buffer
	unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &message);

	// Write buffer to serial port, locks port while writing
	int bytesWritten = _write_port(buf,len);

	return bytesWritten;
}


void Serial_Port::open_serial()
{
	printf("OPEN PORT\n");

	fd = _open_port(uart_name);

	if (fd == -1)
	{
		printf("failure, could not open port.\n");
		throw EXIT_FAILURE;
	}

	bool success = _setup_port(baudrate, 8, 1, false, false);

	if (!success)
	{
		printf("failure, could not configure port.\n");
		throw EXIT_FAILURE;
	}
	if (fd <= 0)
	{
		printf("Connection attempt to port %s with %d baud, 8N1 failed, exiting.\n", uart_name, baudrate);
		throw EXIT_FAILURE;
	}

	printf("Connected to %s with %d baud, 8 data bits, no parity, 1 stop bit (8N1)\n", uart_name, baudrate);
	lastStatus.packet_rx_drop_count = 0;

	status = true;

	printf("\n");

	return;

}


// ------------------------------------------------------------------------------
//   Close Serial Port
// ------------------------------------------------------------------------------
void Serial_Port::close_serial()
{
	printf("CLOSE PORT\n");

	int result = close(fd);

	if ( result )
	{
		fprintf(stderr,"WARNING: Error on port close (%i)\n", result );
	}

	status = false;

	printf("\n");

}

void
Serial_Port::
start()
{
	open_serial();
}

void
Serial_Port::
stop()
{
	close_serial();
}


// ------------------------------------------------------------------------------
//   Quit Handler
// ------------------------------------------------------------------------------
void
Serial_Port::
handle_quit( int sig )
{
	try {
		stop();
	}
	catch (int error) {
		fprintf(stderr,"Warning, could not stop serial port\n");
	}
}


// ------------------------------------------------------------------------------
//   Helper Function - Open Serial Port File Descriptor
// ------------------------------------------------------------------------------
// Where the actual port opening happens, returns file descriptor 'fd'
int
Serial_Port::
_open_port(const char* port)
{
	// Open serial port
	// O_RDWR - Read and write
	// O_NOCTTY - Ignore special chars like CTRL-C
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	// Check for Errors
	if (fd == -1)
	{
		/* Could not open the port. */
		return(-1);
	}

	// Finalize
	else
	{
		fcntl(fd, F_SETFL, 0);
	}

	// Done!
	return fd;
}

// ------------------------------------------------------------------------------
//   Helper Function - Setup Serial Port
// ------------------------------------------------------------------------------
// Sets configuration, flags, and baud rate
bool
Serial_Port::
_setup_port(int baud, int data_bits, int stop_bits, bool parity, bool hardware_control)
{
	// Check file descriptor
	if(!isatty(fd))
	{
		fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
		return false;
	}

	// Read file descritor configuration
	struct termios  config;
	if(tcgetattr(fd, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
		return false;
	}

	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
						INLCR | PARMRK | INPCK | ISTRIP | IXON);

	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
						 ONOCR | OFILL | OPOST);

	#ifdef OLCUC
		config.c_oflag &= ~OLCUC;
	#endif

	#ifdef ONOEOT
		config.c_oflag &= ~ONOEOT;
	#endif

	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;

	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 10; // was 0

	if(baud== 57600)
	{
        if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
        {
            fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
            return false;
        }
	}
	else
	{
        fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
        return false;
	}

	// Finally, apply the configuration
	if(tcsetattr(fd, TCSAFLUSH, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

	// Done!
	return true;
}



// ------------------------------------------------------------------------------
//   Read Port with Lock
// ------------------------------------------------------------------------------
int
Serial_Port::
_read_port(uint8_t &cp)
{

	// Lock
	pthread_mutex_lock(&lock);

	int result = read(fd, &cp, 1);

	// Unlock
	pthread_mutex_unlock(&lock);

	return result;
}


// ------------------------------------------------------------------------------
//   Write Port with Lock
// ------------------------------------------------------------------------------
int
Serial_Port::
_write_port(char *buf, unsigned len)
{

	// Lock
	pthread_mutex_lock(&lock);

	// Write packet via serial link
	const int bytesWritten = static_cast<int>(write(fd, buf, len));

	// Wait until all data has been written
	tcdrain(fd);

	// Unlock
	pthread_mutex_unlock(&lock);


	return bytesWritten;
}


