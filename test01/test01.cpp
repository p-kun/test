/****************************************************************************
 * Included files
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <sys/stat.h>
#include <shlwapi.h>
#include "dirent.h"
#include "scandir.h"

#pragma comment(lib, "shlwapi.lib")

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define GIT_PATH  L"C:\\user\\spresense\\sdk\\bsp\\board\\collet\\include"
//#define GIT_PATH  L"C:\\ProgramData\\Apple Computer\\iTunes\\adi"
//#define GIT_PATH  L"C:\\Windows\\System32\\Hydrogen\\BakedPlugins\\Fx\\textures"

#define FIND_DATA WIN32_FIND_DATA

/****************************************************************************
 * Private types
 ****************************************************************************/

typedef struct
{
  DWORD   ctime_hi;
  DWORD   ctime_lo;
  DWORD   mtime_hi;
  DWORD   mtime_lo;
  DWORD   dev;
  DWORD   inode;
  DWORD   mode;
  DWORD   uid;
  DWORD   guid;
  DWORD   size;
  BYTE    sha1[20];
  TCHAR   path[MAX_PATH];
}
GIT_INDEX;

/****************************************************************************
 * Private data
 ****************************************************************************/

/****************************************************************************
 * Private functions
 ****************************************************************************/

// ==========================================================================
// -- 
// --------------------------------------------------------------------------
BYTE get_byte( const BYTE** pp_data )
{
  BYTE        data;
  const BYTE* bp = *pp_data;

  data = *bp;

  bp++;

  *pp_data = bp;

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
WORD get_word( const BYTE** pp_data )
{
  WORD  data = 0;

  data |= get_byte(pp_data);
  data <<= 8;
  data |= get_byte(pp_data);

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
DWORD get_dword( const BYTE** pp_data )
{
  DWORD data = 0;

  data |= get_byte(pp_data);
  data <<= 8;
  data |= get_byte(pp_data);
  data <<= 8;
  data |= get_byte(pp_data);
  data <<= 8;
  data |= get_byte(pp_data);

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
const BYTE *Header( const BYTE *bp, DWORD *total = NULL, DWORD *version = NULL )
{
  const BYTE  *ret = NULL;
  DWORD        _version;
  DWORD        _total;

  if ( get_byte( &bp ) != 'D' )
    {
      /**/
    }
  else if ( get_byte( &bp ) != 'I' )
    {
      /**/
    }
  else if ( get_byte( &bp ) != 'R' )
    {
      /**/
    }
  else if ( get_byte( &bp ) != 'C' )
    {
      /**/
    }
  else
    {
      _version = get_dword( &bp );
      _total   = get_dword( &bp );

      if ( version )
        {
          *version = _version;
        }

      if ( total )
        {
          *total = _total;
        }

      ret = bp;
    }

  return ret;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static const BYTE *Body( const BYTE *bp, const TCHAR *root, GIT_INDEX *g_idx )
{
  WORD         len;
  TCHAR        path[ MAX_PATH ];
  int          i;
  CHAR         cc[ MAX_PATH ];
  const BYTE  *top = bp;

  /* ctime  */

  g_idx->ctime_hi = get_dword( &bp );
  g_idx->ctime_lo = get_dword( &bp );

  /* mtime  */

  g_idx->mtime_hi = get_dword( &bp );
  g_idx->mtime_lo = get_dword( &bp );

  /* dev */

  g_idx->dev = get_dword( &bp );

  /* inode */

  g_idx->inode = get_dword( &bp );

  /* mode */

  g_idx->mode = get_dword( &bp );

  /* uid */

  g_idx->uid = get_dword( &bp );

  /* guid */

  g_idx->guid = get_dword( &bp );

  /* sha1 size */

  get_dword( &bp );

  /* sha1 */

  for ( i = 0; i < 20; i++ )
    {
      g_idx->sha1[i] = get_byte( &bp );
    }

  /* path length */

  len = get_word( &bp );

  /* path */

  for ( i = 0; i < len; i++ )
    {
      cc[ i ] = get_byte( &bp );
    }

  cc[ i ] = '\0';

  /* path terminate */

  get_byte( &bp );

  /* path conv */

  ::MultiByteToWideChar( CP_UTF8, 0U, cc, -1, path, MAX_PATH );

  _tcscpy_s(g_idx->path, MAX_PATH, root);

  PathAppend(g_idx->path, path);

  /* next point */

  bp = ( BYTE* )( top + ( ( bp - top + 7 ) & ~7 ) );

  return bp;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static void debug_git_index( GIT_INDEX *g_idx )
{
  /* ctime  */

  _tprintf( L"ctime = %08X\n", g_idx->ctime_hi );
  _tprintf( L"ctime = %08X\n", g_idx->ctime_lo );

  /* mtime  */

  _tprintf( L"mtime = %08X\n", g_idx->mtime_hi );
  _tprintf( L"mtime = %08X\n", g_idx->mtime_lo );

  /* dev */

  _tprintf( L"%08X\n", g_idx->dev );

  /* inode */

  _tprintf( L"%08X\n", g_idx->inode );

  /* mode */

  _tprintf( L"mode = %08X\n", g_idx->mode );

  /* uid */

  _tprintf( L"%08X\n", g_idx->uid );

  /* guid */

  _tprintf( L"%08X\n", g_idx->guid );

  /* sha1 */

  _tprintf( L"sha1:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",
    g_idx->sha1[0],
    g_idx->sha1[1],
    g_idx->sha1[2],
    g_idx->sha1[3],
    g_idx->sha1[4],
    g_idx->sha1[5],
    g_idx->sha1[6],
    g_idx->sha1[7],
    g_idx->sha1[8],
    g_idx->sha1[9],
    g_idx->sha1[10],
    g_idx->sha1[11],
    g_idx->sha1[12],
    g_idx->sha1[13],
    g_idx->sha1[14],
    g_idx->sha1[15],
    g_idx->sha1[16],
    g_idx->sha1[17],
    g_idx->sha1[18],
    g_idx->sha1[19]);

  _tprintf(L"[%s]\n", g_idx->path);
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
static HANDLE open_git_index(const TCHAR *input, TCHAR *git_path, size_t size)
{
  TCHAR   path[ MAX_PATH ];
  HANDLE  hFile = INVALID_HANDLE_VALUE;

  _tcscpy_s(git_path, size, input);

  do
    {
      _stprintf( path, L"%s\\%s", git_path, L".git\\index" );

      hFile = CreateFile( path,
                          GENERIC_READ,
                          FILE_SHARE_READ,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL );

      if ( hFile != INVALID_HANDLE_VALUE )
        {
          break;
        }

      if ( !PathRemoveFileSpec( git_path ) )
        {
          break;
        }
    }
  while ( 1 );

  return hFile;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
DWORD scan_git_index( const BYTE *bp, const TCHAR *git_path, dirent ***entry )
{
  DWORD         total = 0;
  DWORD         version;
  GIT_INDEX     g_idx;
  struct _stat  st;
  FIND_DATA     fd;

  bp = Header( bp, &total, &version );

  if ( !bp )
    {
      return 0;
    }

  for ( int i = 0; i < total; i++ )
    {
      bp = Body( bp, git_path, &g_idx );

      _tstat( g_idx.path, &st );

      if ((DWORD)st.st_mtime == g_idx.mtime_hi)
        {
          continue;
        }

      HANDLE  hFile = ::FindFirstFile( g_idx.path, &fd );

      if (hFile == INVALID_HANDLE_VALUE)
        {
          continue;
        }

      /* Add file info */

      
    }

  return total;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
int scan_git_dir( const TCHAR dir[], dirent ***entry )
{
  HANDLE    hFile;
  TCHAR     path[ MAX_PATH ];
  DWORD     size_lo;
  DWORD     size_hi;
  DWORD     size_ot;
  BYTE     *bp;
  DWORD     total = 0;

  hFile = open_git_index(dir, path, MAX_PATH);

  if ( hFile == INVALID_HANDLE_VALUE )
    {
      return 0;
    }

  size_lo = GetFileSize(hFile, &size_hi);

  if ( size_lo == INVALID_FILE_SIZE )
    {
      /* File size acquisition failure! */
    }
  else if ( size_hi > 0 )
    {
      /* File size is too large! */
    }
  else
    {
      bp = ( BYTE* )malloc( size_lo );

      if ( bp )
        {
          if ( ReadFile( hFile, bp, size_lo, &size_ot, NULL ) )
            {
              total = scan_git_index(bp, path, entry);
            }
        }

      free( bp );
    }

  CloseHandle( hFile );

  return total;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
int _tmain( int argc, TCHAR **argv )
{
  DWORD    total;
  dirent **namelist;

  total = scan_git_dir(GIT_PATH, &namelist);

  _tprintf( L"%d\n", total );

#if 0
  HANDLE        hFile;
  BYTE         *bp;
  BYTE         *top;
  DWORD         total;
  DWORD         version;
  DWORD         size;
  WORD          len;
  TCHAR         path[ MAX_PATH ];
  int           i;
  TCHAR*        cp;
  DWORD         size_lo;
  DWORD         size_hi;
  DWORD         size_ot;
  GIT_INDEX     g_idx;

  _tcscpy_s( git_path, MAX_PATH, GIT_PATH );


  _tprintf( L"Not git index path=\"%s\"\n", git_path );

  hFile = open_git_index(GIT_PATH, git_path, MAX_PATH);

  if ( hFile != INVALID_HANDLE_VALUE )
    {
      _tprintf( L"top=%s %p\n", git_path, hFile );

      size_lo = GetFileSize(hFile, &size_hi);

      _tprintf(L"%d\n", size_lo);

      if ( size_lo == INVALID_FILE_SIZE )
        {
          _tprintf( L"File size acquisition failure!\n" );
        }
      else if ( size_hi > 0 )
        {
          _tprintf( L"File size is too large!\n" );
        }
      else
        {
          _tprintf( L" FileSize = %u bytes\n", size_lo );

          top = bp = ( BYTE* )malloc( size_lo );

          if ( top )
            {
              if ( ReadFile( hFile, top, size_lo, &size_ot, NULL ) )
                {
                  bp = Header( top, &total, &version );
          
                  if ( bp )
                    {
                      for ( i = 0; i < total; i++ )
                        {
                          bp = Body( bp, git_path, &g_idx );

                          debug_git_index(&g_idx);
                        }
                    }
                }

              free( top );
            }
        }

      CloseHandle( hFile );
    }


  FILETIME  CreationTime;
  FILETIME  LastAccessTime;
  FILETIME  LastWriteTime;
  FILETIME  local;

  hFile = CreateFile( L"C:\\user\\Gottani\\README.md",
                      GENERIC_READ,
                      FILE_SHARE_READ,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL );

  if ( hFile != INVALID_HANDLE_VALUE )
    {
      if ( GetFileTime( hFile, &CreationTime, &LastAccessTime, &LastWriteTime ) )
        {
          _tprintf( L"README.md\n" );
          _tprintf( L"%08X\n", CreationTime.dwHighDateTime );
          _tprintf( L"%08X\n", CreationTime.dwLowDateTime );
          _tprintf( L"%08X\n", LastAccessTime.dwHighDateTime );
          _tprintf( L"%08X\n", LastAccessTime.dwLowDateTime );
          _tprintf( L"%08X\n", LastWriteTime.dwHighDateTime );
          _tprintf( L"%08X\n", LastWriteTime.dwLowDateTime );

          FileTimeToLocalFileTime( &CreationTime, &local );
          _tprintf( L"%08X\n", local.dwHighDateTime );
          _tprintf( L"%08X\n", local.dwLowDateTime );
          LocalFileTimeToFileTime( &CreationTime, &local );
          _tprintf( L"%08X\n", local.dwHighDateTime );
          _tprintf( L"%08X\n", local.dwLowDateTime );
        }

      CloseHandle( hFile );
    }

  _tprintf( L"\nEND.\n" );

#endif
  return 0;
}
// --------------------------------------------------------------------------
