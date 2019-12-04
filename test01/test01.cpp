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

  _tstat(g_node->path, &st);

  _localtime64_s(&tm1, &st.st_mtime);
  _localtime32_s(&tm2, (const __time32_t *)&g_node->mtime_hi);

/*
  _tprintf(L"%d-%2d-%2d %02d:%02d:%02d(%d:%d:%d)\n%d-%2d-%2d %2d:%02d:%02d(%d:%d:%d)%s\n",
           tm1.tm_year + 1900,
           tm1.tm_mon,
           tm1.tm_yday,
           tm1.tm_hour,
           tm1.tm_min,
           tm1.tm_sec,
           tm1.tm_mday,
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
           g_node->path);
*/
  _tprintf(L"%08X %08X %d.%02d.%02d %02d-%02d-%02d(%d:%d:%d)%s\n",
           (DWORD)st.st_mtime, 
           (DWORD)g_node->mtime_hi,
           tm2.tm_year + 1900,
           tm2.tm_mon,
           tm2.tm_mday,
           tm2.tm_hour,
           tm2.tm_min,
           tm2.tm_sec,
           tm2.tm_yday,
           tm2.tm_wday,
           tm2.tm_isdst,
           g_node->path);

  d_node = savedir(d_node, g_node->path);

  if (d_node->subd)
    {
//    _tprintf(L"SUB: %s\n", d_node->subd->d_name);
    }

  if (d_node->next)
    {
//    _tprintf(L"NXT: %s %s\n", d_node->next->d_name, d_node->d_name);
    }

#ifdef _KERNELX
  FILETIME    local;
#endif
  SYSTEMTIME  systi;

  __time32_t  st_mtime;

  hFind = FindFirstFile(g_node->path, &fd);

  if (hFind != INVALID_HANDLE_VALUE)
    {
      if (fd.ftLastWriteTime.dwLowDateTime || fd.ftLastWriteTime.dwHighDateTime)
        {
#ifndef _KERNELX
          if (!FileTimeToSystemTime(&fd.ftLastWriteTime, &systi)
           || !SystemTimeToTzSpecificLocalTime(NULL, &systi, &systi))
#else
          if (!FileTimeToLocalFileTime(&fd.ftLastWriteTime, &local)
           || !FileTimeToSystemTime(&local, &systi))
#endif
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


//    _tprintf(L"%s\n", g_node->path);
//    _tprintf(L"%08X\n", fd.ftCreationTime.dwLowDateTime);
      _tprintf(L"%X %X\n", st_mtime, g_node->mtime_hi);

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
          d_node->git = 1;
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
//_tprintf(L"%s\n", entry->d_name);
  return DENT_FLT_RECURSION;
}

int iii = 0;

int scan(D_NODE *p, void *param)
{
  int  *iii = (int *)param;
  int   ret = 0;

//_tprintf(L"%s\n", p->d_name);

  (*iii)++;

  if (*iii == 100000)
    {
      ret = -1;
    }

  return ret;
}

/****************************************************************************
 * Public functions
 ****************************************************************************/

int _tmain( int argc, TCHAR **argv )
{
  TCHAR     path[MAX_PATH];
  dirent  **handle;
  int       total;
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

  GetCurrentDirectory(MAX_PATH, path);

//_tcscpy_s(path, MAX_PATH, GIT_PATH0);

//total = scandir(path, &handle, filter);

//d_node = savedir(path);

  _tprintf(L"ROOT: %s\n", path);

  d_node = savedir(path);

  total = scan_git_dir(path, node_callback, d_node);

  printf("total %d\n", total);

  d_node = savedir(L"C:\\user\\test\\test01\\xgit.cpp");

  _tprintf(L"0: %s %d\n", d_node->d_name, d_node->git);

  d_node = savedir(L"C:\\user\\test\\test01");

  _tprintf(L"1: %s %d\n", d_node->d_name, d_node->git);

  d_node = savedir(L"C:\\user");

  _tprintf(L"2: %s %d\n", d_node->d_name, d_node->git);

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

  savedir_log(scan, &iii);

  printf("%d\n", iii);

  return 0;
}
