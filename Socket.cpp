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

#include "Socket.h"
#ifndef WIN32
#include <unistd.h>
#endif
#include <errno.h>
#include "File.h"
// #include <string>

CSockTCP::~CSockTCP() {
    disconnect();
}

void CSockTCP::disconnect() {
    m_iFlags &= ~FL_CONNECTED;
#ifdef WIN32
	closesocket(m_iSock);
	WSACleanup();
#else
    close(m_iSock);
#endif
    m_iSock = 0;
}

bool CSockTCP::_connect() {
    m_iFlags = 0;

#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(1,2), &wsa);
#endif

    m_iSock = socket(AF_INET, SOCK_STREAM, 0);

    if( m_iSock == -1 ) {
        printf("Error: Could not init socket...\n");
        return false;
    }

    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_addr.s_addr = m_HostInfo.getIP();
    sockAddr.sin_port = htons(m_HostInfo.getPort());
    sockAddr.sin_family = AF_INET;
    errno = 0;
    if( connect(m_iSock, (sockaddr *) &sockAddr, sizeof(sockAddr)) == -1 ) {
        printf("Error: connection failed.\n");
        switch(errno) {
            case ECONNREFUSED:
                printf("Connection refused.\n");
                break;
            case EISCONN:
                printf("Already connected.\n");
                break;
            case ETIMEDOUT:
                printf("Timeout...\n");
                break;
        }
        disconnect();
        return false;
    }

    m_iFlags |= FL_CONNECTED;
    return true;
}

void CSockTCP::sendMsg(const char *strMsg, size_t iLen) {
    if( !isConnected() )
        return;

    if( iLen == 0 ) {
        iLen = strlen(strMsg);
    }

    if( send(m_iSock, strMsg, iLen, 0) != iLen ) {
        printf("Error: Could not send all data...\n");
        return;
    }
}

size_t CSockTCP::read() {
    if( !isConnected() )
        return -1;

    size_t iLen = 0;
    memset(&m_strBuf, 0, sizeof(m_strBuf));
    iLen = recv(m_iSock, m_strBuf, sizeof(m_strBuf), 0);

    if( iLen == -1 ) {
        printf("Error: Could not read data...\n");
    }

    if( iLen == 0 ) {
        printf("Other end disconnected...\n");
        disconnect();
    }

    m_iLenBuf = iLen;
    return iLen;
}

const char *CSockTCP::readLine() {
    if( !isConnected() )
        return NULL;

    static CStr s;
    s.clear();
    while( 1 ) {
        char c = 0;
        int res = recv(m_iSock, &c, 1, 0);
        if( res == 0 ) {
            disconnect();
            break;
        }
        else if( res == -1 ) {
            break;
        }

        s += c;
        if( c == '\n' )
            break;
    }

    return s.data();
}

bool CSockTCP::isReadReady(unsigned int sec_Timeout) {
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(m_iSock, &fd);
    timeval time;
    time.tv_usec = 0;
    time.tv_sec = sec_Timeout;
    select(m_iSock+1, &fd, NULL, NULL, &time);
    if( FD_ISSET(m_iSock, &fd) )
        return true;

    return false;
}

bool CSockTCP::isReadyWrite(unsigned int sec_Timeout) {
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(m_iSock, &fd);
    timeval time;
    time.tv_usec = 0;
    time.tv_sec = sec_Timeout;
    select(m_iSock+1, NULL, &fd, NULL, &time);
    if( FD_ISSET(m_iSock, &fd) )
        return true;

    return false;
}

void CSockTCP::setKeepAlive(bool keepAlive) {
    int flags = (keepAlive)?1:0;
    if( setsockopt(m_iSock, SOL_SOCKET, SO_KEEPALIVE, (char *) &flags, 4) == 0 )
        m_bKeepAlive = flags;
}

void CSockTCP::setNonBlocking(bool nonBlocking) {
#if defined WIN32
	ULONG nonblocking = 1;
	ioctlsocket(m_iSock, FIONBIO, &nonblocking);
#else
    int flags = fcntl(m_iSock, F_GETFL);
    if( nonBlocking ) {
        flags |= O_NONBLOCK;
        m_bNonBlocking = true;
    }
    else {
        flags &= ~O_NONBLOCK;
        m_bNonBlocking = false;
    }
    fcntl(m_iSock, F_SETFL, flags);
#endif
}

