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

#if 0
uint32_t totime(int year,
                int month,
                int day,
                int hour,
                int min,
                int sec,
                int dstflag )
{
        int tmpdays;
        __time64_t tmptim;
        struct tm tb;
        int daylight = 0;
        long dstbias = 0;
        long timezone = 0;
  localtime

  int   days[] = {

        yr -= 1900;

        _VALIDATE_RETURN_NOEXC(
            ( ( mo >= 1 ) && ( mo <= 12 ) ),
            EINVAL,
            ( ( __time64_t )( -1 ) )
        )
        _VALIDATE_RETURN_NOEXC(
            ( ( hr >= 0 ) && ( hr <= 23 ) ),
            EINVAL,
            ( ( __time64_t )( -1 ) )
        )
        _VALIDATE_RETURN_NOEXC(
            ( ( mn >= 0 ) && ( mn <= 59 ) ),
            EINVAL,
            ( ( __time64_t )( -1 ) )
        )
        _VALIDATE_RETURN_NOEXC(
            ( ( sc >= 0 ) && ( sc <= 59 ) ),
            EINVAL,
            ( ( __time64_t )( -1 ) )
        )
        _VALIDATE_RETURN_NOEXC(
            ( ( dy >= 1 ) && (
                (
                    (_days[mo] - _days[mo - 1]) >= dy) ||               // Make sure that the # of days is in valid range for the month
                    (_IS_LEAP_YEAR(yr) && mo == 2 && dy <= 29)  // Special case for Feb in a leap year
                )
            ),
            EINVAL,
            ( ( __time64_t )( -1 ) )
        )

        /*
         * Compute the number of elapsed days in the current year.
         */
        tmpdays = dy + _days[mo - 1];
        if ( _IS_LEAP_YEAR(yr) && (mo > 2) )
            tmpdays++;

        /*
         * Compute the number of elapsed seconds since the Epoch. Note the
         * computation of elapsed leap years would break down after 2100
         * if such values were in range (fortunately, they aren't).
         */
        tmptim = /* 365 days for each year */
                 (((__time64_t)yr - _BASE_YEAR) * 365

                 /* one day for each elapsed leap year */
                 + (__time64_t)_ELAPSED_LEAP_YEARS(yr)

                 /* number of elapsed days in yr */
                 + tmpdays)

                 /* convert to hours and add in hr */
                 * 24 + hr;

        tmptim = /* convert to minutes and add in mn */
                 (tmptim * 60 + mn)

                 /* convert to seconds and add in sec */
                 * 60 + sc;
        /*
         * Account for time zone.
         */
        __tzset();

        _ERRCHECK(_get_daylight(&daylight));
        _ERRCHECK(_get_dstbias(&dstbias));
        _ERRCHECK(_get_timezone(&timezone));

        tmptim += timezone;

        /*
         * Fill in enough fields of tb for _isindst(), then call it to
         * determine DST.
         */
        tb.tm_yday = tmpdays;
        tb.tm_year = yr;
        tb.tm_mon  = mo - 1;
        tb.tm_hour = hr;
        tb.tm_min  = mn;
        tb.tm_sec  = sc;
        if ( (dstflag == 1) || ((dstflag == -1) && daylight &&
                                _isindst(&tb)) )
            tmptim += dstbias;
        return(tmptim);
}
#endif

/* ------------------------------------------------------------------------ */
static int cnv_localtime(DWORD st_time, FILETIME *ft_time)
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
static void node_callback2(const TCHAR *path, GIT_NODE *g_node, void *param)
{
  D_NODE  *d_node = savedir(g_node->path);

  d_node->d_data[0] = g_node->mtime_hi;
}

/* ------------------------------------------------------------------------ */
static void node_callback(const TCHAR *path, GIT_NODE *g_node, void *param)
{
  D_NODE       *d_node = (D_NODE *)param;
  FIND_DATA     fd;
  HANDLE        hFind;
  struct _stat  st = {0};
  TCHAR         c0 = L' ';
  TCHAR         c1 = L' ';
  struct tm     tm1;
  struct tm     tm2;
  TCHAR         buf[MAX_PATH];

  _tcscpy_s(buf, MAX_PATH, path);
  PathAppend(buf, g_node->path);

  _tstat(buf, &st);



  _localtime64_s(&tm1, &st.st_mtime);
  _localtime32_s(&tm2, (const __time32_t *)&g_node->mtime_hi);

  _tprintf(L" %s\n", buf);
/*
  _tprintf(L"%d-%2d-%2d %02d:%02d:%02d(%d:%d:%d)\n%d-%2d-%2d %02d:%02d:%02d(%d:%d:%d)%s %s\n",
           tm1.tm_year + 1900,
           tm1.tm_mon,
           tm1.tm_mday,
           tm1.tm_hour,
           tm1.tm_min,
           tm1.tm_sec,
           tm1.tm_yday,
           tm1.tm_wday,
           tm1.tm_isdst,
           tm2.tm_year + 1900,
           tm2.tm_mon,
           tm2.tm_mday,
           tm2.tm_hour,
           tm2.tm_min,
           tm2.tm_sec,
           tm2.tm_yday,
           tm2.tm_wday,
           tm2.tm_isdst,
           path,
           g_node->path);
*/
/*
  _tprintf(L"%08X %08X\n%d.%02d.%02d %02d-%02d-%02d(%d:%d:%d)%s %s\n",
           (DWORD)st.st_mtime, 
           (DWORD)g_node->mtime_hi,
           tm2.tm_year + 1900,
           tm2.tm_mon  + 1,
           tm2.tm_mday,
           tm2.tm_hour,
           tm2.tm_min,
           tm2.tm_sec,
           tm2.tm_yday,
           tm2.tm_wday,
           tm2.tm_isdst,
           path,
           g_node->path);
*/
  d_node = savedir(d_node, g_node->path);

  if (d_node->subd)
    {
//    _tprintf(L"SUB: %s\n", d_node->subd->d_name);
    }

  if (d_node->next)
    {
//    _tprintf(L"NXT: %s %s\n", d_node->next->d_name, d_node->d_name);
    }

  FILETIME    local;
  SYSTEMTIME  systi;

  __time32_t  st_mtime;

  hFind = FindFirstFile(buf, &fd);

  if (hFind != INVALID_HANDLE_VALUE)
    {
      cnv_localtime(st.st_mtime, &fd.ftLastWriteTime);

      FileTimeToLocalFileTime(&fd.ftLastWriteTime, &local);
      FileTimeToSystemTime(&local, &systi);
/*
      _tprintf(L"%d.%02d.%02d %02d-%02d-%02d(%d:%d:%d)%s\n",
        systi.wYear,
        systi.wMonth,
        systi.wDay,
        systi.wHour,
        systi.wMinute,
        systi.wSecond,
        systi.wMilliseconds,
        systi.wDayOfWeek,
        0,
        buf);
*/
#if 0
      if (fd.ftLastWriteTime.dwLowDateTime || fd.ftLastWriteTime.dwHighDateTime)
        {
          if (!FileTimeToLocalFileTime(&fd.ftLastWriteTime, &local)
           || !FileTimeToSystemTime(&local, &systi))
            {
              printf("AHO\n");
              return;
            }
/*
          st_mtime = __loctotime64_t(systi.wYear,
                                     systi.wMonth,
                                     systi.wDay,
                                     systi.wHour,
                                     systi.wMinute,
                                     systi.wSecond,
                                     -1 );
*/
        }
      else
        {
          st_mtime = 0;
        }
#endif

//    _tprintf(L"%s\n", g_node->path);
//    _tprintf(L"%08X\n", fd.ftCreationTime.dwLowDateTime);
//    _tprintf(L"%X %X\n", st_mtime, g_node->mtime_hi);

      if ((DWORD)st.st_mtime != g_node->mtime_hi)
        {
          c0 = L'*';
        }
      else
        {
          c0 = L' ';
        }

      if ((DWORD)st.st_ctime != g_node->ctime_hi)
        {
          c1 = L'*';
        }
      else
        {
          c1 = L' ';
        }

      if (c0 == L'*' || c1 == L'*')
        {
          d_node->d_data[0] = 1;
        }

/*
      if (c0 == L'*' || c1 == L'*')
        {
          _tprintf(L"%c%08X %08X %c%08X %08X %s\n", c0, (DWORD)st.st_mtime, g_node->mtime_hi, c1, (DWORD)st.st_mtime, g_node->mtime_hi, g_node->path);

          if (d_node->next)
            {
              _tprintf(L"NXT: %s\n", d_node->next->d_name);
              d_node->next->git = 1;
            }
        }
*/
//    _tprintf(L"%08X\n", g_node->ctime_lo);
//    _tprintf(L"%08X\n", fd.ftCreationTime.dwHighDateTime);

//    fd.ftLastAccessTime;
//    fd.ftLastWriteTime;

      FindClose(hFind);
    }
/*
  FILETIME  ctime;
  FILETIME  atime;
  FILETIME  mtime;

  GetFileTime(
    HANDLE              hFile,
    LPFILETIME          lpCreationTime,
    LPFILETIME          lpLastAccessTime,
    LPFILETIME          lpLastWriteTime
*/

//_tprintf(L"%s %s\n", path, g_node->path);
/*
  struct _stat  st;
  D_NODE       *p_node = savedir(g_node->path);

  _tstat(g_node->path, &st);

  p_node->git = (DWORD)st.st_ctime;
*/
#if 0
  D_NODE *p_node = savedir(g_node->path);

  _tprintf(L"%X %X %X %X\n",
           g_node->ctime_hi,
           g_node->mtime_hi,
           g_node->ctime_lo,
           g_node->mtime_lo);

  struct _stat  st;

  _tstat(g_node->path, &st);

  _tprintf(L"%llX %llX\n",
           st.st_ctime,
           st.st_mtime);

  _tprintf(L"%s %d %X\n", g_node->path, g_node->inode, g_node->sha1[0]);
#endif
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

  if (size > 200)
    {
      _tprintf(L"%s %zd\n", entry->d_name, size);
    }

  D_NODE  *d_node = savedir(entry->d_name);

  for (int i = 0; i < 8; i++)
    {
      entry->d_data[i] = d_node->d_data[i];
    }

  _tprintf(L"%d %s %s\n", d_node->d_data[0], entry->d_name, d_node->d_name);


  if (entry->d_data[0] == 2)
    {
//    _tprintf(L"::%s\n", entry->d_name);
    }

  return DENT_FLT_RECURSION | DENT_FLT_MALLOC;
}

/* ------------------------------------------------------------------------ */
int scan(D_NODE *p, void *param)
{
  int  *iii = (int *)param;
  int   ret = 0;

//_tprintf(L"%s\n", p->d_name);

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
int git_node_callback(const TCHAR *path, GIT_NODE *g_node, void *param)
{
  D_NODE       *d_node = (D_NODE *)param;
  TCHAR         temp[MAX_PATH];
  int           i;
  DWORD         poyan;
  struct _stat  st;

  d_node = savedir(d_node, g_node->path);

  savedir_get_fullpath(temp, MAX_PATH, d_node);

  _tstat(temp, &st);

  if (g_node->mtime_hi != st.st_mtime)
    {
      poyan = 2;

      _tprintf(L"%d <<%s>>\n", poyan, temp);
    }
  else
    {
      poyan = 1;
    }

  i = 0;

  do
    {
      if (poyan > d_node->d_data[0])
        {
          _tprintf(L"%d %d %d[%s]\n", i, poyan, d_node->d_data[0], d_node->d_name);

          d_node->d_data[0] = poyan;
        }

      i++;

      d_node = d_node->parent;
    }
  while (d_node != (D_NODE *)param);

  if (poyan > d_node->d_data[0])
    {
      _tprintf(L"%d[%s]\n", d_node->d_data[0], d_node->d_name);

      d_node->d_data[0] = poyan;
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

//savedir_log(d_node, scan, &iii);

//total = scandir(path, &entries, filter);

  savedir_log(d_node, scan, &iii);

  _tprintf(L"t=%d\n", total);

#if 0

  TCHAR     path[MAX_PATH];
  dirent  **handle;
  int       total = 0;

  D_NODE   *d_node;
  D_NODE   *d_subd;

  HANDLE  hThread;
  DWORD   dwThreadId;

  hThread = CreateThread(NULL,          // Security attributes
                         0,             // Stack size
                         ThreadFunc,    // Thread function
                         NULL,          // Arguments passed to the thread function
                         0,             // Creation option (0 or CREATE_SUSPENDED)
                        &dwThreadId);   // Thread ID

  /* Main processing */

//GetCurrentDirectory(MAX_PATH, path);

  _tcscpy_s(path, MAX_PATH, GIT_PATH0);

//total = scandir(path, &handle, filter);

//d_node = savedir(path);

  _tprintf(L"ROOT: %s\n", path);

  d_node = savedir(path);

//total = scan_git_dir(path, node_callback2, d_node);

  printf("total %d\n", total);

  d_node = savedir(L"C:\\user\\test\\test01\\xgit.cpp");

  _tprintf(L"0: %s %d\n", d_node->d_name, d_node->d_data[0]);

  d_node = savedir(L"C:\\user\\test\\test01");

  _tprintf(L"1: %s %d\n", d_node->d_name, d_node->d_data[0]);

  d_node = savedir(L"C:\\user");

  _tprintf(L"2: %s %d\n", d_node->d_name, d_node->d_data[0]);

  d_node = savedir(L"D:\\AAA");

  total = scandir(path, &handle, filter);

  d_node = savedir();

  _tprintf(L"___ <%s>\n", d_node->d_name);
  d_node = d_node->subd;
  _tprintf(L"___ <%s>\n", d_node->d_name);
  d_node = d_node->next;
  _tprintf(L"___ <%s>\n", d_node->d_name);

/*
  for (int i = 0; i < total && d_node; i++)
    {
      _tprintf(L"%d/%d %d %s\n", i + 1, total, d_node->git, d_node->d_name);

      d_subd = d_node->subd;

      while (d_subd)
        {
          _tprintf(L"%s\n", d_subd->d_name);
          d_subd = d_subd->subd;
        }

      d_node = d_node->next;
    }
*/

  /* End processing */

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

  iii = 0;

  total = scandir(L"C:\\user", &handle, filter);

  _tcscpy_s(path, MAX_PATH, GIT_PATH4);

  _tprintf(L"shino: %s %d %d\n", path, total, MAX_PATH);

  savedir(L"C:\\user");

  savedir_log(scan, &iii);

  total = scan_git_dir(path, node_callback2);

  savedir_log(scan, &iii);

  printf("%d/%d\n", iii, total);

  total = scan_git_dir(L"C:\\user\\spresense-arduino-compatible", node_callback2);

  savedir_log(scan, &iii);

  printf("%d/%d\n", iii, total);
#endif

//total = scan_git_dir(L"C:\\user\\spresense\\sdk\\bsp\\board\\collet\\include", node_callback2);

//printf("%d\n", total);

  return 0;
}
