//
//  repeat.cpp
//  primitive tandem repeat entry
//

#include "repeat.hpp"
#include <iostream>

repeat *repeat::free_chain=NULL;

repeat::repeat(int in_width,repeat *in_next) {
  clear(in_width,in_next);
}

void repeat::clear(int in_width,repeat *in_next) {
  width=in_width;
  next=in_next;
  prev_entry=NULL;
  best_rep=NULL;
  depth=0;
  length=0;
#ifdef APPROXIMATE
  additional_length[0]=0;
  additional_length[1]=0;
  best_additional_length=0;
#endif
}

repeat::repeat() {
}

repeat *repeat::create(int in_width,repeat *in_next) {
  repeat *new_repeat;
  if(free_chain!=NULL) {
    new_repeat=free_chain;
    free_chain=new_repeat->next;
    new_repeat->clear(in_width,in_next);
  }
  else {
    new_repeat=new repeat(in_width,in_next);
  }
  return new_repeat;
}

void repeat::free() {
  next=free_chain;
  free_chain=this;
}
