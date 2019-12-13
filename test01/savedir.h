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

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#undef  MAX_PATH
#define MAX_PATH            300

/****************************************************************************
 * Public type declarations
 ****************************************************************************/

typedef struct DIR_NODE_T
{
  struct DIR_NODE_T*  next;
  struct DIR_NODE_T*  subd;
  struct DIR_NODE_T*  parent;
  size_t              size;
  DWORD               d_no;
  DWORD               attr;
  int                 exist;
  unsigned int        htime;
  unsigned int        ltime;
  HANDLE              hHandle;
  HANDLE              hHeap;
  DWORD               d_data[8];
  TCHAR               d_name[1];
}
D_NODE;

/* Callback for savedir_log */

typedef int (*D_NODE_CB)(D_NODE *, void *);

/****************************************************************************
 * Public function prototypes
 ****************************************************************************/

void    delete_savedir( void );
D_NODE* node_search( D_NODE *p_parent, TCHAR *elem );
D_NODE* savedir( const TCHAR *path );
D_NODE* savedir( void );
D_NODE* savedir(D_NODE  *p_node, const TCHAR *path);
int     savedir_log(D_NODE_CB cb = NULL, void *param = NULL);
int     savedir_log(D_NODE *parent, D_NODE_CB cb, void *param = NULL);
void    savedir_get_fullpath(TCHAR *path, int size, D_NODE *d_node);

#endif // _SAVEDIR_H_
