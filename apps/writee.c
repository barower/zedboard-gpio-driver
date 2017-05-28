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

int main()
{
  printf("Probuje otworzyc urzadzenie!!!\n");
  fflush(stdout);
  int plik=open("/dev/my_dev0", O_RDWR);
  if(plik==-1)
    {
      printf("Nie moge otworzyc urzadzenia!!!\n");
      fflush(stdout);
      exit(1);
    }

//  printf("Otworzylem urzadzenie!!!\n");
//  fflush(stdout);

  write(plik,num_to_send,sizeof(num_to_send));

  printf("Wyslalem liczbe %s\n", num_to_send);
  fflush(stdout);
  /*
  while(1)
    {
      int i;
      char czas[20];
      time_t t1=time(NULL);
      struct tm * t=localtime(&t1);
      strftime(czas,20,"%H:%M:%S",t);
      lseek(plik,0,SEEK_SET);
      for(i=0;i<strlen(czas);i++)
	{
	  unsigned short j=(short)czas[i]+15*256;
	  write(plik,&j,sizeof(j));
	}
      sleep(1);
    }  
    */
}
