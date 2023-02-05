//
//  node.cpp
//  Node
//

#include <limits.h>
#include <iostream>
#include <stdlib.h>
#include "node.hpp"

using namespace node_space;
using namespace edge_space;
using namespace std;

int node::char_num;
node *node::free_chain=NULL;

node::node() {
  clear();
  childlist=NULL;
}

node *node::create()
{
  node *new_node;
  if(free_chain!=NULL) {
    new_node=free_chain;
    free_chain=new_node->link;
    new_node->clear();
  }
  else {
    new_node=new node();
  }
  return new_node;
}

void node::set_base_num(int cnum) {
  char_num=cnum;
}

void node::clear() {
  parent=NULL;
  link=NULL;
}

void node::initclist() {
  childlist=new childinfo();
  childlist->edgelist=new int[char_num];
  childlist->child=(edge**)malloc(sizeof(edge*)*char_num);
  for(int i=0;i<char_num;i++) childlist->child[i]=NULL;
  childlist->edge_num=0;
}

void node::addchild(unsigned char fc,int from,int to,node *new_child) {
  if(childlist==NULL) initclist();
  childlist->child[fc]=edge::create(from,to,this,new_child);
  childlist->edgelist[childlist->edge_num++]=fc;
}

void node::print(int pos,unsigned char *str) {
  if(childlist!=NULL) {
    cout<<"--+--";
    for(int i=0;i<childlist->edge_num;i++) {
      if(i>0) {
        for(int i=0;i<pos;i++) cout<<" ";
        cout<<"  |"<<endl;
        for(int i=0;i<pos;i++) cout<<" ";
        cout<<"  +--";
      }
      edge *cur_edge=childlist->child[childlist->edgelist[i]];
      for(int i=cur_edge->range[0];i<cur_edge->range[1];i++) cout<<((char *)str)[i];
      cur_edge->to->print(pos+cur_edge->range[1]-cur_edge->range[0]+5,str);
    }
  }
  else {
    cout<<"--*"<<endl;
  }
}

node *node::set_id(int *cur_id,int *cur_level,int *h_table,node **L,int cur_depth) {
  depth=cur_depth;
  int i=1<<(*cur_level);
  if(*cur_id>i) {
    i=(*cur_id)%i;
    if(i==0) h=(++(*cur_level)+1);
    else h=h_table[i];
  }
  else {
    h=*cur_level+1;
  }
  h_table[*cur_id]=h;
  id=(*cur_id)++;
  I=this;
  if(childlist!=NULL) {
    for(int i=0;i<childlist->edge_num;i++) {
      edge *cur_edge=childlist->child[childlist->edgelist[i]];
      node *max_h_node=cur_edge->to->set_id(cur_id,cur_level,h_table,L,cur_depth+cur_edge->range[1]-cur_edge->range[0]);
      if(I->h<max_h_node->h) I=max_h_node;
    }
  }
  L[I->id]=this;
  return I;
}

void node::set_A(int cur_A) {
  A=cur_A | (1<<(I->h-1));
  if(childlist!=NULL) {
    for(int i=0;i<childlist->edge_num;i++) {
      childlist->child[childlist->edgelist[i]]->to->set_A(A);
    }
  }
}

int node::mark_repeat_end(occurrences *P) {
  int smallest_lno;
  if(childlist!=NULL) {
    smallest_lno=INT_MAX;
    for(int i=0;i<childlist->edge_num;i++) {
      int cur_lno=childlist->child[childlist->edgelist[i]]->to->mark_repeat_end(P);
      if(cur_lno<smallest_lno) {
        smallest_lno=cur_lno;
      }
    }
  }
  else {
    smallest_lno=leaf_no;
  }

  edge *edge_from_parent=parent;
  if(edge_from_parent!=NULL) {
    while((P->occurrence_list[smallest_lno]!=NULL)&&(P->occurrence_list[smallest_lno]->val>edge_from_parent->from->depth)) {
      edge_from_parent->repeat_end[(edge_from_parent->repeat_end_num[0])++]=P->occurrence_list[smallest_lno]->val-edge_from_parent->from->depth;
      P->pop(smallest_lno);
    }
    edge_from_parent->repeat_end_num[1]=edge_from_parent->repeat_end_num[0];
  }
  return smallest_lno;
}

