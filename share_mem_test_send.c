#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>



int main(int argc, char const *argv[]) {
  int fd;
  int size;
  char string[50] = "Hello Testing the shared memory";
  char *ptr,location;

  fd = shm_open("share_data", O_CREAT | O_RDWR, 0666);
  if(fd == -1)
		printf("FUCK shm_open error\n");
  else
		printf("FUCK What's wrong\n");

  size = sizeof(string);

  ftruncate(fd, size);

  ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd , 0);
  location = ptr;

  int v = 0;
  while(1)
  {

    for(int i=0; i<50 ; ++i){
    scanf("%c",ptr);
	if(*ptr == '\\')
		break;

    ptr++;
    }


	ptr = location;

  }
  printf("Where am I ??");

  return 0;
}
