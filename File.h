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

#ifndef FILE_H_
#define FILE_H_

#include <stdio.h>
#include <string.h>
#include "CStr.h"
#include "Vector.h"
// #include <vector>

#ifdef WIN32
#define _FS '\\'
#else
#define _FS '/'
#endif

class CFileMgr {
private:
    FILE *m_hFile;
    size_t m_iSize;
    char *m_strFile;
    char *m_strExt;

private:
    void _setExt();

public:
    CFileMgr() : m_hFile(NULL), m_iSize(0), m_strFile(NULL), m_strExt(NULL) { }
    CFileMgr(FILE *pFile) : m_hFile(pFile), m_iSize(0), m_strFile(NULL), m_strExt(NULL) {
        //get filesize
        fseek(m_hFile, 0, SEEK_END);
        m_iSize = ftell(m_hFile) + 1;
        fseek(m_hFile, 0, SEEK_SET);
    }
    CFileMgr(const char *strFile, const char *mode) : m_hFile(NULL), m_iSize(0), m_strFile(NULL), m_strExt(NULL) {
        size_t iLen = strlen(strFile);
        m_strFile = new char[iLen+1];
        memcpy(m_strFile, strFile, iLen);
        m_strFile[iLen] = 0;

        open(strFile, mode);
        _setExt();
    }
    ~CFileMgr() {
        close();
    }

    bool open(const char *strFile, const char *mode);
    void close();
    void seek(size_t iBytes, char orig = SEEK_CUR);
    size_t read(size_t iBytes, char *strBuf);
    bool write(const char *strBuf, size_t iBytes = 0);
    static const char *getFileFromPath(const char *strPath);
    static const char *getExtFromFile(const char *strFile);
    static bool getFilesInDir(const char *strDir, vector<CStr>& sFiles);
    static bool createDir(const char *strPath);
    static int del(const char *strFile);
    static bool existDir(const char *strDir);
    static bool Rename(const char *strOld, const char *strNew);
    static bool currentModulePath(char *strBuf, size_t iLen);
    static void log(const char *strTag, const char *strText);
    int del();
    const char *readLine();

    const char *getExt() {
        return m_strExt;
    }

    FILE *getHandle() {
        return m_hFile;
    }

    bool isOpened() const {
        return ( m_hFile != NULL );
    }

    const char *getFileName() const {
        return m_strFile;
    }
};


#endif /* FILE_H_ */
