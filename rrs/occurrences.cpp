//
//  occurrences.cpp
//  leftmost occurence storage structure
//

#include "occurrences.hpp"
#include <iostream>

occurrences::occurrences(int in_size) {
  size=in_size;
  occurrence_list=new int_list_entry*[size];
  for(int i=0;i<size;i++) occurrence_list[i]=NULL;
}

void occurrences::clear() {
  for(int i=0;i<size;i++) {
    while(pop(i)>=0);
  }
}

void occurrences::push(int start,int len) {
  occurrence_list[start]=int_list_entry::create(len,occurrence_list[start]);
}

int occurrences::pop(int start) {
  int len=-1;
  if(occurrence_list[start]!=NULL) {
    len=occurrence_list[start]->val;
    int_list_entry *del_occurrence=occurrence_list[start];
    occurrence_list[start]=del_occurrence->next;
    del_occurrence->free();
  }
  return len;
}
