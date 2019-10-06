#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <sys/stat.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "User32.lib")

#define MAX_HICON   100



class HandleBackup
{
public:
  HandleBackup(int i)
  {
    size = 0;
  }

  HandleBackup()
  {
  }

  void add(HICON hIcon)
  {
    if (size >= MAX_HICON)
      {
        return;
      }

    int   i;

    for (i = 0; i < size; i++)
      {
        if (buf_hIcon[i] == hIcon)
          {
            ::DestroyIcon(hIcon);
            return;
          }
      }

    buf_hIcon[i] = hIcon;
    size++;
  }

  ~HandleBackup()
  {
    for (int i = 0; i < size; i++)
      {
        printf("%x\n", buf_hIcon[i]);
        ::DestroyIcon(buf_hIcon[i]);
      }
  }

private:
  static HICON   buf_hIcon[MAX_HICON];
  static int     size;

};

HICON   HandleBackup::buf_hIcon[MAX_HICON];
int     HandleBackup::size;



HandleBackup  _handle_backup(0);


int _tmain( int argc, TCHAR **argv )
{
  HandleBackup  bk;

  bk.add((HICON)199);
  
  return 0;
}
