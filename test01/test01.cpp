/****************************************************************************
 * Included files
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <sys/stat.h>
#include <time.h>
#include <shlwapi.h>
#include "dirent.h"
#include "savedir.h"
#include "scandir.h"
#include "xgit.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define GIT_PATH3   L"C:\\user\\test\\test0\\savedir.cpp"
#define GIT_PATH0   L"C:\\user\\spresense\\sdk\\bsp\\board\\collet\\include"
#define GIT_PATH4   L"C:\\user\\spresense\\sdk\\bsp\\board\\collet"
#define GIT_PATH1   L"C:\\ProgramData\\Apple Computer\\iTunes\\adi"
#define GIT_PATH2   L"C:\\user\\spresense-arduino-compatible\\Arduino15\\packages\\SPRESENSE\\hardware\\spresense\\1.0.0\\libraries\\Audio\\examples\\application\\voice_effector"

#define FIND_DATA   WIN32_FIND_DATA

/****************************************************************************
 * Private functions
 ****************************************************************************/

/* ------------------------------------------------------------------------ */
int cnv_localtime(DWORD st_time, FILETIME *ft_time)
{
  struct tm   tm1;
  FILETIME    local1;
  FILETIME    local2;
  SYSTEMTIME  sys;
  int         res = 0;

  /* FILETIME -> local -> system */

  FileTimeToLocalFileTime(ft_time, &local1);
  FileTimeToSystemTime(&local1, &sys);

  /* system -> FILETIME */

  sys.wDayOfWeek    = 0;
  sys.wMilliseconds = 0;

  SystemTimeToFileTime(&sys, &local1);

  /* st_time -> struct tm */

  _localtime32_s(&tm1, (const __time32_t *)&st_time);

  /* system -> FILETIME */

  sys.wYear         = tm1.tm_year + 1900;
  sys.wMonth        = tm1.tm_mon  + 1;
  sys.wDay          = tm1.tm_mday;
  sys.wHour         = tm1.tm_hour;
  sys.wMinute       = tm1.tm_min;
  sys.wSecond       = tm1.tm_sec;
  sys.wDayOfWeek    = 0;
  sys.wMilliseconds = 0;

  SystemTimeToFileTime(&sys, &local2);

  /* Check time */

  if (local1.dwHighDateTime > local2.dwHighDateTime)
    {
      res = -1;
    }
  else if (local1.dwHighDateTime < local2.dwHighDateTime)
    {
      res = 1;
    }
  else
    {
      if (local1.dwLowDateTime > local2.dwLowDateTime)
        {
          res = -1;
        }
      else if (local1.dwLowDateTime < local2.dwLowDateTime)
        {
          res = 1;
        }
    }

  return res;
}

/* ------------------------------------------------------------------------ */
DWORD WINAPI ThreadFunc(LPVOID arg)
{
  for (int i = 0; i < 1; i++)
    {
      printf("ThreadFunc %d\n", i);
      Sleep(50);
    }

  return 0;
}

/* ------------------------------------------------------------------------ */
static int filter(dirent *entry)
{
  size_t  size = _tcslen(entry->d_name);

  D_NODE  *d_node = savedir(entry->d_name);

//_tprintf(L"%08X %08X %s\n", d_node->d_data[0], d_node->d_data[1], entry->d_name);
//_tprintf(L"%08p %s\n", d_node, entry->d_name);

  if (d_node->d_data[0])
    {
//    _tprintf(L"%08X %08X\n", d_node->d_data[0], d_node->d_data[1]);

      if (d_node->d_data[0] != d_node->d_data[1])
        {
          _tprintf(L"%s\n", entry->d_name);
        }
      else
        {
        }
    }

  return DENT_FLT_RECURSION | DENT_FLT_MALLOC;
}

/* ------------------------------------------------------------------------ */
int scan(D_NODE *p, void *param)
{
  int  *iii = (int *)param;
  int   ret = 0;

  (*iii)++;

  if (*iii == 1000000)
    {
      ret = -1;
    }

  p->d_data[0] = 0;
  p->d_data[1] = 0;
  p->d_data[2] = 0;
  p->d_data[3] = 0;
  p->d_data[4] = 0;
  p->d_data[5] = 0;
  p->d_data[6] = 0;

  return ret;
}

