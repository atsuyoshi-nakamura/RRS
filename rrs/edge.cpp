//
//  edge.cpp
//  Edge
//
#include <iostream>
#include "edge.hpp"
#include "node.hpp"

using namespace edge_space;
using namespace node_space;

edge *edge::free_chain=NULL;

edge::edge(int rfrom,int rto,node *from_in,node *to_in) {
  clear(rfrom,rto,from_in,to_in);
}

void edge::clear(int rfrom,int rto,node *from_in,node *to_in) {
  range[0]=rfrom;
  range[1]=rto;
  from=from_in;
  to=to_in;
  repeat_end_num[0]=0;
  repeat_end_num[1]=0;
  to->parent=this;
}

edge *edge::create(int rfrom,int rto,node *from_in,node *to_in) {
  edge *new_edge;
  if(free_chain!=NULL) {
    new_edge=free_chain;
    free_chain=new_edge->next;
    new_edge->clear(rfrom,rto,from_in,to_in);
  }
  else {
    new_edge=new edge(rfrom,rto,from_in,to_in);
  }
  return new_edge;
}

void edge::free() {
  next=free_chain;
  free_chain=this;
}

