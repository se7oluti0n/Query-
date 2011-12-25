//**************************************************************************
//�������m�ۂɊւ���֐��Q
//--------------------------------------------------------------------------
//_cd_home�E�E�E
//balloc�E�E�E�Q�����m��
//bfree�E�E�E�Q�������
//talloc�E�E�E�R�����m��
//tfree�E�E�E�R�������
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
//  �ړI����:   2�����z��̃������̊m�ۂ��s��
//  ���͈���:
//    rows:     �s��
//    columns:  ��
//    size:     �e�v�f�̃T�C�Y
//  �߂�l:     void�^��2�����z��ւ̃|�C���^
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
//  �ړI����:	balloc�Ŋm�ۂ������������������
//  ���͈���:
//    **storage:  2�����z��
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
//  �ړI����:	3�����z��̃������̊m�ۂ��s��

//  �߂�l:	    void�^��3�����z��ւ̃|�C���^
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
//  �ړI����:	talloc�Ŋm�ۂ������������������
//  ���͈���:
//    ***storage:	3�����z��
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