/* ------------------------------------------------------------------------ */
int git_node_callback2nd(const TCHAR *path, GIT_NODE *g_node, void *param)
{
#if 0
  D_NODE       *d_node = (D_NODE *)param;
  TCHAR         temp[MAX_PATH];
  DWORD         poyan;
  HANDLE        hFile = INVALID_HANDLE_VALUE;
  FILETIME      mtime,
                gomi;

  d_node = savedir(d_node, g_node->path);

  savedir_get_fullpath(temp, MAX_PATH, d_node);

  hFile = CreateFile(temp,
                     GENERIC_READ,
                     FILE_SHARE_READ,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    {
      return 0;
    }

  GetFileTime(hFile, &gomi, &gomi, &mtime);

  CloseHandle(hFile);

  if (cnv_localtime(g_node->mtime_hi, &mtime) == 0)
    {
      poyan = 2;
    }
  else
    {
      poyan = 1;
    }

  do
    {
      if (poyan > d_node->d_data[0])
        {
          d_node->d_data[0] = poyan;
        }

      d_node = d_node->parent;
    }
  while (d_node != (D_NODE *)param);

  if (poyan > d_node->d_data[0])
    {
      d_node->d_data[0] = poyan;
    }

  return 0;

#else

  D_NODE       *d_node = (D_NODE *)param;
  TCHAR         temp[MAX_PATH];
  DWORD         mark;
  struct _stat  st;

  d_node = savedir(d_node, g_node->path);

  /* Fullpath creation */

  _tcscpy_s(temp, MAX_PATH, path);
  PathAppend(temp, g_node->path);

  /* Get file time */

  if (_tstat(temp, &st) != 0)
    {
      return 0;
    }

  if (g_node->mtime_hi != st.st_mtime)
    {
      mark = 2;
    }
  else
    {
      mark = 1;
    }

  do
    {
      if (mark > d_node->d_data[0])
        {
          d_node->d_data[0] = mark;
          d_node->d_data[0] = mark;
        }

      d_node = d_node->parent;
    }
  while (d_node != (D_NODE *)param);

  if (mark > d_node->d_data[0])
    {
      d_node->d_data[0] = mark;
    }

  return 0;
#endif
}

/* ------------------------------------------------------------------------ */
int git_node_callback(const TCHAR *path, GIT_NODE *g_node, void *param)
{
  D_NODE       *d_node   = (D_NODE *)param;
  D_NODE       *d_parent;
  TCHAR         temp[MAX_PATH];
  struct _stat  st;

  if (d_node)
    {
      d_parent = d_node->parent;
    }
  else
    {
      return 0;
    }

  d_node = savedir(d_node, g_node->path);

  if (d_node == NULL)
    {
      return 0;
    }

  if (d_parent == NULL)
    {
      return 0;
    }

  /* Fullpath creation */

  _tcscpy_s(temp, MAX_PATH, path);
  PathAppend(temp, g_node->path);

  /* Get file time */

  if (_tstat(temp, &st) != 0)
    {
      return 0;
    }

  /* Keep git time and file time */

  for (; d_node != d_parent; d_node = d_node->parent)
    {
      if (d_node->d_data[0] < g_node->mtime_hi)
        {
          /* git time */

          d_node->d_data[0] = g_node->mtime_hi;
        }

      if (d_node->d_data[1] < (DWORD)st.st_mtime)
        {
          /* file time */

          d_node->d_data[1] = (DWORD)st.st_mtime;
        }
    }

  return 0;
}

/****************************************************************************
 * Public functions
 ****************************************************************************/

int _tmain( int argc, TCHAR **argv )
{
  TCHAR   path[MAX_PATH];
  HANDLE  handle;
  D_NODE *d_node;
  int     total;
  int     iii = 0;

  handle = git_open(GIT_PATH0, path, MAX_PATH);

  if (handle == INVALID_HANDLE_VALUE)
    {
      _tprintf(L"File not found. (%s)\n", path);

      return 1;
    }

  _tprintf(L"File open (%s)\n", path);

  d_node = savedir(path);

  savedir_log(d_node, scan, &iii);

  _tprintf(L"(%s)\n", d_node->d_name);

  total = git_scan(handle, path, git_node_callback, d_node);

  _tprintf(L"t=%d/%d\n", total, iii);

  git_close(handle);

  dirent  **entries;

  total = scandir(path, &entries, filter);

  savedir_log(d_node, scan, &iii);

  _tprintf(L"t=%d\n", total);

  return 0;
}
