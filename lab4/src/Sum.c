#include "Sum.h"

#include <stdio.h>
#include <stdlib.h>

int Sum(int* array,int begin,int end) {
  int sum = 0;
  // TODO: your code here 
  
  for(int i=begin; i<=end;i++)
  {
      
	sum+=array[i];
        
  }
  return sum;
}
