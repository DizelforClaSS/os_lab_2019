#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>


int main(int argc, char** argv){

int j=0;
int active_child_processes=0;
int k=1;
for(int i=0;i<10;i++){
   pid_t child_pid = fork();
    if (child_pid >= 0)
      active_child_processes += 1;

    if (child_pid == 0) {
	 while(j!=100){
		j+=1;
	  }
        printf("Процесс %d завершил выполнение \n",getpid());
	return 0;
     }
}

printf("Я-родитель, ушел спать");
sleep(50);

printf("The End! \n");

return 0;

}
