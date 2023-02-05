//
//  suffixtree.hpp
//  Suffix tree header file
//
#ifndef _INC_SUFFIXTREE
#define _INC_SUFFIXTREE

#include "node.hpp"

using namespace node_space;

class suffixtree {
  static unsigned char char_code[256];
  int  len;
  unsigned char *str;
  node *leaf;
  node root;
  node **L;
  int *h_table;
  int *left_most_sign;
  int id_level;
   int *primitive_check_tbl;
  int lca_in_B(int i,int j);
  node *same_run_node(int x,int j);
  int h_value(int x);
  void set_str(char *in_str);
public:
  int comp_num;
  int *comp_idx;
  suffixtree(int len_in,char mode);
  void make(char *str_in,int len_in,char mode);
  void create();
  void create_ZL();
  void print();
  void print_ZL();
  void prep_lca_query();
  int longest_com_subseq_len(int x,int y);
  void algorithm_phaseII(occurrences *P);
  void algorithm_phaseIII();
  void print_all_vocabulary();
  void set_primitive_repeats(DP_table_entry *DP_table);
#ifdef APPROXIMATE
  void copy_str(suffixtree *stree);
#endif
  static int code_num;
  static void put_char(char c);

};

#endif    //SUFFIXTREE
