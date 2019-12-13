/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2004 ****, Inc.
 *
 *  File Name: scandir.cpp
 *  Created  : 11/03/03(水) 09:46:44
 *
 *  Function
 *    Scan folder
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

/* Exclude rarely used parts from Windows headers */

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <Shlwapi.h>
#include <stddef.h>
#include <tchar.h>
#include <assert.h>
#include "dirent.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define FIND_DATA   WIN32_FIND_DATA

/****************************************************************************
 * Private types
 ****************************************************************************/

typedef struct _NAMELIST_
{
  HANDLE    hHeap;
  dirent*   list[ 1 ];
}
NAMELIST;

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
 * Private data
 ****************************************************************************/

/****************************************************************************
 * Private functions
 ****************************************************************************/

// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static dirent* malloc_entry( HANDLE hHeap, dirent *top_entry, const dirent *src_entry )
{
  size_t  len   = _tcslen(src_entry->d_name) + 1;
  size_t  size  = sizeof(dirent) + len * sizeof(TCHAR);
  dirent* entry = (dirent *)HeapAlloc(hHeap, HEAP_NO_SERIALIZE, size);

  if ( entry )
    {
      *entry      = *src_entry;
      entry->next = top_entry;
      entry->size = size;
      _tcscpy_s(entry->d_name, len, src_entry->d_name);
    }

  return entry;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static void set_atter( dirent* entry, FIND_DATA* fp )
{
  entry->dwFileAttributes = fp->dwFileAttributes;
  entry->ftCreationTime   = fp->ftCreationTime;
  entry->ftLastAccessTime = fp->ftLastAccessTime;
  entry->ftLastWriteTime  = fp->ftLastWriteTime;
  entry->nFileSizeHigh    = fp->nFileSizeHigh;
  entry->nFileSizeLow     = fp->nFileSizeLow;
  entry->state            = DENT_FLT_MALLOC;
  _tcscpy_s( entry->cAlternateFileName, 14, fp->cAlternateFileName );
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static void Append(TCHAR *path, size_t len, const TCHAR* dir, const TCHAR* name)
{
  size_t  len1 = _tcslen(dir);
  size_t  len2 = _tcslen(name);

  if (len1 + len2 >= len)
    {
      _tprintf(L"%s %s\n", dir, name);
    }

  assert(len1 + len2 < len);

  if (dir[0] == L'\0' || (dir[0] == L'.' && dir[1] == L'\0'))
    {
      path[0] = L'\0';
    }
  else
    {
      _tcscpy_s(path, len, dir);

      for (size_t i = 0; i < len1; i++)
        {
          if (path[len1 - i - 1] != L'\\')
            {
              break;
            }
          path[len1 - i - 1] = L'\0';
        }
    }

  _tcscat_s(path, len, L"\\");
  _tcscat_s(path, len, name);
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static dirent *open_sub(HANDLE  hHeap,
                        dirent *top,
                        int  ( *filter )( dirent* ),
                        void   *param,
                        int    *cancel )
{
  dirent*           entry     = NULL;
  HANDLE            hFile;
  static FIND_DATA  fd;
  const TCHAR*      dir       = top->d_name;
  int               depth     = top->depth + 1;
  dirent*           top_entry = top->next;
  size_t            max_len   = MAX_PATH;

  top->d_cnt = 0;

  entry = (dirent *)alloca(sizeof(dirent) + max_len * sizeof(TCHAR));

  Append(entry->d_name, max_len, dir, _T("*.*"));

  hFile = ::FindFirstFile(entry->d_name, &fd);

  if (hFile == INVALID_HANDLE_VALUE)
    {
      return top_entry;
    }

  do
    {
      if ( _tcscmp( fd.cFileName, _T( "." ) ) == 0 )
        {
          continue;
        }

      Append(entry->d_name, max_len, dir, fd.cFileName);

      entry->depth = depth;
      entry->param = param;

      set_atter( entry, &fd );

      if ( filter )
        {
          entry->state = filter( entry );

          if ( entry->state < 0 )
            {
              /* Interruption */

              *cancel = 1;
              break;
            }

          /* Drop DENT_FLT_RECURSION if target is not a directory */

          if( !( entry->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
            {
              entry->state &= (~DENT_FLT_RECURSION);
            }
        }

      if( ( entry->state & DENT_FLT_RECURSION ) || ( entry->state & DENT_FLT_MALLOC ) )
        {
          /* Reserve file contents in memory */

          top_entry = malloc_entry( hHeap, top_entry, entry );

          if ( top_entry == NULL )
            {
              break;
            }

          top->d_cnt++;
        }
    }
  while( ::FindNextFile( hFile, &fd ) );

  ::FindClose( hFile );

  return top_entry;
}
// --------------------------------------------------------------------------


/****************************************************************************
 * Public functions
 ****************************************************************************/

// ==========================================================================
// -- 
// --------------------------------------------------------------------------
int scandir( const TCHAR   *dir,
             dirent      ***namelist,
             SCAN_FILTER    f_filter,
             SCAN_FILTER    d_filter,
             SCAN_COMPARE   compar,
             void          *param )
{
  int       count  = 0;
  int       cancel = 0;
  size_t    len;
  dirent*   node;
  dirent*   top_entry    = NULL;
  dirent**  top_namelist = NULL;
  HANDLE    hHeap;
  NAMELIST* name_t;

  *namelist = NULL;

  hHeap = HeapCreate(HEAP_NO_SERIALIZE, 1000000, 0);

  if (hHeap == NULL)
    {
      return 0;
    }

  len = _tcslen(dir) + 1;

  node = (dirent *)alloca(sizeof(dirent) + len * sizeof(TCHAR));

  /* First element setting */

  top_entry = node;

  _tcscpy_s(top_entry->d_name, len, dir);

  /* Excluding the '\' mark at the end */

  top_entry->state            = DENT_FLT_RECURSION;
  top_entry->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
  top_entry->next             = NULL;
  top_entry->depth            = -1;

  /* Expand the directory below */

  while ( top_entry )
    {
      top_entry->param = param;

      if ( ( top_entry->state & DENT_FLT_RECURSION ) &&
           ( top_entry->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) &&
           _tcscmp( PathFindFileName( top_entry->d_name ) , _T( ".." ) ) != 0 )
        {
          /* Expand subdirectories */

          top_entry->next = open_sub( hHeap, top_entry, f_filter, param, &cancel );

          if ( cancel )
            {
              break;
            }

          if ( d_filter )
            {
              d_filter( top_entry );
            }
        }

      if ( top_entry->state & DENT_FLT_MALLOC )
        {
          count++;
        }

      top_entry = top_entry->next;
    }

  if ( count == 0 )
    {
      /* No search result file */

      HeapDestroy( hHeap );
    }
  else
    {
      /* Substitute elements into the list below */

      top_entry = node->next;

      name_t = ( NAMELIST* )HeapAlloc( hHeap, HEAP_NO_SERIALIZE, sizeof( dirent* ) * ( count + 1 ) + sizeof( NAMELIST ) );

      if ( name_t == NULL )
        {
          /* Exception! */

          count = -1;
          HeapDestroy( hHeap );
        }
      else
        {
          *namelist = top_namelist = name_t->list;

          name_t->hHeap = hHeap;

          while ( top_entry )
            {
              if( top_entry->state & DENT_FLT_MALLOC )
                {
                  *top_namelist = top_entry;
                  top_namelist++;
                }
              top_entry = top_entry->next;
            }

          *top_namelist = top_entry;

          if ( compar )
            {
              qsort( *namelist, count, sizeof( dirent* ), ( int(*)( const void*, const void* ) )compar );
            }
        }
    }

  return count;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
void delete_scandir(dirent **top_namelist)
{
  NAMELIST* name_t;

  if (top_namelist)
    {
      name_t = (NAMELIST *)((char *)top_namelist - offsetof(NAMELIST, list));

      HeapDestroy(name_t->hHeap);
    }
}
// --------------------------------------------------------------------------
