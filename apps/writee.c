#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<fcntl.h>
#include <unistd.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

char num_to_send[50] = "69";

int main(int argc, char *argv[]){
  printf("Probuje otworzyc urzadzenie!!!\n");
  fflush(stdout);
  int plik=open("/dev/my_dev0", O_RDWR);
  if(plik==-1)
    {
      printf("Nie moge otworzyc urzadzenia!!!\n");
      fflush(stdout);
      exit(1);
    }

  printf("Otworzylem urzadzenie!!!\n");
  fflush(stdout);

  if(argc == 2){
    strcpy(num_to_send,argv[1]);
  }

  write(plik,num_to_send,sizeof(num_to_send));

  printf("Wyslalem liczbe %s\n", num_to_send);
  fflush(stdout);
}
