#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>



int main(int argc, char const *argv[]) {
  int fd;
  int size;
  char *ptr;

  fd = shm_open("share_data", O_RDWR, S_IRUSR | S_IWUSR);
  if(fd == -1)
		printf("FUCK shm_open error\n");
  else
		printf("FUCK What's wrong\n");


  
  size = 50*sizeof(char);

  ftruncate(fd, size);

  ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  while(1)
  printf("Here is the world %s", ptr);

  return 0;
}
