#ifndef __GLOBAL_OBJ_H__
#define __GLOBAL_OBJ_H__

#include "cyaTypes.h"

class DBServerMan;

INT InitializeDBSConfig();
INT InitializeDBServer();

void FinishDBSConfig();
void FinishDBServer();

DBServerMan* FetchDBServerManObj();

#endif