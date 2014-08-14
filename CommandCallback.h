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

#ifndef COMMANDCALLBACK_H_
#define COMMANDCALLBACK_H_

#include "Vector.h"
#include <string.h>

typedef void (*pCommand)(void);

class CCmd {
private:
	pCommand m_pCmd;
	char *m_strName;

public:
	CCmd() : m_pCmd(NULL), m_strName(NULL) { }
	CCmd(const char *strName, void (*func)(void)) {
		size_t iLen = strlen(strName);
		m_strName = new char[iLen+1];
		memcpy(m_strName, strName, iLen);
		m_strName[iLen+1] = 0;
		m_pCmd = func;
	}

	~CCmd() {
		if( m_strName != NULL )
			delete[] m_strName;
		m_strName = NULL;
	}

	bool operator ==(CCmd& Cmd) const {
		return ( !strcmp(m_strName, Cmd.m_strName) );
	}
};

class CCommandCallback {
private:
	vector<CCmd> m_vecCmd;

public:
	static void hookDummy();
};

#endif /* COMMANDCALLBACK_H_ */
