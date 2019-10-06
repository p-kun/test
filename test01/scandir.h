/*
 * --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --
 * Copyright(C) 2004 ****, Inc.
 *
 *  File Name: scandir.h
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
#ifndef _SCANDIR_H_
#define _SCANDIR_H_

#ifndef ENOMEM
#define ENOMEM				      -1
#endif
#define DENT_ENOMEM			    ENOMEM
#define DENT_FLT_MALLOC		  0x1
#define DENT_FLT_RECURSION	0x2
#define DENT_FLT_CANCEL		  (-1)

int  scandir( const TCHAR *,
              dirent ***,
              int ( * )( dirent* ) = NULL,
              int ( * )( dirent* ) = NULL,
              int ( * )( const dirent **, const dirent ** ) = NULL,
              void * = NULL );
void delete_scandir( dirent **top_namelist );

#endif // _SCANDIR_H_
