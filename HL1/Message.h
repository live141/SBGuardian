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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "../Vector.h"
#include "../CStr.h"

class IType {
public:
	virtual ~IType() { };
	virtual void *data() const = 0;
};

class CMsgString : public IType {
private:
	CStr m_sBuf;

public:
	CMsgString(const char *strBuf) : m_sBuf(strBuf) { }
	virtual ~CMsgString() {

	}

	virtual void *data() const {
		return (void *) m_sBuf.data();
	}
};

class CMsgInt : public IType {
private:
	int *m_iInt;

public:
	CMsgInt(int iInt) {
		m_iInt = new int;
		*m_iInt = iInt;
	}
	virtual ~CMsgInt() {
		delete m_iInt;
	}
	virtual void *data() const {
		return (void *) m_iInt;
	}
};

class CMsgFloat : public IType {
private:
	float *m_flFloat;

public:
	CMsgFloat(float flFloat) {
		m_flFloat = new float;
		*m_flFloat = flFloat;
	}
	virtual ~CMsgFloat() {
		delete m_flFloat;
	}
	virtual void *data() const {
		return m_flFloat;
	}
};

class CMessage {
private:
	int m_iMsg;
	edict_t *m_pEnt;
	vector<IType*> m_vecTypes;

public:
	CMessage() : m_iMsg(0), m_pEnt(NULL) {
		m_vecTypes.clear();
	}

	void init(int iMsg, edict_t *pEnt) {
		m_iMsg = iMsg;
		m_pEnt = pEnt;
		clear();
	}

	void add(IType *type) {
		m_vecTypes.push_back(type);
	}

	edict_t *getEnt() const {
		return m_pEnt;
	}

	int getMsg() const {
		return m_iMsg;
	}

	void clear() {
		vector<IType *>::iterator it = m_vecTypes.begin();
		for( ; it < m_vecTypes.end(); it++ ) {
			delete *it;
		}

		m_vecTypes.clear();
	}

	void *arg(int i) {
		if( m_vecTypes.empty() || i > m_vecTypes.size() )
			return NULL;
		vector<IType *>::iterator it = m_vecTypes.begin();
		return it[i-1]->data();
	}

};

#endif /* MESSAGE_H_ */
