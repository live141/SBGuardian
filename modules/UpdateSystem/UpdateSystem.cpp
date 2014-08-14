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

#include "UpdateSystem.h"
#include "../../version.h"

void CUpdateSystem::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;
	m_iThreadId = 0;
	m_flLastCheck = -UPDATE_INTERVALL;
	m_bReload = false;

	char buf[512];
	CFileMgr::currentModulePath(buf, 512);
	// printf("%s\n", buf);
	const char *pChar = strrchr(buf, _FS);
	size_t iLen = pChar-buf+1;
	m_strPath = new char[iLen+1];
	memcpy(m_strPath, buf, iLen);
	m_strPath[iLen] = 0;
	iLen = strlen(++pChar);
	m_strFile = new char[iLen+1];
	memcpy(m_strFile, pChar, iLen);
	m_strFile[iLen] = 0;
	printf("[SBG]: Loaded File: %s%s\n", m_strPath, m_strFile);

	// delete old tmp
	CBuf bufTmp;
	bufTmp.thisFormat("%s" S_TMPNAME, m_strPath);
	CFileMgr::del(bufTmp.data());

	setEnabled(true);
}

void CUpdateSystem::unload() {
	setEnabled(false);

	if( m_strFile != NULL )
		delete m_strFile;
	if( m_strPath != NULL )
		delete m_strPath;
	m_strPath = NULL;
	m_strFile = NULL;
}

void CUpdateSystem::onFrame() {
	if( m_bReload ) {
		m_bReload = false;
		printf("Reloading SBGuardian...\n");
		m_pEngine->reloadPlugin(CStr::format("%s%s", m_strPath, m_strFile));
	}

	if( m_pEngine->time() < m_flLastCheck + UPDATE_INTERVALL || !isEnabled() )
		return;
	m_flLastCheck = m_pEngine->time();

	if( !CThreads::newThread(&m_iThreadId, &CUpdateSystem::dlRev, this) )
		printf("[SBG]: Could not look for updates.\n");
}

void *CUpdateSystem::dlRev(void *argv) {
	CUpdateSystem *pUpdateSystem = (CUpdateSystem *) argv;
	// get server first
	const char strUrl[] = URL_REV;
	// filter http://
	const char *pSlash = NULL;
	const char *pUrl = strUrl;
	if( !strncmp(strUrl, "http://", 7) )
		pUrl += 7;
	pSlash = strchr(pUrl, '/');
    if( pSlash == NULL )
    	return NULL; // no filepath to download
	size_t iLenServer = pSlash - pUrl;
	char *strServer = new char[iLenServer+1];
    memcpy(strServer, pUrl, iLenServer);
    strServer[iLenServer] = 0;

    // get filepath
    const char *pFilePath = pSlash;

    // printf("%s %s\n", strServer, pFilePath);

	CSockTCP sock(strServer, 80);
	if( !sock._connect() ) {
		printf("[SBG]: Could not connect to host.\n");
		return NULL;
	}

	if( !sock.isReadyWrite(2) )
		return NULL;

	sock.sendMsg(CStr::format("GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", pFilePath, strServer));
	// sock.sendMsg("GET /sbguardian2/rev.txt HTTP/1.1\r\nHost: http://storage.steambans.com\r\n\r\n");

	delete[] strServer;
	strServer = NULL;

	// parse header
	const char *strLine = sock.readLine();
	if( strLine == NULL ) {
	    printf("[SBG]: Error: no data received.\n");
	    return false;
	}

	int iStatus = 0;
	sscanf(strLine, "HTTP/1.1 %d", &iStatus);

	if( iStatus != 200 ) { // not OK
	    switch( iStatus ) {
	        printf("[SBG]: HTTP Status not OK.\n");
	        case 404: // file not found
	            printf("[SBG]: HTTP-Error: File not found.\n");
	            break;
	    }

	    // get the rest
	    while( sock.isReadReady(2) ) {
	    	sock.read();
	    }

    return false;
	}

    size_t iLen = 0;

    while( 1 ) {
        strLine = sock.readLine();

        if( strLine == NULL )
            return false; // should not happen
        if( strLine[1] == '\n' ) // \r\n
            break;
        if( strstr(strLine, "Content-Length: ") != NULL ) {
            iLen = atoi(strLine+16);
        }
    }

    if( iLen == 0 )
        return NULL;

    size_t iBitsRead = sock.read();
    if( iLen != iBitsRead )
    	return NULL;

    pUpdateSystem->m_iHeadRev = atoi(sock.data());
    printf("[SBG]: HeadRev: %d\n", pUpdateSystem->m_iHeadRev);


    if( pUpdateSystem->m_iHeadRev <= REV )
    	return NULL;

    CFileMgr::log(pUpdateSystem->getName(), "A new version is available");
    pUpdateSystem->m_pEngine->printHud("[SBG]: A new version available...");

#ifdef STANDALONE
    // hl1 standalone cant download and reload
    return NULL;
#endif


	CFileMgr::log(pUpdateSystem->getName(), "Downloading the new version...");
	pUpdateSystem->m_pEngine->printHud("[SBG]: Downloading the new version...");

	if( CSockTCP::download(DL_FILE, CStr::format("%s", pUpdateSystem->m_strPath)) ) {
    	printf("[SBG]: downloaded the update.\n");
    	CFileMgr::log(pUpdateSystem->getName(), "Download was successfully.");
        CBuf bufDl, bufNew, bufTmp;
        bufDl.thisFormat("%s%s", pUpdateSystem->m_strPath, _DL_FILE _EXT);
        bufNew.thisFormat("%s%s", pUpdateSystem->m_strPath, pUpdateSystem->m_strFile);
        bufTmp.thisFormat("%s" S_TMPNAME, pUpdateSystem->m_strPath);
    	CFileMgr::Rename(bufNew.data(), bufTmp.data());
    	CFileMgr::Rename(bufDl.data(), bufNew.data());
    	pUpdateSystem->m_bReload = true;
    }
    else {
    	printf("[SBG]: Could not download the files.\n");
    	CFileMgr::log(pUpdateSystem->getName(), "Download failed.");
    }
	return NULL;
}

void CUpdateSystem::onMapChange() {
	m_flLastCheck = 0.0;
	m_iThreadId = 0;
}

bool CUpdateSystem::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "updatesystem") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("UpdateSystem is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		setEnabled(atoi(m_pEngine->argv(2)));
	}

	return true;
}