bool CSockTCP::download(const char *strUrl, const char *strDir) {
    if( strUrl == NULL || strDir == NULL ) {
        return false;
    }

    // get server first
    const char *pUrl = strUrl;
    if( !strncmp(pUrl, "http://", 7) )
    	pUrl += 7;
    const char *pSlash = strstr(pUrl, "/");
    if( pSlash == NULL )
        return false; // no filepath to download
    size_t iLenServer = pSlash - pUrl;
    char *strServer = new char[iLenServer+1];
    memcpy(strServer, pUrl, iLenServer);
    strServer[iLenServer] = 0;

    // get filepath
    const char *pFilePath = pSlash;

    printf("Download %s%s\n", strServer, pFilePath);

    CSockTCP sock(strServer, 80);
    if( !sock._connect() )
        return false;

    size_t iLen = iLenServer + strlen(pFilePath) + 25 + 1;
    char *strBuf = new char[iLen+1];
    snprintf(strBuf, iLen+1, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", pFilePath, strServer);
    sock.sendMsg(strBuf);

    delete[] strServer;
    strServer = NULL;
    delete[] strBuf;
    strBuf = NULL;

    // parse header
    const char *strLine = sock.readLine();
    if( strLine == NULL ) {
        printf("Error: no data received.\n");
        return false;
    }
    int iStatus = 0;
    sscanf(strLine, "HTTP/1.1 %d", &iStatus);

    if( iStatus != 200 ) { // not OK
        switch( iStatus ) {
            printf("HTTP Status not OK.\n");
            case 404: // file not found
                printf("HTTP-Error: File not found: %s.\n", strUrl);
                break;
        }

        // get the rest
        while( sock.isReadReady(2) ) {
            sock.read();
        }

        return false;
    }

    // check if folder exists, if not create
    if( !CFileMgr::existDir(strDir) ) {
        if( !CFileMgr::createDir(strDir) ) {
            printf("Error: Could not create Folder.");
            return false;
        }
    }

    iLen = 0;

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
        return false;

    CStr sPath = strDir;
    if( strDir[strlen(strDir)-1] != '/' && strDir[strlen(strDir)-1] != '\\' ) {
#ifdef WIN32
    	sPath += '\\';
#else
    	sPath += '/';
#endif
    }
    sPath += CFileMgr::getFileFromPath(pFilePath);

    CFileMgr File(sPath.data(), "wb");
    printf("%s\n", sPath.data());
    printf("[SBG]: Downloading %s to %s\n", CFileMgr::getFileFromPath(pFilePath), strDir);

    // content
    size_t iBytes = 0;
    do {
        size_t nBytes = sock.read();
        if( nBytes == -1 || nBytes == 0 )
            break;
        iBytes += nBytes;

        File.write(sock.data(), nBytes);

    } while( iBytes < iLen );
    printf("\n");
    return true;
}

bool CSockTCP::checkFileOnSite(const char* strUrl) {
        if( strUrl == NULL )
        return false;

    // get server first
    const char *pSlash = strstr(strUrl, "/");
    if( pSlash == NULL )
        return false; // no filepath to download
    size_t iLenServer = pSlash - strUrl;
    char *strServer = new char[iLenServer+1];
    memcpy(strServer, strUrl, iLenServer);
    strServer[iLenServer] = 0;

    // get filepath
    const char *pFilePath = pSlash;


    CSockTCP sock(strServer, 80);
    if( !sock._connect() )
        return false;

    size_t iLen = iLenServer + strlen(pFilePath) + 25 + 1;
    char *strBuf = new char[iLen+1];
    snprintf(strBuf, iLen+1, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", pFilePath, strServer);
    sock.sendMsg(strBuf);

    delete[] strServer;
    strServer = NULL;
    delete[] strBuf;
    strBuf = NULL;

    // parse header
    const char *strLine = sock.readLine();
    if( strLine == NULL )
        return false;
    int iStatus = 0;
    sscanf(strLine, "HTTP/1.1 %d", &iStatus);

    bool bStatus = false;

    if( iStatus != 200 ) { // not OK
        switch( iStatus ) {
            case 404: // file not found
                printf("HTTP-Error: File not found.\n");
                break;
        }
    }

    else
        bStatus = true;

        // get rest
        while( sock.isReadReady(2) ) {
            sock.read();
    }

    return bStatus;
}

