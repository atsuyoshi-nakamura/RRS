//
//  occurrences.hpp
//  leftmost occurence storage structure header file
//
#ifndef _INC_OCCURRENCES
#define _INC_OCCURRENCES

#include "int_list_entry.hpp"

class occurrences {
  int size;
public:
  int_list_entry **occurrence_list;
  occurrences(int size);
  void push(int start,int size);
  int pop(int start);
  void clear();
};

#endif    //INC_OCCURRENCES
