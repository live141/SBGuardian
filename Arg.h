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

#ifndef ARG_H_
#define ARG_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class CArg {
private:
	size_t m_iLen, m_iArgc;
	char *m_strCmd;
	char **m_strArgs;

	void _parse(const char *strCmd) {
		if( strCmd == NULL || strCmd[0] == 0 )
			return;

		m_iArgc = 0;
		m_iLen = strlen(strCmd);
		m_strCmd = new char[m_iLen+1];
		memcpy(m_strCmd, strCmd, m_iLen);
		m_strCmd[m_iLen] = 0;
		char *last = m_strCmd;
		char *tmp = m_strCmd;

		while( (last = strstr(last+1, " ")) != NULL ) {
			m_iArgc++;
		}
		m_iArgc++;

		m_strArgs = new char*[m_iArgc];

		last = m_strCmd;

		size_t x = 0;
		while( (last = strstr(last, " ")) != NULL || (last = strstr(tmp, "\n")) != NULL ) {
			size_t len = last - tmp;

			if( *tmp != ' ' ) {
				m_strArgs[x] = new char[len+1];
				memcpy(m_strArgs[x], tmp, len);
				m_strArgs[x][len] = 0;
			}

			if( last[0] == '\n' )
				break;

			tmp = ++last;
			x++;
		}
	}

public:
	CArg() : m_iLen(0), m_strCmd(NULL), m_iArgc(0), m_strArgs(NULL) { }
	CArg(const char *strCmd) : m_iLen(0), m_strCmd(NULL), m_iArgc(0), m_strArgs(NULL) {
		_parse(strCmd);
	}

	~CArg() {
		if( m_strCmd != NULL )
			delete[] m_strCmd;

		if( m_strArgs != NULL ) {
			for( size_t i = 0; i < m_iArgc; i++ ) {
				// delete[] m_strArgs[i];
			}
			delete[] m_strArgs;
		}

		m_strCmd = NULL;
		m_strArgs = NULL;
	}

	size_t argc() const {
		return m_iArgc;
	}

	const char *argv(int iIndex) const {
		if( iIndex < 0 || iIndex > m_iArgc-1 ) {
			return NULL;
		}
		return m_strArgs[iIndex];
	}
};

#endif /* ARG_H_ */
