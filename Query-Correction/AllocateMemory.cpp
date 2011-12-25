//**************************************************************************
//メモリ確保に関する関数群
//--------------------------------------------------------------------------
//_cd_home・・・
//balloc・・・２次元確保
//bfree・・・２次元解放
//talloc・・・３次元確保
//tfree・・・３次元解放
//**************************************************************************
#include "stdafx.h"
#include <direct.h>

#include "AllocateMemory.h"

void _cd_home(void)
{
  char file_path[] = __FILE__;
  short cnt = 0, n = strlen(file_path);
  for (short i = n - 1; i > 0; i--){
    if (file_path[i] == '\\' || file_path[i] == '/'){
      cnt++;
      if (cnt > 0){
        file_path[i] = NULL;
        break;
      }
    }
  }
  _chdir((const char *)file_path);
}

//******************************************************************************
//  目的説明:   2次元配列のメモリの確保を行う
//  入力引数:
//    rows:     行数
//    columns:  列数
//    size:     各要素のサイズ
//  戻り値:     void型の2次元配列へのポインタ
//******************************************************************************
void** balloc(long rows, long columns, unsigned long size)
{
  long i;
  char** buff;

  if (rows <= 0 || columns <= 0) return NULL;
  buff = (char**)calloc((unsigned)(rows + 1), (unsigned)sizeof(char *));
  if (buff == NULL) return NULL;
  buff[rows - 1] = (char*)calloc((unsigned)(rows * columns), size);
  if (buff[rows - 1] == NULL) return NULL;

  for(i = 1; i < rows; i++) buff[rows - 1 - i] = (char*)buff[rows - i] + size * columns;

  return (void**)buff;
}


//******************************************************************************
//  目的説明:	ballocで確保したメモリを解放する
//  入力引数:
//    **storage:  2次元配列
//******************************************************************************
void bfree(char** storage)
{
  long i = 0;

  while(storage[i] != NULL){
    i++;
  }
  free((char*)storage[i - 1]);
  free((char*)storage);
}
//*****************************************************************
//  目的説明:	3次元配列のメモリの確保を行う

//  戻り値:	    void型の3次元配列へのポインタ
//********************************************************************
void*** talloc(short num, long rows, long columns, unsigned long size)
{
  long i;
  void*** buff;

  buff = (void***)calloc(num + 1, sizeof(char*));
  if (buff == NULL) return NULL;
  for(i = 0; i < num; i++){
    buff[i] = (void**)balloc(rows, columns, size);
    if (buff[i] == NULL) return NULL;
  }

  return buff;
}

//*************************************************
//  目的説明:	tallocで確保したメモリを解放する
//  入力引数:
//    ***storage:	3次元配列
//*************************************************
void tfree(char*** storage)
{
  long i = 0;

  while(storage[i] != NULL){
    bfree((char**)storage[i]);
    i++;
  }
  free(storage);
}