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

#ifndef HOSTINFO_H_
#define HOSTINFO_H_

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
// #include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define snprintf _snprintf
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <string.h>

enum eAddrType {
    ip,
    domain
};

class CHostInfo {
private:
    eAddrType m_addrType;
    int m_iIP;
    short m_iPort;

public:
    CHostInfo() : m_iIP(0), m_iPort(0) { }
    CHostInfo(const char *strAddr, short iPort, eAddrType addrType) : m_addrType(addrType),
    m_iIP(0), m_iPort(iPort) {
        init(strAddr, iPort, addrType);
    }

    int getIP() {
        return m_iIP;
    }

    int getPort() {
        return m_iPort;
    }

    void init(const char *strAddr, short iPort, eAddrType addrType) {
        m_iPort = iPort;
        m_iIP = 0;

        if( strAddr == NULL )
            return;

        if( addrType == ip ) {
            m_iIP = (int) inet_addr(strAddr);
        }
        else if( addrType == domain ) {
            addrinfo *pAddrInfo = NULL;
            char strPort[20];
            snprintf(strPort, sizeof(strPort), "%u", m_iPort);

            // filter :// and before
            const char *strAddrFilt = strstr(strAddr, "://");
            if( strAddrFilt != NULL )
                strAddrFilt += 3;
            else
                strAddrFilt = strAddr;

            int iRes = getaddrinfo(strAddrFilt, strPort, NULL, &pAddrInfo);

            if( iRes !=0 ) {
                // error
                printf("Could not get IP from domain...\n");
            }

            else {
                m_iIP = ((sockaddr_in *)(pAddrInfo->ai_addr))->sin_addr.s_addr;
                freeaddrinfo(pAddrInfo);
            }
        }
    }

    ~CHostInfo() {

    }
};

#endif /* HOSTINFO_H_ */
