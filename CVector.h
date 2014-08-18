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
	//float m_f[0], m_f[1], m_f[2];
	float m_f[4] __attribute__((aligned(4)));

public:
	CVector() { m_f[0] = 0.0; m_f[1] = 0.0; m_f[2] = 0.0; }
	CVector(float x, float y, float z) { m_f[0] = x; m_f[1] = y; m_f[2] = z; }
	CVector(float *flCoords) { m_f[0] = flCoords[0]; m_f[1] = flCoords[1]; m_f[2] = flCoords[2]; }
	CVector(const CVector& vec) {
		m_f[0] = vec.m_f[0];
		m_f[1] = vec.m_f[1];
		m_f[2] = vec.m_f[2];
	}

	void set(float *flCoords) {
		m_f[0] = flCoords[0];
		m_f[1] = flCoords[1];
		m_f[2] = flCoords[2];
	}

	void set(float x, float y, float z) {
		m_f[0] = x;
		m_f[1] = y;
		m_f[2] = z;
	}

	bool operator ==(const CVector& vec) const {
		return ( m_f[0] == vec.m_f[0] && m_f[1] == vec.m_f[1] && m_f[2] == vec.m_f[2] );
	}

	bool operator !=(const CVector& vec) const {
		return ( m_f[0] != vec.m_f[0] || m_f[1] != vec.m_f[1] || m_f[2] != vec.m_f[2] );
	}

	CVector operator =(const CVector& vec) {
		m_f[0] = vec.m_f[0];
		m_f[1] = vec.m_f[1];
		m_f[2] = vec.m_f[2];
		return *this;
	}

	CVector operator +(const CVector& vec) {
		float f[4] __attribute__((aligned(4)));
		asm volatile("movaps (%1), %%xmm0\n\t"
				"movaps (%2), %%xmm1\n\t"
				"addps %%xmm1, %%xmm0\n\t"
				"movaps %%xmm0, %0\n\t"
				: "=m"(f)
				: "r"(m_f), "r"(vec.m_f)
				: );
		return CVector(f);
		//return CVector(m_f[0]+vec.m_f[0], m_f[1]+vec.m_f[1], m_f[2]+vec.m_f[2]);
	}

	CVector operator -(const CVector& vec) {
		float f[4] __attribute__((aligned(4)));
		asm volatile("movaps (%1), %%xmm0\n\t"
				"movaps (%2), %%xmm1\n\t"
				"subps %%xmm1, %%xmm0\n\t"
				"movaps %%xmm0, %0\n\t"
				: "=m"(f)
				: "r"(m_f), "r"(vec.m_f)
				: );
		return CVector(f);
		//return CVector(m_f[0]-vec.m_f[0], m_f[1]-vec.m_f[1], m_f[2]-vec.m_f[2]);
	}

	float operator *(const CVector& vec) {
		/* TODO: Further optimization */
		float f[4] __attribute__((aligned(4)));
		asm volatile("movaps (%1), %%xmm0\n\t"
				"movaps (%2), %%xmm1\n\t"
				"mulps %%xmm1, %%xmm0\n\t"
				"movaps %%xmm0, %0\n\t"
				: "=m"(f)
				: "r"(m_f), "r"(vec.m_f)
				: );
		return f[0]+f[1]+f[2];
		//return m_f[0]*vec.m_f[0] + m_f[1]*vec.m_f[1] + m_f[2]*vec.m_f[2];
	}

	CVector operator *(float fNum) {
		return CVector(fNum*m_f[0], fNum*m_f[1], fNum*m_f[2]);
	}

	float len() const {
		/* TODO: More optimization */
		float f[4] __attribute__((aligned(4)));
		asm volatile("movaps (%1), %%xmm0\n\t"
				"mulps %%xmm0, %%xmm0\n\t"
				"movaps %%xmm0, %0\n\t"
				: "=m"(f)
				: "r"(m_f)
				: );
		return sqrt(f[0]+f[1]+f[2]);
		//return sqrt(m_f[0]*m_f[0] + m_f[1]*m_f[1] + m_f[2]*m_f[2]);
	}

	CVector& normalize() {
		float fLen = len();
		if( fLen != 0.0 ) {
			m_f[0] /= fLen;
			m_f[1] /= fLen;
			m_f[2] /= fLen;
		}

		return *this;
	}
};

inline CVector operator *(float fNum, CVector& vec) {
		return vec * fNum;
}

#endif /* CVECTOR_H_ */
