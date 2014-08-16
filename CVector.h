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

#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <cmath>

class CVector {
	friend CVector operator *(float fNum, CVector& vec);
public:
	float m_fX, m_fY, m_fZ;

public:
	CVector() : m_fX(0.0), m_fY(0.0), m_fZ(0.0) { }
	CVector(float x, float y, float z) : m_fX(x), m_fY(y), m_fZ(z) { }
	CVector(float *flCoords) : m_fX(flCoords[0]), m_fY(flCoords[1]), m_fZ(flCoords[2]) { }
	CVector(const CVector& vec) {
		m_fX = vec.m_fX;
		m_fY = vec.m_fY;
		m_fZ = vec.m_fZ;
	}

	void set(float *flCoords) {
		m_fX = flCoords[0];
		m_fY = flCoords[1];
		m_fZ = flCoords[2];
	}

	void set(float x, float y, float z) {
		m_fX = x;
		m_fY = y;
		m_fZ = z;
	}

	bool operator ==(const CVector& vec) const {
		return ( m_fX == vec.m_fX && m_fY == vec.m_fY && m_fZ == vec.m_fZ );
	}

	bool operator !=(const CVector& vec) const {
		return ( m_fX != vec.m_fX || m_fY != vec.m_fY || m_fZ != vec.m_fZ );
	}

	/*
	CVector operator =(CVector& vec) {
		m_fX = vec.m_fX;
		m_fY = vec.m_fY;
		m_fZ = vec.m_fZ;
		return *this;
	}
*/

	CVector operator =(const CVector& vec) {
		m_fX = vec.m_fX;
		m_fY = vec.m_fY;
		m_fZ = vec.m_fZ;
		return *this;
	}

	CVector operator +(const CVector& vec) {
		return CVector(m_fX+vec.m_fX, m_fY+vec.m_fY, m_fZ+vec.m_fZ);
	}

	CVector operator -(const CVector& vec) {
		return CVector(m_fX-vec.m_fX, m_fY-vec.m_fY, m_fZ-vec.m_fZ);
	}

	float operator *(const CVector& vec) {
		return m_fX*vec.m_fX + m_fY*vec.m_fY + m_fZ*vec.m_fZ;
	}

	CVector operator *(float fNum) {
		return CVector(fNum*m_fX, fNum*m_fY, fNum*m_fZ);
	}

	float len() const {
		return sqrt(m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ);
	}

	CVector& normalize() {
		float fLen = len();
		if( fLen != 0.0 ) {
			m_fX /= fLen;
			m_fY /= fLen;
			m_fZ /= fLen;
		}

		return *this;
	}
};

inline CVector operator *(float fNum, CVector& vec) {
		return vec * fNum;
}

#endif /* CVECTOR_H_ */
