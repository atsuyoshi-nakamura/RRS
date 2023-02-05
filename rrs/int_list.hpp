//
//  int_list.hpp
//  Integer type list header file
//
#ifndef _INC_INT_LIST
#define _INC_INT_LIST

#include "int_list_entry.hpp"

class int_list {
public:
  int_list_entry *head;
  int_list_entry *tail;
  int_list();
  void append(int_list_entry *in_head,int_list_entry *in_tail);
  void put_to_head(int value);
  void put_to_tail(int value);
};

#endif    //INC_INT_LIST
