//
//  int_list_entry.cpp
//  Integer type list entry
//

#include "int_list_entry.hpp"
#include <iostream>

int_list_entry *int_list_entry::free_chain=NULL;

int_list_entry::int_list_entry(int val_in,int_list_entry *prev_next) {
  set(val_in,prev_next);
}

void int_list_entry::set(int val_in,int_list_entry *prev_next) {
  val=val_in;
  next=prev_next;
}

int_list_entry *int_list_entry::create(int in_width,int_list_entry *in_next) {
  int_list_entry *new_int_list_entry;
  if(free_chain!=NULL) {
    new_int_list_entry=free_chain;
    free_chain=new_int_list_entry->next;
    new_int_list_entry->set(in_width,in_next);
  }
  else {
    new_int_list_entry=new int_list_entry(in_width,in_next);
  }
  return new_int_list_entry;
}

void int_list_entry::free() {
  next=free_chain;
  free_chain=this;
}

void int_list_entry::free_list(int_list_entry *head,int_list_entry *tail) {
  tail->next=free_chain;
  free_chain=head;
}
