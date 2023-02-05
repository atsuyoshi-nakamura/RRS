//
//  timer.cpp
//  Timer
//

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "timer.hpp"

using namespace std;

void timer::print_cpu_time()
{
  struct rusage usage;
  int sec;
  int min;
  int hour;
  int day;

  getrusage(RUSAGE_SELF,&usage);
  sec=usage.ru_utime.tv_sec+usage.ru_utime.tv_usec/1000000;

  min=sec/60;
  sec=sec%60;
  hour=min/60;
  min=min%60;
  day=hour/24;
  hour=hour%24;
  printf("cpu_time = %d days %d hours %d minutes %d.%06d seconds\n",day,hour,min,sec,
	 (int)(usage.ru_utime.tv_usec)%1000000);
  return;
}
