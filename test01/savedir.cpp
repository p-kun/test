/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2019 ****, Inc.
 *
 *  File Name: savedir.cpp
 *  Created  : 19/12/01(日) 09:46:44
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

/****************************************************************************
 * Included files
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <tchar.h>
#include "savedir.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

/****************************************************************************
 * Public type declarations
 ****************************************************************************/

/****************************************************************************
 * Public function prototypes
 ****************************************************************************/

/****************************************************************************
 * Public data
 ****************************************************************************/

/****************************************************************************
 * Private types
 ****************************************************************************/

/****************************************************************************
 * Private data
 ****************************************************************************/

/* Top node */

static D_NODE*  _node_top = NULL;

/****************************************************************************
 * Private functions
 ****************************************************************************/

static D_NODE* Malloc(D_NODE *p_parent, HANDLE hHeap, TCHAR *target)
{
  D_NODE* p_node;
  size_t  size;
  size_t  str_len = _tcslen(target) + 1;

  size = sizeof(D_NODE) + str_len * sizeof(TCHAR);

  p_node = (D_NODE *)HeapAlloc(hHeap, HEAP_NO_SERIALIZE, size);

  if (p_node)
    {
      _tcscpy_s(p_node->d_name, str_len, target);

      p_node->size   = size;
      p_node->hHeap  = hHeap;
      p_node->subd   = NULL;
      p_node->parent = p_parent;
      p_node->ltime  = 0;
      p_node->htime  = 0;
      p_node->d_no   = 0;
      p_node->attr   = 0;
      p_node->exist  = 0;

      for (int i = 0; i < 8; i++)
        {
          p_node->d_data[i] = 0;
        }
    }

  return p_node;
}

/* ------------------------------------------------------------------------ */
static D_NODE *create(void)
{
  if (_node_top)
    {
      return _node_top;
    }

  /* Create memory heap */

  HANDLE  hHeap = HeapCreate(HEAP_NO_SERIALIZE, 1000000, 0);

  if (!hHeap)
    {
      return NULL;
    }

  /* Initialize top node */

  D_NODE  *p_topd = (D_NODE *)HeapAlloc(hHeap, HEAP_NO_SERIALIZE, sizeof(D_NODE));

  if (p_topd)
    {
      p_topd->next      = NULL;
      p_topd->subd      = NULL;
      p_topd->parent    = NULL;
      p_topd->size      = 0;
      p_topd->hHeap     = hHeap;
      p_topd->d_name[0] = _T('\0');
      _node_top         = p_topd;
    }
  else
    {
      /* Although heap allocation was possible, memory allocation failed */

      HeapDestroy(hHeap);
    }

  return p_topd;
}

/****************************************************************************
 * Public functions
 ****************************************************************************/

D_NODE *node_search(D_NODE *p_parent, TCHAR *elem)
{
  D_NODE *p_node = p_parent->subd;
  D_NODE *p_prev = NULL;
  HANDLE  hHeap  = p_parent->hHeap;

  while (1)
    {
      if (p_node == NULL)
        {
          p_node = Malloc(p_parent, hHeap, elem);

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

/* ------------------------------------------------------------------------ */
void delete_savedir(void)
{
  if (_node_top)
    {
      HeapDestroy(_node_top->hHeap);
      _node_top = NULL;
    }
}

/* ------------------------------------------------------------------------ */
D_NODE *savedir(const TCHAR *path)
{
  D_NODE  *p_node = create();

  return savedir(p_node, path);
}

/* ------------------------------------------------------------------------ */
D_NODE *savedir(D_NODE *p_node, const TCHAR *path)
{
  TCHAR   *cp;
  TCHAR    buf[MAX_PATH];
  TCHAR   *ctxt = NULL;

  _tcscpy_s(buf, MAX_PATH, path);

  for (cp = _tcstok_s(buf, L"\\/", &ctxt); p_node && cp; cp = _tcstok_s(NULL, L"\\/", &ctxt))
    {
      p_node = node_search(p_node, cp);
    }

  return p_node;
}

/* ------------------------------------------------------------------------ */
D_NODE *savedir(void)
{
  return _node_top;
}

/* ------------------------------------------------------------------------ */
int savedir_log(D_NODE *p_parent, D_NODE_CB scan_cb, void *param)
{
  D_NODE *p_node = p_parent->subd;
  int     ret    = 0;

  while (p_node)
    {
      if (scan_cb)
        {
          ret = scan_cb(p_node, param);

          if (ret < 0)
            {
              break;
            }
        }

      ret = savedir_log(p_node, scan_cb, param);

      if (ret < 0)
        {
          break;
        }

      p_node = p_node->next;
    }

  return ret;
}

/* ------------------------------------------------------------------------ */
int savedir_log(D_NODE_CB scan_cb, void *param)
{
  return savedir_log(_node_top, scan_cb, param);
}

/* ------------------------------------------------------------------------ */
void savedir_get_fullpath(TCHAR *path, int size, D_NODE *d_node)
{
  TCHAR  *data[100];
  int     i;

  for (i = 0; i < 100 && d_node; i++)
    {
      data[i] = d_node->d_name;

      d_node = d_node->parent;
    }

  i--;
  i--;

  if (i < 0)
    {
      return;
    }

  _tcscpy_s(path, MAX_PATH, data[i]);

  for (; i > 0; i--)
    {
      _tcscat_s(path, MAX_PATH, L"\\");
      _tcscat_s(path, MAX_PATH, data[i - 1]);
    }
}
