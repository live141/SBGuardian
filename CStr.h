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

#ifndef CSTR_H_
#define CSTR_H_

#include <string.h>
#include <stdio.h>

class CStr {
protected:
	char *m_strBuf;
	size_t m_iLen;

public:
	CStr() : m_strBuf(NULL), m_iLen(0) { }
	CStr(const char *strStr) : m_strBuf(NULL), m_iLen(0) {
		m_iLen = strlen(strStr);
		m_strBuf = new char[m_iLen+1];
		memcpy(m_strBuf, strStr, m_iLen);
		m_strBuf[m_iLen] = 0;
	}

	virtual ~CStr() {
		clear();
	}

	void clear() {
		if( m_strBuf != NULL )
			delete[] m_strBuf;
		m_strBuf = NULL;
		m_iLen = 0;
	}

	size_t len() const {
		return m_iLen;
	}

	const char *data() const {
		return m_strBuf;
	}

	bool operator ==(const CStr& str) const {
		return ( !strcmp(str.m_strBuf, m_strBuf) );
	}

	CStr& operator =(const char c) {
		clear();
		m_strBuf = new char[2];
		m_strBuf[0] = c;
		m_strBuf[1] = 0;
		m_iLen = 1;
		return *this;
	}

	CStr& operator =(const char* strStr) {
		clear();
		m_iLen = strlen(strStr);
		m_strBuf = new char[m_iLen+1];
		memcpy(m_strBuf, strStr, m_iLen);
		m_strBuf[m_iLen] = 0;
		return *this;
	}

	CStr& operator =(const CStr& str) {
		clear();
		m_iLen = str.m_iLen;
		m_strBuf = new char[m_iLen+1];
		memcpy(m_strBuf, str.m_strBuf, m_iLen);
		m_strBuf[m_iLen] = 0;
		return *this;
	}

	CStr& operator +=(const char c) {
		const char *tmp = m_strBuf;
		m_strBuf = new char [m_iLen+2];
		if( tmp != NULL ) {
			memcpy(m_strBuf, tmp, m_iLen);
			delete[] tmp;
		}
		m_strBuf[m_iLen] = c;
		m_iLen++;
		m_strBuf[m_iLen] = 0;
		return *this;
	}

	CStr& operator +=(const char *strStr) {
		const char *tmp = m_strBuf;
		size_t iLen = strlen(strStr);
		m_strBuf = new char [m_iLen+iLen+1];
		if( tmp != NULL ) {
			memcpy(m_strBuf, tmp, m_iLen);
			delete[] tmp;
		}
		memcpy(m_strBuf+m_iLen, strStr, iLen);
		m_iLen += iLen;
		m_strBuf[m_iLen] = 0;
		return *this;
	}

	static const char *format(const char *fmt, ...);
};

class CBuf : public CStr {
private:

public:
	virtual ~CBuf() {
		clear();
	}

	const char *thisFormat(const char *fmt, ...);
};


#endif /* CSTR_H_ */
