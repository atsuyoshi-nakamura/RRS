//
//  rrs.cpp
//  program for generating the minimum repetition representation of a given string
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream> 
#include "tandem_repeats.hpp"
#ifdef TIMER
#include "timer.hpp"
#endif

using namespace std;

static char *work_buf;
static int work_pos;
static char *str_buffer;
static int buffer_pos;
static int *pos_list;
static char *whole_str;
static int whole_str_len;
static char *str;
static DP_table_entry *DP_table;
static int from_pos;
static int str_len;
static int skipping_n_len=0;
static int work_limit_size;

#define CONSECUTIVE_N_LEN 10
#define WORK_SIZE 6

void
file_contents(char *file_name) {
  whole_str_len=0;
  char ch;
  ifstream fin;
  fin.open(file_name);
#ifdef FA
  while(fin.get(ch)) if(ch=='\n') break;
  while(fin.get(ch)) if(ch!='\n') whole_str_len++;
#else
  while(fin.get(ch)) whole_str_len++;
#endif
  fin.close();
  whole_str=(char *)malloc(sizeof(char)*(whole_str_len+1));
  whole_str_len=0;
  fin.open(file_name);
#ifdef FA
  while(fin.get(ch)) if(ch=='\n') break;
  while(fin.get(whole_str[whole_str_len])) if(whole_str[whole_str_len]!='\n') whole_str_len++;
#else
  while(fin.get(whole_str[whole_str_len++]));
#endif
  fin.close();
  whole_str[--whole_str_len]=0;
}

int
max_str_len() {
  int start_pos=0;
  int max_len=0;
  int code_no=0;
  while(start_pos<whole_str_len) {
    str_len=0;
    for(;start_pos<whole_str_len;start_pos++) {
      if(whole_str[start_pos]!='N') break;
    }
    if(start_pos>=whole_str_len) break;
    suffixtree::put_char(whole_str[start_pos]);
    int count=0;
    for(start_pos++,str_len++;start_pos<whole_str_len;start_pos++,str_len++) {
      if(whole_str[start_pos]=='N') {
        count++;
        if(count==CONSECUTIVE_N_LEN) {
          start_pos++;
          str_len-=CONSECUTIVE_N_LEN-1;
          if(str_len>max_len) max_len=str_len;
          break;
        }
      }
      else {
        if(count>0) {
          count=0;
          suffixtree::put_char('N');
        }
        suffixtree::put_char(whole_str[start_pos]);
      }
    }
  }
  if(str_len>max_len) max_len=str_len;
  return max_len;
}

void
set_next_str(int *start_pos) {
  str_len=0;
  for(;*start_pos<whole_str_len;(*start_pos)++) {
    if(whole_str[*start_pos]!='N') break;
    skipping_n_len++;
  }
  if(*start_pos>=whole_str_len) return;
  str=&whole_str[*start_pos];
  from_pos=*start_pos;
  int count=0;
  for((*start_pos)++,str_len++;*start_pos<whole_str_len;(*start_pos)++,str_len++) {
    if(whole_str[*start_pos]=='N') count++;
    else count=0;
    if(count==CONSECUTIVE_N_LEN) {
      (*start_pos)++;
      whole_str[*start_pos-CONSECUTIVE_N_LEN]=0;
      str_len-=CONSECUTIVE_N_LEN-1;
      skipping_n_len+=CONSECUTIVE_N_LEN;
      break;
    }
  }
}

