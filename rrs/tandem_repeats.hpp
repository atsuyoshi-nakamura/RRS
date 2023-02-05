//
//  tandem_repeats.hpp
//  Header file of the program for marking the vocabulary of tandem repeats on a given string
//
#ifndef _INC_TANDEM_REPEATS
#define _INC_TANDEM_REPEATS

#include "suffixtree.hpp"

class tandem_repeats {
  static char *backward_str;
  static occurrences *leftmost_occur;
  static suffixtree *stree;
  static suffixtree *bstree;
  static char *create_backword_str(char *str,int len);
  static void algorithm_1a(int len,int h,int h_1,suffixtree *stree,suffixtree *bstree,occurrences *P);
  static void algorithm_1b(int len,int h,int h_1,int h_2,suffixtree *stree,suffixtree *bstree,occurrences *P);
public:
  static suffixtree *get_decorated_stree(char *str,int len,char mode);
  static void alloc_resource(int len);
};

#endif    //TANDEM_REPEATS
