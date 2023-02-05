//
//  repeat.hpp
//  primitive tandem repeat entry header file
//
#ifndef _INC_REPEAT
#define _INC_REPEAT   

class repeat {
  static repeat *free_chain;
  void clear(int in_width,repeat *in_next);
public:
  repeat *next;
  int width;
  int length;
  int depth;
  int best_rep_num;
  char *best_rep;
#ifdef APPROXIMATE
  int additional_length[2];
  int best_additional_length;
  int leftmost_pos;
#endif
  repeat *prev_entry;
  repeat *leftmost_repeat;
  repeat(int in_width,repeat *in_next);
  repeat();
  static repeat *create(int in_width,repeat *in_next);
  void free();
};

#endif    //INC_REPEAT