void one_end_pos_DP(int idx, int max_len, repeat *end_entry) {
  DP_table[idx].length=DP_table[idx-1].length + 1;
#ifdef APPROXIMATE
  DP_table[idx].alength=DP_table[idx-1].alength;
#endif
  DP_table[idx].best_entry=NULL;
  repeat *cur_entry;
  for(cur_entry=DP_table[idx].entry_list;cur_entry!=end_entry;cur_entry=cur_entry->next) {
    if(max_len<2*cur_entry->width) break;
#ifdef APPROXIMATE
    cur_entry->best_additional_length=cur_entry->additional_length[0]+cur_entry->additional_length[1];
    int best_len=cur_entry->leftmost_repeat->length+1+DP_table[idx-2*cur_entry->width].length;    
    int best_alen=cur_entry->best_additional_length+DP_table[idx-2*cur_entry->width].alength;    
#else
    int best_len=cur_entry->length+1+DP_table[idx-2*cur_entry->width].length;
#endif
    cur_entry->best_rep_num=2;
    if((cur_entry->prev_entry!=NULL)&&(max_len>=3*cur_entry->width)) {
#ifdef APPROXIMATE
      int cur_len=cur_entry->leftmost_repeat->length+1+DP_table[idx-(cur_entry->prev_entry->best_rep_num+1)*cur_entry->width].length;
      int cur_alen=cur_entry->additional_length[1]+cur_entry->prev_entry->best_additional_length+DP_table[idx-(cur_entry->prev_entry->best_rep_num+1)*cur_entry->width].alength;
      if(cur_len+cur_alen<best_len+best_alen) {
#else
      int cur_len=cur_entry->length+1+DP_table[idx-(cur_entry->prev_entry->best_rep_num+1)*cur_entry->width].length;
      if(cur_len<best_len) {
#endif
        best_len=cur_len;
        cur_entry->best_rep_num=cur_entry->prev_entry->best_rep_num+1;
#ifdef APPROXIMATE
        best_alen=cur_alen;
        cur_entry->best_additional_length=cur_entry->prev_entry->best_additional_length+cur_entry->additional_length[1];
      }
    }
    if(DP_table[idx].length+DP_table[idx].alength>=best_len+best_alen) {
      DP_table[idx].alength=best_alen;
#else
      }
    }
    if(DP_table[idx].length>best_len) {
#endif
      DP_table[idx].length=best_len;
      DP_table[idx].best_entry=cur_entry;
    }
  }
}
/*
void make_best_str(int pos,int begin) {
  if(pos<begin) return;
  if(DP_table[pos].best_entry==NULL) {
    make_best_str(pos-1,begin,str);
    str_buffer[buffer_pos++]=str[pos];
  }
  else {
    make_best_str(pos-DP_table[pos].best_entry->width*DP_table[pos].best_entry->best_rep_num,begin,str);
    sprintf(&str_buffer[buffer_pos],"(%s)^%d",DP_table[pos].best_entry->best_rep,DP_table[pos].best_entry->best_rep_num);
    buffer_pos+=strlen(&str_buffer[buffer_pos]);
  }
}
*/

int make_best_str(int pos,int begin) {
  int cur_pos=0;
  int depth=0;
  while(pos>=begin) {
    pos_list[cur_pos++]=pos;
    if(DP_table[pos].best_entry==NULL) {
      pos--;
    }
    else {
      if(depth<=DP_table[pos].best_entry->depth)
        depth=DP_table[pos].best_entry->depth+1;
      pos=pos-DP_table[pos].best_entry->width*DP_table[pos].best_entry->best_rep_num;
    }
  }
#ifdef OUTMAXDEPTHSTR
  if(begin==0) cout << "depth=" << depth << endl;
#endif
  buffer_pos=0;
  for(cur_pos--;cur_pos>=0;cur_pos--) {
    pos=pos_list[cur_pos];
    if(DP_table[pos].best_entry==NULL) {
      str_buffer[buffer_pos++]=str[pos];
    }
    else {
#ifdef OUTMAXDEPTHSTR
      if(begin==0) {
        if(DP_table[pos].best_entry->depth==depth-1) {
          cout<< from_pos+pos << ":";
          cout<<((DP_table[pos].best_entry->leftmost_repeat==DP_table[pos].best_entry)? "f":"n")<<":";
	  cout<<"("<<DP_table[pos].best_entry->best_rep<<")^"<<DP_table[pos].best_entry->best_rep_num<<endl;
        }
      }
#endif
#ifdef APPROXIMATE
      sprintf(&str_buffer[buffer_pos],"(%s)^%d",DP_table[pos].best_entry->leftmost_repeat->best_rep,DP_table[pos].best_entry->best_rep_num);
#else
      sprintf(&str_buffer[buffer_pos],"(%s)^%d",DP_table[pos].best_entry->best_rep,DP_table[pos].best_entry->best_rep_num);
#endif
      buffer_pos+=strlen(&str_buffer[buffer_pos]);
    }
  }
  return depth;
}

