//
//  int_list_entry.hpp
//  Integer type list entry header file
//
#ifndef _INC_INT_LIST_ENTRY
#define _INC_INT_LIST_ENTRY

class int_list_entry {
  static int_list_entry *free_chain;
  void set(int val_in,int_list_entry *prev_next);
public:
  int_list_entry *next;
  int val;
  int_list_entry(int val_in,int_list_entry *prev_next);
  static int_list_entry *create(int in_width,int_list_entry *in_next);
  void free();
  static void free_list(int_list_entry *head,int_list_entry *tail);
};

#endif    //INC_INT_LIST_ENTRY
