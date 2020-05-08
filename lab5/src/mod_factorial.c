#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <stdint.h>


struct Segment {

int begin;
int end;

};

void factorial(void *args );
long int res=1; //Результат начинаем всегда с 1 так как удобней будет умножать
int mod=-1; //модуль
int tnum=-1; //количество потоков
int k=-1; //число от которого ищем факториал
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char** argv){

while(true){
int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
       
		if(k<=0){
		   printf("number can't be negative \n");		
		   return -1;
		}
            // error handling
            break;
          case 1:
            tnum = atoi(optarg);
     
		if(tnum<1){
		  printf("amount of threads can't be negative\n");	
		   return -1;
		}
            // error handling
            break;
          case 2:
           mod = atoi(optarg);
		if(mod<1){
		   	printf("module can't be negative \n");
		   return -1;
		}
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }
	

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (k == -1 || mod == -1 || tnum == -1) {
    printf("Usage: %s --k \"num\" --mod \"num\" --tnum \"num\" \n",
           argv[0]);
    return 1;
  }

struct Segment seg[tnum];

 int beginf=0; int endf=0;//Отрезки для потоков вычисляющих факториаль
 
 int step_factorial=0;
if( k>tnum){
step_factorial=(k/tnum);
}else{
 step_factorial=1;
 tnum=k;
}
 
 pthread_t threads[tnum];

 for(int i=0; i<tnum; i++){
    beginf=endf+1;
    endf=beginf+step_factorial-1;
    if((i+1)==tnum)
      endf=k;
    seg[i].begin=beginf;
    seg[i].end=endf;
   
    if (pthread_create(&threads[i], NULL, factorial,(void *)&seg[i])) 
    {      printf("Error: pthread_create failed!\n");
      return 1;
    }

 }
 

for (int i = 0; i < tnum; i+=1) {
    pthread_join(threads[i],NULL);
  }

printf("The factorial %d modulo %d is equal %ld \n", k, mod, res);
return 0;

}

void factorial(void* args){
struct Segment *s = (struct Segment *)args;
pthread_mutex_lock(&mut);
for(int i=s->begin; i<=s->end; i++){
res=res*i;

res=res%mod;
}
pthread_mutex_unlock(&mut);

}

