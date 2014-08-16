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

#ifndef TIME_H_
#define TIME_H_

#include <time.h>

class CTime {
private:
	time_t m_time;
	char m_strBufTime[80];
	char m_strBufDate[80];

public:
	CTime() : m_time(0) {
		m_time = time(NULL);
	}

	CTime& operator =(CTime& t) {
		m_time = t.m_time;
		return *this;
	}

	bool operator ==(CTime& t) const {
		return ( m_time == t.m_time );
	}

	bool operator >=(CTime& t) const {
		return ( m_time >= t.m_time );
	}

	bool operator >(CTime& t) const {
		return ( m_time > t.m_time );
	}

	bool operator <=(CTime& t) const {
		return ( m_time <= t.m_time );
	}

	bool operator <(CTime& t) const {
		return ( m_time < t.m_time );
	}

	time_t timeSecs() const {
		return m_time;
	}

	const char *getTime();
	const char *getDate();
	static const char *timeStatic();
	static const char *dateStatic();
	static time_t now();
};

#endif /* TIME_H_ */
