//
//  edge.hpp
//  Edge header file
//
#ifndef _INC_EDGE
#define _INC_EDGE   

namespace node_space{
class node;
}

namespace edge_space{
class edge {
  static edge *free_chain;
public:
  int range[2];
  node_space::node *from;
  node_space::node *to;
  edge *next;
  int repeat_end_num[2]; // repeat_end_num[0]: found phase II, repeat_end_num[1]: all 
  int repeat_end[2];
  int primitive_end_num;
  int primitive_end[2];
  edge(int rfrom,int rto,node_space::node *from_in,node_space::node *to_in);
  void clear(int rfrom,int rto,node_space::node *from_in,node_space::node *to_in);
  static edge *create(int rfrom,int rto,node_space::node *from_in,node_space::node *to_in);
  void free();
};
}

#endif    //INC_EDGE
