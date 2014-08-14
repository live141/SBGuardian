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

#ifndef SOCKET_H_
#define SOCKET_H_

#include "HostInfo.h"
#ifdef WIN32
#include <winsock.h>
#define _SOCKET SOCKET
#define MSG_NOSIGNAL 0
#else
#define _SOCKET int
#include <fcntl.h>
#endif
#include <string.h>

#define FL_CONNECTED 1

class CSocket {
protected:
    _SOCKET m_iSock;
    CHostInfo m_HostInfo;

public:
    CSocket() : m_iSock(0) { }
    virtual ~CSocket() { }
    virtual bool _connect() = 0;
    virtual void disconnect() = 0;
    virtual void setNonBlocking(bool nonBlocking) = 0;
    virtual void sendMsg(const char *strMsg, size_t iLen = 0) = 0;
    virtual size_t read() = 0;
    virtual bool isReadReady(unsigned int sec_Timeout) = 0;
    virtual bool isReadyWrite(unsigned int sec_Timeout) = 0;

    CHostInfo& getHostInfo() {
    	return m_HostInfo;
    }
};

class CSockTCP : public CSocket {
private:
    bool m_bKeepAlive;
    bool m_bNonBlocking;
    char m_strBuf[1024];
    int m_iFlags;
    size_t m_iLenBuf;

public:
    CSockTCP()  : m_bKeepAlive(false), m_bNonBlocking(false),
    m_iFlags(0), m_iLenBuf(0) { }
    CSockTCP(const CSocket& orig);
    CSockTCP(const char *strAddr, unsigned int iPort) : m_bKeepAlive(false), m_bNonBlocking(false),
    m_iFlags(0), m_iLenBuf(0) {
        eAddrType addrType = ip;
        char *pStr = (char *) strAddr;
        for( char i = 0; i < 4; i++ ) {
            if( pStr == NULL )
                break;
            pStr = strstr(pStr+1, ".");
        }
        if( pStr == NULL )
            addrType = domain;
        m_HostInfo.init(strAddr, iPort, addrType);
    }

    virtual ~CSockTCP();
    virtual bool _connect();
    virtual void disconnect();
    virtual void setKeepAlive(bool keepAlive);
    virtual void setNonBlocking(bool nonBlocking);
    virtual void sendMsg(const char *strMsg, size_t iLen = 0);
    virtual size_t read();
    virtual const char *readLine();
    virtual bool isReadReady(unsigned int sec_Timeout);
    virtual bool isReadyWrite(unsigned int sec_Timeout);
    static bool download(const char *strUrl, const char *strDir);
    static bool checkFileOnSite(const char *strUrl);

    virtual bool isConnected() const {
        return ( m_iFlags & FL_CONNECTED );
    }

    virtual const char *data() const {
        return (const char *) &m_strBuf;
    }

    virtual size_t getLenBuf() const {
        return m_iLenBuf;
    }
};

#endif /* SOCKET_H_ */