void node::suffix_link_walk(int start,int length,unsigned char *str) {
  node *cur_node;
  unsigned char leftmost_char;
  if(link==NULL) {
    cur_node=this;
    start++;
    length--;
    leftmost_char=str[start];
  }
  else {
    cur_node=link;
    leftmost_char=link_label;
  }
  edge *cur_edge=cur_node->childlist->child[str[start]];
  int cur_edge_len;
  while((cur_edge_len=cur_edge->range[1]-cur_edge->range[0])<length) {
    cur_node=cur_edge->to;
    start+=cur_edge_len;
    length-=cur_edge_len;
    if((cur_edge=cur_node->childlist->child[str[start]])==NULL) return;
  }
  if(cur_edge->range[1]-cur_edge->range[0]==length) {
    cur_node=cur_edge->to;
    if((cur_edge=cur_node->childlist->child[leftmost_char])==NULL) return;
    length=0;
    start=cur_edge->range[0];
  }
  length++;
  if(str[start+length-1]==leftmost_char) {
    int i;
    for(i=0;i<cur_edge->repeat_end_num[0];i++) {
      if(cur_edge->repeat_end[i]==length) break;
    }
    if(i>=cur_edge->repeat_end_num[0]) {
      cur_edge->repeat_end[(cur_edge->repeat_end_num[1])++]=length;
      cur_node->suffix_link_walk(cur_edge->range[0],length,str);
    }
  }
  return;
}

void node::mark_all_vocabulary(unsigned char *str) {
  if(childlist!=NULL) {
    for(int i=0;i<childlist->edge_num;i++) {
      edge *cur_edge=childlist->child[childlist->edgelist[i]];
      cur_edge->to->mark_all_vocabulary(str);
      for(int j=0;j<cur_edge->repeat_end_num[0];j++) {
        suffix_link_walk(cur_edge->range[0],cur_edge->repeat_end[j],str);
      }
     }
  }
}

void node::print_vocabulary(char *str) {
  if(childlist!=NULL) {
    for(int i=0;i<childlist->edge_num;i++) {
      edge *cur_edge=childlist->child[childlist->edgelist[i]];
      cur_edge->to->print_vocabulary(str);
      for(int j=0;j<cur_edge->repeat_end_num[1];j++) {
        int start=cur_edge->range[0]-depth;
        for(int k=0;k<depth+cur_edge->repeat_end[j];k++) {
          cout << str[start+k];
        }
        cout << endl;
      }
    }
  }
}

int_list_entry *node::set_prepeats(DP_table_entry *DP_table,int *primitive_check_tbl,int len,int_list_entry **tail
#ifdef APPROXIMATE
     ,unsigned char *str)
#else
     )