char *copy_to_work() {
  if(work_pos+buffer_pos+1>work_limit_size) {
    cerr<<"work_buf overflow!"<<endl;
    exit(1);
  }
  char *copied_str;
  copied_str=&work_buf[work_pos];
  strncpy(copied_str,str_buffer,buffer_pos);
  copied_str[buffer_pos]=0;
  work_pos+=buffer_pos+1;
  return copied_str;
}

void partial_DP(int begin, repeat *entry) {
  int len=entry->width;
  DP_table[begin-1].length=0;
  DP_table[begin].length=1;
#ifdef APPROXIMATE
  DP_table[begin-1].alength=0;
  DP_table[begin].alength=0;
#endif
  DP_table[begin].best_entry=NULL;
  for(int i=1;i<len-1;i++) {
    one_end_pos_DP(begin+i,i+1,NULL);
  }
  one_end_pos_DP(begin+len-1,len,entry);
  entry->depth=make_best_str(begin+len-1,begin);
  entry->best_rep=copy_to_work();
  entry->length=DP_table[begin+len-1].length;
#ifdef APPROXIMATE
  entry->length+=DP_table[begin+len-1].alength;
#endif
}

#ifdef APPROXIMATE
// bubble sort
repeat *sort_by_width(repeat *root) {
  bool finished=false;
  repeat init;
  init.next=root;
  repeat *sorted_head=NULL;
  while((finished==false)&&(init.next->next!=sorted_head)){
    finished=true;
    repeat *cur_entry=&init;
    repeat *next_entry=cur_entry->next;
    do {
      if(next_entry->width>next_entry->next->width) {
        cur_entry->next=next_entry->next;
        next_entry->next=next_entry->next->next;
        cur_entry->next->next=next_entry;
        finished=false;
      }
      cur_entry=cur_entry->next;
      next_entry=cur_entry->next;
    }
    while(next_entry->next!=sorted_head);
    sorted_head=next_entry;
  }
  return init.next;
}

void cal_best_length(int begin,repeat *entry) {
  for(int i=1;i<entry->width;i++) {
    repeat *cur_entry=DP_table[begin+i].entry_list;
    if(cur_entry!=NULL) {
      for(;(cur_entry!=NULL)&&(2*cur_entry->width<=i+1);cur_entry=cur_entry->next) {
        repeat *cur_leftmost=cur_entry->leftmost_repeat;
        int cur_leftmost_end=cur_entry->leftmost_pos+2*cur_entry->width-1;
        if(cur_leftmost->length==0) {
          cal_best_length(cur_leftmost_end-cur_leftmost->width+1,cur_leftmost);
        }
      }
    }
  }
  partial_DP(begin,entry);
}
#endif


#ifdef STATISTICS
void print_info(int no,int p_num,int v_num,int v_len,int rrs_len,int string_len)
{
  stringstream ss;
  if(no>0) {
    ss<< "(" << no << ") ";
  }
  else {
    ss<< "[total] ";
  }
  cout << ss.str() << "#primitive tandem repeat=" << p_num << endl;
  cout << ss.str() << "#vocaburary=" << v_num << " ave_length=" << ((double)v_len)/v_num <<" total_len="<< v_len<<endl;
  cout << ss.str() << "RRS_length=" << rrs_len << endl;
  cout << ss.str() << "str_length="<< string_len <<endl;
  cout << ss.str() << "compression_rate=" << ((float)rrs_len)/string_len << " PTR density=" << ((float)p_num)/string_len << endl;
}
#endif

