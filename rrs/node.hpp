//
//  node.hpp
//  Node header file
//
#ifndef _INC_NODE
#define _INC_NODE   

#include "edge.hpp"
#include "occurrences.hpp"
#include "int_list.hpp"
#include "DP_table_entry.hpp"

namespace node_space{
typedef struct {
  int edge_num;
  int *edgelist;
  edge_space::edge **child;
} childinfo;

class node {
  void initclist();
  void suffix_link_walk(int start,int length,unsigned char *str);
  static int char_num;
  static node *free_chain;
public:
  node *link;
  char link_label;
  childinfo *childlist;
  edge_space::edge *parent;
  int id;
  int h;
  node *I;
  int A;
  int depth;
  int leaf_no;
  node();
  static node *create();
  void free();
  void clear();
  static void set_base_num(int cnum);
  void addchild(unsigned char fc,int from,int to,node *new_child);
  void print(int pos,unsigned char *str);
  node *set_id(int *cur_id,int *cur_level,int *h_table,node **L,int cur_depth);
  void set_A(int cur_A);
  int mark_repeat_end(occurrences *P);
  void mark_all_vocabulary(unsigned char *str);
  void print_vocabulary(char *str);
#ifdef APPROXIMATE
  int_list_entry *set_prepeats(DP_table_entry *DP_table,int *primitive_check_tbl,int len,int_list_entry **tail,unsigned char *str);
  int cal_dist(int base_end,int comp_end,int width,unsigned char *str);
#else
  int_list_entry *set_prepeats(DP_table_entry *DP_table,int *primitive_check_tbl,int len,int_list_entry **tail);
#endif
};
}

#endif    //INC_NODE
