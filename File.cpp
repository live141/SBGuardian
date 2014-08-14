/**
* SBGuardian v2.0
* Copyright (c) 2011 Fabian Fischer
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#include "File.h"
#include "string.h"
#include <errno.h>
#include "Socket.h"
#include "Time.h"
#include "CStr.h"
#include <string>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>
#endif

#ifdef WIN32
extern "C" __declspec(dllexport) LPVOID __ImageBase; // for getting modulefilename
#endif

bool CFileMgr::open(const char *strFile, const char *mode) {
    m_hFile = fopen(strFile, mode);
    if( m_hFile == NULL )
        return false;

    //get filesize
	size_t curPos = ftell(m_hFile);
    fseek(m_hFile, 0, SEEK_END);
    m_iSize = ftell(m_hFile) + 1;
    fseek(m_hFile, curPos, SEEK_SET);
    return true;
}

void CFileMgr::_setExt() {
    if( m_strFile == NULL )
        return;

    size_t iLen = strlen(m_strFile);
    if( iLen < 3 || m_strFile[iLen-1] == '.' )
        return;

    for( size_t i = iLen-2; i >= 1; i-- ) {
        const char *pExt = ( m_strFile[i] == '.' )?m_strFile+i+1:NULL;
        if( pExt != NULL ) {
            m_strExt = (char *) pExt;
            break;
        }
    }
}

int CFileMgr::del(const char* strFile) {
    int iRes = remove(strFile);
    errno = 0;
    if( iRes != 0 ) {
        // perror("Could not Delete file");
    }

    return iRes;
}

int CFileMgr::del() {
    close();
    int iRes = del(m_strFile);
    errno = 0;
    if( iRes != 0 ) {
        // perror("Could not Delete file");
    }

    return iRes;
}

const char *CFileMgr::getExtFromFile(const char* strFile) {
    if( strFile == NULL )
        return NULL;

    size_t iLen = strlen(strFile);
    if( iLen < 3 || strFile[iLen-1] == '.' )
        return NULL;

    static CStr strExt;
    strExt.clear();

    for( size_t i = iLen-2; i >= 1; i-- ) {
        const char *pExt = ( strFile[i] == '.' )?strFile+i+1:NULL;
        if( pExt != NULL ) {
            strExt = pExt;
            break;
        }
    }

    return strExt.data();
}

void CFileMgr::close() {
   if( m_hFile != NULL ) {
       fclose(m_hFile);
   }

   if( m_strFile != NULL ) {
       delete[] m_strFile;
   }

   m_hFile = NULL;
   m_strFile = NULL;
   m_iSize = 0;
   m_strExt = NULL;
}

void CFileMgr::seek(size_t iBytes, char orig) {
    if( m_hFile == NULL )
        return;
    fseek(m_hFile, iBytes, orig);
}

size_t CFileMgr::read(size_t iBytes, char *strBuf) {
    if( m_hFile == 0 )
        return -1;
    return fread(strBuf, 1, iBytes, m_hFile);
}

bool CFileMgr::write(const char* strBuf, size_t iBytes) {
    if( m_hFile == 0 )
        return false;

    if( iBytes == 0 )
        iBytes = strlen(strBuf);

    if( fwrite(strBuf, 1, iBytes, m_hFile) != iBytes )
        return false;
    return true;
}

const char *CFileMgr::getFileFromPath(const char* strPath) {
    static std::string strFile;
    strFile.clear();

    if( strPath == NULL )
        return NULL;
    size_t iLen = strlen(strPath);
    if( iLen < 3 ) // at least a/b
        return NULL;
    for( size_t i = iLen-2; i >= 1; i-- ) {
        if( strPath[i] == '/' || strPath[i] == '\\' ) {
            strFile = strPath+i+1;
            break;
        }
    }

    return strFile.data();
}

const char *CFileMgr::readLine() {
    if( m_hFile == NULL || (unsigned) ftell(m_hFile) >= m_iSize-1 )
        return NULL;

    static CStr s;
    s.clear();
    char c = 0 ;
    fread(&c, 1, 1, m_hFile);
    s = c;
    if( c == '\n' )
        return s.data();
    size_t i = ftell(m_hFile);
    for( ; i < m_iSize; i++ ) {
        fread(&c, 1, 1, m_hFile);
        s += c;
        if( c == '\n' )
            break;
    }

    return s.data();
}

bool CFileMgr::createDir(const char* strPath) {
#ifdef WIN32
	return ( _mkdir(strPath) == 0 );
#else
    return ( mkdir(strPath, S_IRWXU) == 0 );
#endif
}

bool CFileMgr::currentModulePath(char *strBuf, size_t iLen) {
#ifdef WIN32
	GetModuleFileName((HMODULE) &__ImageBase, strBuf, iLen);
#else
	Dl_info dlinfo;
	dladdr((const void *) &CFileMgr::currentModulePath, &dlinfo);
	if( dlinfo.dli_saddr == NULL )
		return false;
	snprintf(strBuf, iLen, "%s", dlinfo.dli_fname);
#endif
		return true;
}

bool CFileMgr::Rename(const char *strOld, const char *strNew) {
	return ( rename(strOld, strNew) == 0 );
}

bool CFileMgr::getFilesInDir(const char* strDir, vector<CStr>& sFiles) {
#ifdef WIN32
#else
    DIR *pDir = NULL;
    dirent *pDirEnt = NULL;

    if( (pDir = opendir(strDir)) == NULL )
        return false;

    while( (pDirEnt = readdir(pDir)) != NULL ) {
        sFiles.push_back(CStr(pDirEnt->d_name));
    }

    closedir(pDir);
#endif
    return true;
}

void CFileMgr::log(const char *strTag, const char *strText) {
	static char strSbgPath[512];
	static bool bPath = false;
	if( !bPath ) {
		CFileMgr::currentModulePath(strSbgPath, sizeof(strSbgPath));
		const char *pSlash = strrchr(strSbgPath, _FS);

		if( pSlash == NULL )
			return;
		strSbgPath[pSlash-strSbgPath+1] = 0;
		bPath = true;
	}
	
	CBuf buf;
	CTime time;
	CFileMgr File(buf.thisFormat("%s%s", strSbgPath, "sbg.log"), "a");
	printf("%s %s [%s]: %s\n", time.getDate(), time.getTime(), strTag, strText);
	buf.thisFormat("%s %s [%s]: %s\n", time.getDate(), time.getTime(), strTag, strText);
	File.write(buf.data(), buf.len());
	File.close();
}

bool CFileMgr::existDir(const char* strDir) {
#ifdef WIN32
	return true;
#else
    struct stat s;
    return ( stat(strDir, &s) == 0 );
#endif
}