int main(int argc,char **argv)
{
  if(argc<2) {
    cerr << "usage:" << argv[0] << " string_file" << endl;
    exit(1);
  }

  file_contents(argv[1]);

  //  cout<< from_pos<<endl;

  int max_len=max_str_len();

  str_buffer=new char[5*max_len/2+1];

  pos_list=new int[max_len];

  work_limit_size=WORK_SIZE*max_len;

  work_buf=new char[work_limit_size];

  DP_table_entry *DP_table_ext=new DP_table_entry[max_len+1];
  DP_table=&DP_table_ext[1];

  tandem_repeats::alloc_resource(max_len);

  int start_pos=0;
  int partition_no=1;
  int total_p_num=0;
  int total_v_num=0;
  int total_v_len=0;
  int total_str_len=0;
  int total_rrs_len=0;

  for(set_next_str(&start_pos);str_len>0;set_next_str(&start_pos),partition_no++) {
    work_pos=0;

    for(int i=0;i<=str_len;i++) DP_table_ext[i].length=0;

    suffixtree *decorated_stree=tandem_repeats::get_decorated_stree(str,str_len,'B');

    decorated_stree->set_primitive_repeats(DP_table);

#ifdef APPROXIMATE
// sorting
    for(int i=1;i<str_len;i++) {
      if(DP_table[i].entry_list!=NULL) DP_table[i].entry_list=sort_by_width(DP_table[i].entry_list);
    }
#endif

    int p_num=0;
    int v_num=0;
    int v_len=0;

    for(int i=1;i<str_len;i++) {
      repeat *cur_entry;
      for(cur_entry=DP_table[i].entry_list;cur_entry!=NULL;cur_entry=cur_entry->next) {
        p_num++;
        if(cur_entry->leftmost_repeat==cur_entry) {
#ifdef APPROXIMATE
          if(cur_entry->length==0) cal_best_length(i-cur_entry->width+1,cur_entry);
#else
          partial_DP(i-cur_entry->width+1,cur_entry);
#endif
          v_num++;
          v_len+=cur_entry->width;
        }
        else {
          cur_entry->best_rep=cur_entry->leftmost_repeat->best_rep;
          cur_entry->length=cur_entry->leftmost_repeat->length;
          cur_entry->depth=cur_entry->leftmost_repeat->depth;
        }
      }
    }

    DP_table_ext[0].length=0;
    DP_table[0].length=1;
#ifdef APPROXIMATE
    DP_table_ext[0].alength=0;
    DP_table[0].alength=0;
#endif
    DP_table[0].best_entry=NULL;
    for(int i=1;i<str_len-1;i++) {
      one_end_pos_DP(i,i+1,NULL);
    }
    one_end_pos_DP(str_len-1,str_len,NULL);
    make_best_str(str_len-1,0);
    str_buffer[buffer_pos]=0;
    int rrs_length=DP_table[str_len-1].length;
#ifdef APPROXIMATE
    rrs_length+=DP_table[str_len-1].alength;
#endif
#ifdef STATISTICS    
    print_info(partition_no,p_num,v_num,v_len,rrs_length,str_len);
#endif
#ifdef STROUT
    cout << str_buffer << endl;
#endif

    for(int i=1;i<str_len;i++) {
      while(DP_table[i].entry_list!=NULL) {
        repeat *temp_entry=DP_table[i].entry_list;
        DP_table[i].entry_list=temp_entry->next;
        temp_entry->free();
      }
    }

    total_p_num+=p_num;
    total_v_num+=v_num;
    total_v_len+=v_len;
    total_str_len+=str_len;
    total_rrs_len+=rrs_length;
  }
#ifdef STATISTICS
  print_info(-1,total_p_num,total_v_num,total_v_len,total_rrs_len,total_str_len);
  cout<< "[total] " << "#skipping_N="<<skipping_n_len<<endl;
#endif
#ifdef TIMER
  timer::print_cpu_time();
#endif
}

