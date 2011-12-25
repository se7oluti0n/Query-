#ifndef _ALLOCATEMEMORY_H_
#define _ALLOCATEMEMORY_H_

#define TFREE(x) if (x) tfree((char***)x)
#define BFREE(x) if (x) bfree((char**)x)

//intå^óp
#define TALLOC_INT(num, rows, columns) (int***)talloc(num, rows, columns, sizeof(int))
#define BALLOC_INT(rows, columns) (int**)balloc(rows, columns, sizeof(int))

void _cd_home(void);

void** balloc(long rows, long columns, unsigned long size);
void bfree(char** storage);
void*** talloc(short num, long rows, long columns, unsigned long size);
void tfree(char*** storage);

#endif