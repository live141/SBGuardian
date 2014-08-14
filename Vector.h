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

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>

template <class Type>
class vector {
private:
	Type *m_aData;
	size_t m_iSize;

public:
	typedef Type* iterator;

public:
	vector() : m_aData(NULL), m_iSize(0) { }
	~vector() {
		clear();
	}

	bool empty() {
		return ( m_iSize == 0 || m_aData == NULL );
	}

	void clear() {
		if( m_aData != NULL )
			free(m_aData);
		m_aData = NULL;
		m_iSize = 0;
	}

	size_t size() {
		return m_iSize;
	}

	Type* begin() {
		return m_aData;
	}

	Type* end() {
		return m_aData+m_iSize;
	}

	void erase(iterator it) {
		if( it == NULL || it - m_aData < 0 || m_iSize == 0 )
			return;
		m_iSize--;
		Type *tmp = m_aData;
		m_aData = (Type *) calloc(m_iSize, sizeof(Type));
		int iOff = (it - tmp)/sizeof(Type);
		// memcpy(m_aData, tmp, iOff);
		for(int i = 0; i < iOff; i++) {
			m_aData[i] = tmp[i];
		}
		// memcpy(m_aData+iOff, tmp+iOff+1, (m_iSize-iOff));
		for(int i = iOff; i < (m_iSize-iOff); i++) {
			m_aData[i] = tmp[i];
		}

		free(tmp);
	}

	void push_back(const Type& data) {
		if( m_iSize == 0 ) {
			m_aData = (Type *) calloc(1, sizeof(Type));
			// memcpy(m_aData, &data, sizeof(Type));
			m_aData[0] = data;
			m_iSize++;
			return;
		}

		Type *tmp = m_aData;
		m_aData = (Type *) calloc(m_iSize+1, sizeof(Type));
		// memcpy(m_aData, tmp, m_iSize);
		for(unsigned int i = 0; i < m_iSize; i++) {
			m_aData[i] = tmp[i];
		}
		free(tmp);
		m_aData[m_iSize] = data;
		m_iSize++;
	}

	void pop_back() {
		if( m_iSize == 0 )
			return;
		Type *tmp = m_aData;
		m_iSize--;
		m_aData = (Type *) calloc(m_iSize, sizeof(Type));
		// memcpy(m_aData, tmp, m_iSize);
		for(int i = 0; i < m_iSize; i++) {
			m_aData[i] = tmp[i];
		}
		free(tmp);
	}

	Type *back() {
		if( m_iSize == 0 )
			return end();
		return &m_aData[m_iSize-1];
	}

	Type *find(const Type& data) {
		if( m_iSize == 0 )
			return end();
		for( int i = 0; i < m_iSize; i++ ) {
			if( data == m_aData[i] )
				return &m_aData[i];
		}

		return end();
	}
};

#endif /* VECTOR_H_ */