#endif
{
  int_list end_list;
  end_list.head=NULL;
  end_list.tail=NULL;
  if(childlist!=NULL) {
    for(int i=0;i<childlist->edge_num;i++) {
      edge *cur_edge=childlist->child[childlist->edgelist[i]];
      cur_edge->primitive_end_num=0;
      for(int j=0;j<cur_edge->repeat_end_num[1];j++) {
        int length=depth+cur_edge->repeat_end[j];
        int prim_len;
        if((prim_len=primitive_check_tbl[length])==0) {
          cur_edge->primitive_end[(cur_edge->primitive_end_num)++]=length/2;
          primitive_check_tbl[length*2]=length;
        }
        else {
          primitive_check_tbl[length+prim_len]=prim_len;
        }
      }
      int_list_entry *child_tail;
      int_list_entry *child_head=cur_edge->to->set_prepeats(DP_table,primitive_check_tbl,len,&child_tail
#ifdef APPROXIMATE
	 ,str
#endif
							    );
      end_list.append(child_head,child_tail);
      for(int j=0;j<cur_edge->repeat_end_num[1];j++) {
        int length=depth+cur_edge->repeat_end[j];
        int prim_len;
        if((prim_len=primitive_check_tbl[length])==0) {
          primitive_check_tbl[length*2]=0;
        }
        else {
          primitive_check_tbl[length+prim_len]=0;
        }
      }      
    }
  }
  else {
    end_list.put_to_head(len-leaf_no-1);
  }
  int_list_entry *cur_entry;
  edge *prev_edge=parent;
  if((prev_edge!=NULL)&&(prev_edge->primitive_end_num>0)) {
    for(int i=0;i<prev_edge->primitive_end_num;i++) {
      int cur_width=prev_edge->primitive_end[i];
      int first_end=len;
      for(cur_entry=end_list.head;cur_entry!=NULL;cur_entry=cur_entry->next) {
        DP_table[cur_entry->val].entry_list=repeat::create(cur_width,DP_table[cur_entry->val].entry_list);
        if(first_end>cur_entry->val) first_end=cur_entry->val;
      }
      for(cur_entry=end_list.head;cur_entry!=NULL;cur_entry=cur_entry->next) {
        repeat *prev_entry=DP_table[cur_entry->val-cur_width].entry_list;
        if(prev_entry!=NULL) {
#ifdef APPROXIMATE
          if((prev_entry->width==cur_width)&&(prev_entry->additional_length[1]==0)) {
#else
          if(prev_entry->width==cur_width) {
#endif
            DP_table[cur_entry->val].entry_list->prev_entry=prev_entry;
          }
        }
        DP_table[cur_entry->val].entry_list->leftmost_repeat=DP_table[first_end].entry_list;
#ifdef APPROXIMATE
        DP_table[cur_entry->val].entry_list->leftmost_pos=first_end-2*cur_width+1;
#endif      
      }      
#ifdef APPROXIMATE
      for(cur_entry=end_list.head;cur_entry!=NULL;cur_entry=cur_entry->next) {
        int dist;
        int prev_dist=DP_table[first_end].entry_list->additional_length[0];
        for(int end_pos=cur_entry->val-cur_width;
            end_pos>=cur_width*2-1;
            end_pos=end_pos-cur_width) {
          repeat *prev_entry=DP_table[end_pos].entry_list;
          if((prev_entry!=NULL)&&(prev_entry->leftmost_repeat==DP_table[first_end].entry_list)) {
            DP_table[end_pos+cur_width].entry_list->prev_entry=DP_table[end_pos].entry_list;
            break;
          }
          if((dist=cal_dist(first_end,end_pos-cur_width,cur_width,str))<0) break;
          DP_table[end_pos].entry_list=repeat::create(cur_width,prev_entry);
          DP_table[end_pos+cur_width].entry_list->prev_entry=DP_table[end_pos].entry_list;
          DP_table[end_pos].entry_list->leftmost_repeat=DP_table[first_end].entry_list;
          DP_table[end_pos].entry_list->leftmost_pos=first_end-2*cur_width+1;
          DP_table[end_pos].entry_list->additional_length[0]=dist;
          DP_table[end_pos].entry_list->additional_length[1]=prev_dist;
          prev_dist=dist;
        }
        prev_dist=DP_table[first_end].entry_list->additional_length[1];
        for(int end_pos=cur_entry->val+cur_width;
            end_pos<len;
            end_pos=end_pos+cur_width) {
          repeat *next_entry=DP_table[end_pos].entry_list;
          if((next_entry!=NULL)&&(next_entry->leftmost_repeat==DP_table[first_end].entry_list)) {
            DP_table[end_pos].entry_list->prev_entry=DP_table[end_pos-cur_width].entry_list;
            break;
          }
          if((dist=cal_dist(first_end,end_pos,cur_width,str))<0) break;
          DP_table[end_pos].entry_list=repeat::create(cur_width,next_entry);
          DP_table[end_pos].entry_list->prev_entry=DP_table[end_pos-cur_width].entry_list;
          DP_table[end_pos].entry_list->leftmost_repeat=DP_table[first_end].entry_list;
          DP_table[end_pos].entry_list->leftmost_pos=first_end-2*cur_width+1;
          DP_table[end_pos].entry_list->additional_length[1]=dist;
          DP_table[end_pos].entry_list->additional_length[0]=prev_dist;
          prev_dist=dist;
        }
      }
#endif      
    }
  }
  *tail=end_list.tail;
  return end_list.head;
}

#ifdef APPROXIMATE
int node::cal_dist(int base_end,int comp_end,int width,unsigned char *str)
{
  int dist=0;
  for(int j=0;j<width;j++) {
    if(str[base_end-j]!=str[comp_end-j]) {
      dist+=2;
    }
  }
  if(dist>width) return -1;
  else return dist;
}
#endif

void node::free() {
  if(childlist!=NULL) { //not leaf
    for(int i=0;i<childlist->edge_num;i++) {
      childlist->child[childlist->edgelist[i]]->to->free();
      childlist->child[childlist->edgelist[i]]->free();
      childlist->child[childlist->edgelist[i]]=NULL;
    }
    childlist->edge_num=0;
    link=free_chain;
    free_chain=this;
  }
}

