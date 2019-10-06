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
//#define GIT_PATH  L"C:\\Windows\\System32\\Hydrogen\\BakedPlugins\\Fx\\textures"

/****************************************************************************
 * Private types
 ****************************************************************************/

/****************************************************************************
 * Private data
 ****************************************************************************/

TCHAR git_path[ MAX_PATH ];

/****************************************************************************
 * Private functions
 ****************************************************************************/

// ==========================================================================
// -- 
// --------------------------------------------------------------------------
BYTE get_byte( BYTE** pp_data )
{
  BYTE  data;
  BYTE* bp = *pp_data;

  data = *bp;

  bp++;

  *pp_data = bp;

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
WORD get_word( BYTE** pp_data )
{
  WORD  data = 0;
  BYTE* bp = *pp_data;

  data |= *bp;
  bp++;
  data <<= 8;
  data |= *bp;
  bp++;

  *pp_data = bp;

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
DWORD get_dword( BYTE** pp_data )
{
  DWORD data = 0;
  BYTE* bp = *pp_data;

  data |= *bp;
  bp++;
  data <<= 8;
  data |= *bp;
  bp++;
  data <<= 8;
  data |= *bp;
  bp++;
  data <<= 8;
  data |= *bp;
  bp++;

  *pp_data = bp;

  return data;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
BYTE *Header( BYTE *bp, DWORD *total = NULL, DWORD *version = NULL )
{
  BYTE* ret = NULL;
  DWORD _version;
  DWORD _total;

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

      _tprintf( L"ver=%d cnt=%d\n", _version, _total );
    }

  return ret;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
BYTE *Body( BYTE *bp )
{
  DWORD         size;
  WORD          len;
  TCHAR         path[ MAX_PATH ];
  TCHAR         temp[ MAX_PATH ] = { L'\0' };
  int           i;
  CHAR          cch[ MAX_PATH ];
  BYTE          b;
  BYTE*         top = bp;
  struct _stat  st;

  /* ctime  */

  _tprintf( L"ctime = %08X\n", get_dword( &bp ) );
  _tprintf( L"ctime = %08X\n", get_dword( &bp ) );

  /* mtime  */

  _tprintf( L"mtime = %08X\n", get_dword( &bp ) );
  _tprintf( L"mtime = %08X\n", get_dword( &bp ) );

  /* dev */

  _tprintf( L"%08X\n", get_dword( &bp ) );

  /* inode */

  _tprintf( L"%08X\n", get_dword( &bp ) );

  /* mode */

  _tprintf( L"mode = %08X\n", get_dword( &bp ) );

  /* uid */

  _tprintf( L"%08X\n", get_dword( &bp ) );

  /* guid */

  _tprintf( L"%08X\n", get_dword( &bp ) );

  /* size */

  size = get_dword( &bp );

  for ( i = 0; i < 5; i++ )
    {
      _tprintf( L"sha1:%02d:%08X\n", i, get_dword( &bp ) );
    }

  len = get_word( &bp );

  _tprintf( L"len = %d\n", len );

  for ( i = 0; i < len; i++ )
    {
      b = get_byte( &bp );
      path[ i ] = b;
      cch [ i ] = b;
    }
  path[ i ] = '\0';
  cch[ i ] = '\0';
  path[ i ] = get_byte( &bp );

  _tprintf( L"path = %s\n", path );
  printf( "ccch = %s\n", cch );

  bp = ( BYTE* )( top + ( ( bp - top + 7 ) & ~7 ) );

  _tprintf( L"bp = %p %p\n", top, bp );

  size = ::MultiByteToWideChar( CP_UTF8, 0U, cch, -1, path, MAX_PATH );

  _tprintf( L"2path = %s %d\n", path, size );

  PathAppend( temp, git_path );
  PathAppend( temp, path );

  _tstat( temp, &st );

  _tprintf(L"[%s]\n", temp);

  _tprintf( L"temp %08X\n", ( DWORD )st.st_ctime );
  _tprintf( L"temp %08X\n", ( DWORD )st.st_mtime );

  return bp;
}
// --------------------------------------------------------------------------


// ==========================================================================
// -- 
// --------------------------------------------------------------------------
int _tmain( int argc, TCHAR **argv )
{
  HANDLE        hFile;
  DWORD         dwFileSize;
  BYTE*         bp;
  BYTE*         top;
  DWORD         readFileSize;
  DWORD         total;
  DWORD         version;
  DWORD         size;
  WORD          len;
  TCHAR         path[ MAX_PATH ];
  int           i;
  TCHAR*        cp;

  _tcscpy_s( git_path, MAX_PATH, GIT_PATH );

  do
    {
      _stprintf( path, L"%s\\%s", git_path, L".git\\index" );

      _tprintf( L"%s\n", path );

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


  _tprintf( L"top=%s\n", git_path );

  if ( hFile != INVALID_HANDLE_VALUE )
    {
      dwFileSize = GetFileSize(hFile, NULL);

      if ( dwFileSize != ~0 )
        {
          _tprintf( L" FileSize = %u bytes\n", dwFileSize );

          top = bp = ( BYTE* )malloc( dwFileSize );

          if ( bp )
            {
              if ( ReadFile( hFile, bp, dwFileSize, &readFileSize, NULL ) )
                {
                  bp = Header( bp, &total, &version );

                  if ( bp )
                    {
                      for ( i = 0; i < total; i++ )
                        {
                          bp = Body( bp );
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

  return 0;
}
// --------------------------------------------------------------------------
