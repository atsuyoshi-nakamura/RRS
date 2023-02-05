//
//  suffixtree.cpp
//  Suffix tree
//

#include <stdlib.h>
#include <iostream>
#include "suffixtree.hpp"
#include "occurrences.hpp"

using namespace std;
using namespace edge_space;
using namespace node_space;

unsigned char suffixtree::char_code[256]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int suffixtree::code_num=0;

suffixtree::suffixtree(int len_in,char mode) {
  str=(unsigned char *)malloc((len_in+1)*sizeof(unsigned char));
  leaf = new node[len_in+1];
  if(mode=='D') {
    comp_idx=new int[len_in+1];
  }
  else {
    comp_idx=NULL;
  }
  h_table=new int[2*(len_in+1)];
  L=new node*[2*(len_in+1)];
  left_most_sign=new int[4*(len_in+1)];
  primitive_check_tbl=new int[2*len_in];
  root.childlist=NULL;
}

void suffixtree::make(char *str_in,int len_in,char mode) {
  len = len_in;
  set_str(str_in);
  for(int i=0;i<=len;i++) leaf[i].clear();
  if(root.childlist!=NULL) {
    if(root.childlist->edge_num!=0) {
      for(int i=0;i<root.childlist->edge_num;i++) {
        root.childlist->child[root.childlist->edgelist[i]]->to->free();
        root.childlist->child[root.childlist->edgelist[i]]->free();
        root.childlist->child[root.childlist->edgelist[i]]=NULL;
      }
      root.childlist->edge_num=0;
    }
  }
  if(mode=='D') {
    create_ZL();
  }
  else {
    create();
  }
}

void suffixtree::put_char(char c) {
  if(char_code[(unsigned char)c]==0) {
    char_code[(unsigned char)c]=++code_num;
  } 
}

void suffixtree::set_str(char *in_str) {
  for(int i=0;i<len;i++) {
    str[i]=char_code[(unsigned char)in_str[i]];
  }
  str[len]=0;
}

void suffixtree::create() {
  root.addchild(str[0],0,len+1,&leaf[0]);
  int cur_leaf=0;
  node *cur_node=&leaf[cur_leaf];
  edge *cur_edge=cur_node->parent;
  int start=cur_edge->range[0];
  int edge_len=2-cur_edge->range[0];
  cur_node=cur_edge->from;
  for(int i=1;i<=len;i++,edge_len++) {
    node *new_node=NULL;
    while(cur_leaf<i) {
      if(cur_node->link==NULL) {
        start++;
        edge_len--;
      }
      else {
        cur_node=cur_node->link;
      }
      cur_edge=cur_node->childlist->child[str[start]];
      if(cur_edge!=NULL) {
        int cur_edge_len;
        while((cur_edge_len=cur_edge->range[1]-cur_edge->range[0])<edge_len) {
          cur_node=cur_edge->to;
          start+=cur_edge_len;
          edge_len-=cur_edge_len;
          if((cur_edge=cur_node->childlist->child[str[start]])==NULL) break;
        }
        if(cur_edge!=NULL) {
          while(str[start+edge_len-1]==str[cur_edge->range[0]+edge_len-1]) {
            if(new_node!=NULL) {
              new_node->link=cur_node;
              new_node->link_label=str[cur_leaf];
              new_node=NULL;
            }  
            i++;
            edge_len++;
            if((cur_edge_len=cur_edge->range[1]-cur_edge->range[0])<edge_len) {
              cur_node=cur_edge->to;
              start+=cur_edge_len;
              edge_len-=cur_edge_len;
              if((cur_edge=cur_node->childlist->child[str[start]])==NULL) break;            
            }
          }
        }
      }

      node *last_node=new_node;
      if(cur_edge!=NULL) {
        new_node=node::create();
        int start1=cur_edge->range[0]+edge_len-1;
        new_node->addchild(str[start1],start1,cur_edge->range[1],cur_edge->to);
        cur_edge->range[1]=start1;
        cur_edge->to=new_node;
        new_node->parent=cur_edge;
      }
      else {
        new_node=cur_node;
      }
      if(last_node!=NULL) {
        last_node->link=new_node;
        last_node->link_label=str[cur_leaf];
      }
      int start2=start+edge_len-1;
      new_node->addchild(str[start2],start2,len+1,&leaf[cur_leaf+1]);
      cur_leaf++;
    }
  }
}

