//
//  tandem_repeats.cpp
//  Program for marking the vocabulary of tandem repeats on a given string
//  D. Gusfield and J. Stoye, Linear time algorithms for finding and representing all the tandem repeats in a string,
//  JCSS 69(2004) 525-546
//

#include <stdlib.h>
#include <iostream>
#include "suffixtree.hpp"
#include "occurrences.hpp"
#include "tandem_repeats.hpp"

char *tandem_repeats::backward_str;
occurrences *tandem_repeats::leftmost_occur;
suffixtree *tandem_repeats::stree;
suffixtree *tandem_repeats::bstree;

void tandem_repeats::alloc_resource(int len)
{
  backward_str=(char *)malloc(sizeof(char)*(len+1));
  leftmost_occur=new occurrences(len+1);
  node::set_base_num(suffixtree::code_num+1);
  stree=new suffixtree(len,'D');
  bstree=new suffixtree(len,'N');
}

suffixtree *tandem_repeats::get_decorated_stree(char *str,int len,char mode)
{
  char *bstr=create_backword_str(str,len);

  if(mode=='B') {
    char *temp=str;
    str=bstr;
    bstr=temp;
  }

  stree->make(str,len,'D');

  //    stree->print();

  //    stree->print_ZL();

  stree->prep_lca_query();

  //  cout << stree->longest_com_subseq_len(1,8) << endl;

  bstree->make(bstr,len,'N');

  bstree->prep_lca_query();

  // phase I
  int cur_start=0;
  int i;
  leftmost_occur->clear();
  for(i=0;i<stree->comp_num-1;i++) {
    algorithm_1a(len,cur_start,stree->comp_idx[i],stree,bstree,leftmost_occur);
    algorithm_1b(len,cur_start,stree->comp_idx[i],stree->comp_idx[i+1],stree,bstree,leftmost_occur);
    cur_start=stree->comp_idx[i];
  }
  algorithm_1b(len,cur_start,stree->comp_idx[i],stree->comp_idx[i],stree,bstree,leftmost_occur);

  // phase II
  stree->algorithm_phaseII(leftmost_occur);

  // phase III
  stree->algorithm_phaseIII();

  //        stree->print_all_vocabulary();

#ifdef APPROXIMATE
  stree->copy_str(bstree);
#endif
  return stree;
}

char *
tandem_repeats::create_backword_str(char *str,int len) {
  for(int i=0;i<len;i++) backward_str[i]=str[len-i-1];
  backward_str[len]=0;
  return backward_str;
}

void
tandem_repeats::algorithm_1a(int len,int h,int h_1,suffixtree *stree,suffixtree *bstree,occurrences *P) {
  for(int k=1;k<=h_1-h;k++) {
    int q=h_1-k;
    int k_1=stree->longest_com_subseq_len(q,h_1);
    int k_2=bstree->longest_com_subseq_len(len-q,len-h_1);
    if(q-k_2<h) k_2=q-h;
    int start=q - ((k_2>(k-1))? k-1:k_2);
    if((k_1+k_2>=k)&&(k_1>0)) P->push(start,2*k);
  }
}

void
tandem_repeats::algorithm_1b(int len,int h,int h_1,int h_2,suffixtree *stree,suffixtree *bstree,occurrences *P) {
  for(int k=2;k<=h_2-h-1;k++) {
    int q=h+k;
    int k_1=stree->longest_com_subseq_len(q,h);
    int k_2=bstree->longest_com_subseq_len(len-q,len-h);
    int start=h - ((k_2>(k-1))? k-1:k_2);
    if((k_1+k_2>=k)&&(k_1>0)&&(start+k<=h_1)&&(k_2>0)) P->push(start,2*k);
  }
}
