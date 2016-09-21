
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include "serial_port.h"

int main(int argc, char const *argv[])
{
	Serial_Port serial_port;// this is default uart_name /dev/ttyAMA0 , baudrate 57600
    serial_port.start();


	fixed_size *ptr;

  fd = shm_open("share_data", O_CREAT | O_RDWR, 0666);
  if(fd == -1)
		printf("FUCK shm_open error\n");

  size = sizeof(fixed_size);
  ftruncate(fd, size);// open up the enough size of shared memory
  ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd , 0);


	mavlink_message_t	msgrcv;
  int rcv_success=0
	while(1){
    int have_new_data =0;

    rcv_success = serial_port.read_message(msgrcv);// not sure it would wipe out the msgrcv or not
    if(rcv_success)
		{
			printf("sysid :\n compid: \n", msgrcv.sysid,msgrcv.compid);
			switch(msgrcv.msgid)
			{
				/*

				For data recognition








				*/



			}

			ptr->push(drone_data);

		}





	}
	return 0;
}
