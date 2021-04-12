#include "err_str.h"


void err_str(const char* str,int err)
{
    perror(str);
//	exit(err);
}
