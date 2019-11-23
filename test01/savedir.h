/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2004 ****, Inc.
 *
 *  File Name: savedir.h
 *  Created  : 11/03/03(水) 09:46:44
 *
 *  Function
 *    パス名を保持する
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  $Revision: 1.1.1.1 $
 *  $Date: 2008/07/27 10:32:48 $
 *  $Author: poko $
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 */
#ifndef _SAVEDIR_H_
#define _SAVEDIR_H_

typedef struct DIR_NODE_T
{
  struct DIR_NODE_T*  next;
  struct DIR_NODE_T*  subd;
  size_t              size;
  DWORD               d_no;
  DWORD               attr;
  int                 exist;
  unsigned int        htime;
  unsigned int        ltime;
  HANDLE              hHandle;
  HANDLE              hHeap;
  TCHAR               d_name[ 1 ];
}
D_NODE;

void    delete_savedir(HANDLE handle);
D_NODE* savedir(HANDLE handle, const TCHAR *path);

#endif // _SAVEDIR_H_
