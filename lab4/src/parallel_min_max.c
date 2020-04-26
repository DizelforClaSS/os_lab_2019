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
#include "find_min_max.h"
#include "utils.h"


volatile pid_t* child_array;
volatile int child_number;
volatile int pnum;

void Htime(int i){

printf("Время вышло! Было создано %d дочерних процессов \n",child_number );

pnum=child_number;

for(int i=0; i<child_number; i++)
{

kill(child_array[i],SIGKILL);

}
free(child_array);

}




int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  
  bool with_files = false;
  int timeout=0;
  pnum = -1;
 
  
while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
				      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
		if(seed<=0){
		   printf("seed can't be negative \n");		
		   return -1;
		}
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
		if(array_size<=0){
		   printf("size can't be negative \n");		
		   return -1;
		}
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
		if(pnum<=0){
		   printf("amount of processes can't be negative\n");		
		   return -1;
		}
            // your code here
            // error handling
            break;
          case 3:
            with_files = true;
            break;
	  case 4:
	    timeout=atoi(optarg);
	    break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case 'f':
        with_files = true;
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

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
  
  int *array = (int*)malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  int f_p[2];
  int sizeforprocess= pnum<=array_size ? (array_size/pnum) : 1;
  
  FILE *file;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  if(!with_files){  
	if(pipe(f_p) < 0){
        	printf("Can\'t create pipe\n");
	      }
  }else{
	
  	file=fopen("tile","wb+");
	if(file==NULL)
		printf("Can\'t create file\n");
	}


child_array=(pid_t*)malloc(pnum*sizeof(pid_t));

  alarm(timeout);
  signal(SIGALRM,&Htime);


  for (int i = 0; i < pnum; i++) {

    child_array[i] = fork();
    if (child_array[i] >= 0) {
      // successful fork
      active_child_processes += 1;
      child_number= active_child_processes;
      if (child_array[i] == 0) {
	// child process
        // parallel somehow
        int Min_Max[2];	  
	int num=0;
	int min = INT_MAX;
    	int max = INT_MIN;
	int  begin=sizeforprocess*(active_child_processes-1);
	int end=begin+sizeforprocess;
	if(active_child_processes==pnum) 
	    end=array_size;
	     
	for(int j=begin;j<end;j++){
		num=array[j];
	        if(num<min) min=num;
	        if(num>max) max=num;
	     }
	Min_Max[0]=min;
	Min_Max[1]=max;
        if (with_files) {

          // use files here
	fwrite(Min_Max,1,sizeof(Min_Max),file);
	fclose(file);
        } else {
             // use pipe here size_t size;
	    close(f_p[0]);
	    write(f_p[1],Min_Max,sizeof(int)*2);
        }

        return 0;
      }

      } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
 
  while (active_child_processes > 0) {
    wait(0);
    active_child_processes -= 1;
  }

if(!with_files)
 close(f_p[1]);
else{
fclose(file);
  file=fopen("tile","rb");
 if(file==NULL)
	printf("Can\'t create file\n");
}
 
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    int mmnumb[2];
	
    if (with_files) {
       //read from files
	fread(mmnumb,1,sizeof(mmnumb),file);
    } else {
      // read from pipes
	read(f_p[0],mmnumb,sizeof(int)*2);
    }
	min=mmnumb[0];
	max=mmnumb[1];	

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }
if (!with_files) 
  close(f_p[0]); 
else
 fclose(file);
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Min: %d\n",min_max.min);
  printf("Max: %d\n",min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
