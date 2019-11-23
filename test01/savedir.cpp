/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2004 ****, Inc.
 *
 *  File Name: savedir.cpp
 *  Created  : 11/03/03(水) 09:46:44
 *
 *  Function
 *    Preserve path name
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  $Revision: 1.1.1.1 $
 *  $Date: 2008/07/27 10:32:48 $
 *  $Author: poko $
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <tchar.h>
#include "savedir.h"

// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static D_NODE* Malloc(HANDLE hHeap, TCHAR *target)
{
  D_NODE* p_node;
  size_t  size;
  size_t  str_len = _tcslen(target) + 1;

  size = sizeof( D_NODE ) + str_len * sizeof(TCHAR);

  p_node = (D_NODE *)HeapAlloc(hHeap, HEAP_NO_SERIALIZE, size);

  if (p_node)
    {
      _tcscpy_s(p_node->d_name, str_len, target);
      p_node->size  = size;
      p_node->hHeap = hHeap;
      p_node->subd  = NULL;
      p_node->ltime = 0;
      p_node->htime = 0;
      p_node->d_no  = 0;
      p_node->attr  = 0;
      p_node->exist = 0;
    }

  return p_node;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static D_NODE *node_search(D_NODE *p_parent, TCHAR *elem)
{
  D_NODE* p_node = p_parent->subd;
  D_NODE* p_prev = NULL;
  HANDLE  hHeap  = p_parent->hHeap;

  while (1)
    {
      if (p_node == NULL)
        {
          p_node = Malloc(hHeap, elem);

          if (p_node == NULL)
            {
              break;
            }

          p_node->next   = p_parent->subd;
          p_parent->subd = p_node;

          break;
        }

      if (_tcsicmp(p_node->d_name, elem) == 0)
        {
          p_node->exist = 1;

          if (p_prev)
            {
              p_prev->next   = p_node->next;
              p_node->next   = p_parent->subd;
              p_parent->subd = p_node;
            }
          break;
        }

      p_prev = p_node;
      p_node = p_node->next;
    }

  return p_node;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
void delete_savedir(HANDLE handle)
{
  D_NODE* node_top = (D_NODE *)handle;

  if ( node_top )
    {
      HeapDestroy( node_top->hHeap );
      node_top = NULL;
    }
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
D_NODE *savedir(HANDLE handle, const TCHAR *path)
{
  TCHAR*  cp;
  D_NODE *node_top = (D_NODE *)handle;
  D_NODE *p_topd   = node_top;
  HANDLE  hHeap    = NULL;
  TCHAR   buf[MAX_PATH];
  TCHAR  *context = NULL;
  D_NODE *p_node  = p_topd;

  if (p_topd == NULL)
    {
      hHeap = HeapCreate(HEAP_NO_SERIALIZE, 1000000, 0);

      if (hHeap)
        {
          p_topd = (D_NODE *)HeapAlloc(hHeap, HEAP_NO_SERIALIZE, sizeof(D_NODE));

          if (p_topd)
            {
              p_topd->next      = NULL;
              p_topd->subd      = NULL;
              p_topd->size      = 0;
              p_topd->hHeap     = hHeap;
              p_topd->d_name[0] = _T('\0');
              p_node            = p_topd;
            }
          else
            {
              /* Although heap allocation was possible, memory allocation failed */

              HeapDestroy(hHeap);
            }
        }
    }

  if (p_topd)
    {
      _tcscpy_s(buf, MAX_PATH, path);

      for (cp = _tcstok_s(buf, L"\\", &context); p_node && cp; cp = _tcstok_s(NULL, L"\\", &context))
        {
          p_node = node_search(p_node, cp);
        }
    }

  return p_node;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
D_NODE *savedir(HANDLE handle)
{
  return (D_NODE *)handle;
}
// --------------------------------------------------------------------------
