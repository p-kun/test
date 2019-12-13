#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <windows.h>

typedef struct dirent
{
  size_t          size;
  int             d_cnt;
  struct dirent*  next;
  int             depth;
  int             state;
  void*           param;
  DWORD           dwFileAttributes;
  FILETIME        ftCreationTime;
  FILETIME        ftLastAccessTime;
  FILETIME        ftLastWriteTime;
  DWORD           nFileSizeHigh;
  DWORD           nFileSizeLow;
  DWORD           dwReserved0;
  DWORD           dwReserved1;
  DWORD           bookMark;
  DWORD           userData;
  HANDLE          hHandle;
  DWORD           d_data[8];
  TCHAR           cAlternateFileName[ 14 ];
  TCHAR           d_name[ 2 ];
}
dirent;

#include "scandir.h"

#endif // _DIRENT_H_
