//
//  int_list.cpp
//  Integer type list 
//

#include "int_list.hpp"
#include <iostream>

int_list::int_list() {
  head=NULL;
  tail=NULL;
}

void int_list::append(int_list_entry *in_head,int_list_entry *in_tail) {
  if(head==NULL) head=in_head;
  else           tail->next=in_head;
  tail=in_tail;
}

void int_list::put_to_head(int value) {
  head=int_list_entry::create(value,head);
  if(tail==NULL) tail=head;
}

void int_list::put_to_tail(int value) {
  if(tail==NULL) {
    head=int_list_entry::create(value,NULL);
    tail=head;
  }
  else {
    tail->next=int_list_entry::create(value,NULL);
    tail=tail->next;
  }
}