// + Ziv-Lempel decomposition + leaf_no + smallest_lno_child
void suffixtree::create_ZL() {
  comp_num=1;
  comp_idx[0]=1;
  leaf[0].leaf_no=0;
  root.addchild(str[0],0,len+1,&leaf[0]);
  int cur_leaf=0;
  node *cur_node=&leaf[cur_leaf];
  edge *cur_edge=cur_node->parent;
  int start=cur_edge->range[0];
  int edge_len=2-cur_edge->range[0];
  cur_node=cur_edge->from;
  for(int i=1;i<=len;i++,edge_len++) {
    node *new_node=NULL;
    while(cur_leaf<i) {
      if(cur_node->link==NULL) {
        start++;
        edge_len--;
      }
      else {
        cur_node=cur_node->link;
      }
      cur_edge=cur_node->childlist->child[str[start]];
      if(cur_edge!=NULL) {
        int cur_edge_len;
        while((cur_edge_len=cur_edge->range[1]-cur_edge->range[0])<edge_len) {
          cur_node=cur_edge->to;
          start+=cur_edge_len;
          edge_len-=cur_edge_len;
          if((cur_edge=cur_node->childlist->child[str[start]])==NULL) break;
        }
        if(cur_edge!=NULL) {
          while(str[start+edge_len-1]==str[cur_edge->range[0]+edge_len-1]) {
            if(new_node!=NULL) {
              new_node->link=cur_node;
              new_node->link_label=str[cur_leaf];
              new_node=NULL;
            }  
            i++;
            edge_len++;
            if((cur_edge_len=cur_edge->range[1]-cur_edge->range[0])<edge_len) {
              cur_node=cur_edge->to;
              start+=cur_edge_len;
              edge_len-=cur_edge_len;
              if((cur_edge=cur_node->childlist->child[str[start]])==NULL) break;            
            }
          }
        }
      }

      node *last_node=new_node;
      if(cur_edge!=NULL) {
        new_node=node::create();
        int start1=cur_edge->range[0]+edge_len-1;
        new_node->addchild(str[start1],start1,cur_edge->range[1],cur_edge->to);
        cur_edge->range[1]=start1;
        cur_edge->to=new_node;
        new_node->parent=cur_edge;
      }
      else {
        new_node=cur_node;
      }
      if(cur_leaf+1==comp_idx[comp_num-1]) {
        if(cur_leaf+1==i) comp_idx[comp_num++]=i+1;
        else            comp_idx[comp_num++]=i;
      }
      if(last_node!=NULL) {
        last_node->link=new_node;
        last_node->link_label=str[cur_leaf];
      }
      int start2=start+edge_len-1;
      leaf[cur_leaf+1].leaf_no=cur_leaf+1;
      new_node->addchild(str[start2],start2,len+1,&leaf[cur_leaf+1]);
      cur_leaf++;
    }
  }
}

void suffixtree::print() {
  root.print(0,str);
}

void suffixtree::print_ZL() {
  cout<<endl;
  int j=0;
  for(int i=0;i<len;i++) {
    if(i==comp_idx[j]) {
      cout<<"|";
      j++;
    } 
    cout<<((char *)str)[i];
  }
  cout<<endl;
}

void suffixtree::prep_lca_query() {
  int cur_id=1;
  id_level=0;
  root.set_id(&cur_id,&id_level,h_table,L,0);
  root.set_A(0);
  int from=0;
  int to=1;
  for(int level=0;from<2*(len+1);level++,from=to,to*=2) {
    for(int i=from;i<to;i++) {
      left_most_sign[i]=level;
    }
  }
}

int suffixtree::lca_in_B(int i,int j) {
  int lms=left_most_sign[i^j];
  int mask=(-1)<<lms;
  return (i&mask)|(1<<(lms-1));
}

node *suffixtree::same_run_node(int x,int j) {
  node *x_near;
  if(h_value(leaf[x].A)==j) {
    x_near=&leaf[x];
  }
  else {
    int mask=(0x7FFFFFFF)>>(sizeof(int)*8-j);
    int k=left_most_sign[leaf[x].A&mask];
    mask=(-1)<<k;
    int I_w_id=(leaf[x].I->id&mask)|(1<<(k-1));
    x_near=L[I_w_id]->parent->from;
  }
  return x_near;
}

int suffixtree::h_value(int x) {
  int level_val=1<<id_level;
  if(x>level_val) x%=level_val;
  return h_table[x];
}

int suffixtree::longest_com_subseq_len(int x,int y) {
  int h_b=h_table[lca_in_B(leaf[x].I->id,leaf[y].I->id)];
  int mask=(-1)<<(h_b-1);
  int j=h_value(leaf[x].A&leaf[y].A&mask);
  node *x_near=same_run_node(x,j);
  node *y_near=same_run_node(y,j);
  if(x_near->id>y_near->id) x_near=y_near;
  return x_near->depth;
}

void suffixtree::algorithm_phaseII(occurrences *P) {
  root.mark_repeat_end(P);
}

void suffixtree::algorithm_phaseIII() {
  root.mark_all_vocabulary(str);
}

void suffixtree::print_all_vocabulary() {
  root.print_vocabulary((char *)str);
}

void suffixtree::set_primitive_repeats(DP_table_entry *DP_table) {
  for(int i=0;i<len;i++) DP_table[i].entry_list=NULL;
  for(int i=0;i<2*len;i++) primitive_check_tbl[i]=0;
  int_list_entry *tail;
  int_list_entry *head;
#ifdef APPROXIMATE
  head=root.set_prepeats(DP_table,primitive_check_tbl,len,&tail,str);
#else
  head=root.set_prepeats(DP_table,primitive_check_tbl,len,&tail);
#endif
  int_list_entry::free_list(head,tail);
}

#ifdef APPROXIMATE
void suffixtree::copy_str(suffixtree *stree) {
  free(str);
  str=stree->str;
}
#endif
