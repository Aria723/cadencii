/*
 * CadenciiHelper.h
 * Copyright © 1997-2010 M.Kimura
 * Copyright © 2011 kbinani
 *
 * This file is part of cadencii.helper.
 *
 * cadencii.helper is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * cadencii.helper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#pragma once

using namespace System;

		namespace cadencii
		{
			namespace helper
			{

				public ref class Utils
				{
				public:
					static bool MountPointCreate( String ^mount_point_path/*LPCTSTR lpLinkName*/, String ^target/*LPCTSTR lpTargetName /*, LPSECURITY_ATTRIBUTES lpsa*/ )
					{
						union
						{
							REPARSE_DATA_BUFFER iobuf;
							TCHAR dummy[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
						} u;

						TCHAR *lpLinkName;
						TCHAR *lpTargetName;

						lpLinkName = toUnmanagedChar( mount_point_path );
						lpTargetName = toUnmanagedChar( target );

						LPSECURITY_ATTRIBUTES lpsa = NULL;
						HANDLE hFile;
						TCHAR namebuf[MAX_PATH + 4];
						DWORD cb;
						DWORD attr;
						BOOL isDirectory;

						attr = GetFileAttributes( lpTargetName );
						if( INVALID_FILE_ATTRIBUTES == attr ){
							return FALSE;
						}
						isDirectory = attr & FILE_ATTRIBUTE_DIRECTORY;

						//
						// リンク先をフルパスにする
						//
#ifdef UNICODE
                        wcscpy( namebuf, L"\\?\?\\" );
#else
	                    strcpy( namebuf, "\\?\?\\" );
#endif
						if( !GetFullPathName( lpTargetName, sizeof( namebuf ) / sizeof( namebuf[0] ) - 4, namebuf + 4, NULL ) ){
							return FALSE;
	                    }
#ifdef UNICODE
	                    if( !lstrcpyn( u.iobuf.MountPointReparseBuffer.PathBuffer, namebuf, MAXIMUM_REPARSE_DATA_BUFFER_SIZE ) )
#else
						if( !MultiByteToWideChar( CP_ACP, 0, namebuf, -1,
								u.iobuf.MountPointReparseBuffer.PathBuffer, MAXIMUM_REPARSE_DATA_BUFFER_SIZE ) )
#endif
						{
							return FALSE;
						}

						//
						// リンクファイルを作成
						//
						if( isDirectory ){
							if( !CreateDirectory( lpLinkName, lpsa ) ){
								return FALSE;
							}
							hFile = 
								CreateFile(
									lpLinkName, 
									GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_EXISTING,
									FILE_FLAG_BACKUP_SEMANTICS,
									NULL );
						}else{
							hFile = 
								CreateFile(
									lpLinkName,
									GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									lpsa,
									CREATE_NEW,
									0,
									NULL);
						}
						if( INVALID_HANDLE_VALUE == hFile ){
							return FALSE;
						}

						//
						// リパースデータを設定
						//
						u.iobuf.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
						u.iobuf.Reserved   = 0;
						u.iobuf.MountPointReparseBuffer.SubstituteNameOffset = 0;
						u.iobuf.MountPointReparseBuffer.SubstituteNameLength = wcslen( u.iobuf.MountPointReparseBuffer.PathBuffer ) * 2;
						u.iobuf.MountPointReparseBuffer.PrintNameOffset = u.iobuf.MountPointReparseBuffer.SubstituteNameLength + 2;
						u.iobuf.MountPointReparseBuffer.PrintNameLength = 0;
						memset(
							(char *)u.iobuf.MountPointReparseBuffer.PathBuffer + u.iobuf.MountPointReparseBuffer.SubstituteNameLength,
							0,
							4);
						u.iobuf.ReparseDataLength =
							8 +
							u.iobuf.MountPointReparseBuffer.PrintNameOffset +
							u.iobuf.MountPointReparseBuffer.PrintNameLength + 2;
						cb = 8 + u.iobuf.ReparseDataLength;
						if( !DeviceIoControl( hFile, FSCTL_SET_REPARSE_POINT,
										&u.iobuf, cb, NULL, 0, &cb, NULL ) ){
							CloseHandle( hFile );
							if( isDirectory ){
								RemoveDirectory( lpLinkName );
							}else{
								DeleteFile( lpLinkName );
							}
							//deletefunc(lpLinkName);
							return FALSE;
						}

						CloseHandle( hFile );
						return TRUE;
					}
                    
				private:
					Utils()
					{
					}

					static TCHAR *toUnmanagedChar( String ^str )
					{
#ifdef UNICODE
                        return (wchar_t *)System::Runtime::InteropServices::Marshal::StringToHGlobalUni( str ).ToPointer();
#else
                        return (char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( str ).ToPointer();
#endif
					}
				};

			}
		}
