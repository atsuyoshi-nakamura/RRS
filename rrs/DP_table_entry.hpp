//
//  DP_table_entry.hpp
//  Dynamic Programming Table Entry
//
#ifndef _INC_DP_TABLE_ENTRY
#define _INC_DP_TABLE_ENTRY   

#include "repeat.hpp"

class DP_table_entry {
public:
  int length;
#ifdef APPROXIMATE
  int alength;
#endif
  repeat *best_entry;
  repeat *entry_list;
};

#endif    //INC_DP_TABLE_ENTRY
