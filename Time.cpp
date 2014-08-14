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

#include "Time.h"

time_t CTime::now() {
	return time(NULL);
}

const char *CTime::getTime() {
	tm *timeinfo = localtime(&m_time);
	strftime(m_strBufTime, sizeof(m_strBufTime), "%H:%M:%S", timeinfo);
	// printf("%s\n", m_strBufTime);
	return m_strBufTime;
}

const char *CTime::getDate() {
	tm *timeinfo = localtime(&m_time);
	strftime(m_strBufDate, sizeof(m_strBufDate), "%d/%m/%Y", timeinfo);
	// printf("%s\n", m_strBufDate);
	return m_strBufDate;
}

const char *CTime::timeStatic() {
	static char buf[80];
	time_t t = time(NULL);
	tm *timeinfo = localtime(&t);
	strftime(buf, sizeof(buf), "%H:%M:%S", timeinfo);
	return buf;
}

const char *CTime::dateStatic() {
	static char buf[80];
	time_t t = time(NULL);
	tm *timeinfo = localtime(&t);
	strftime(buf, sizeof(buf), "%d/%m/%Y", timeinfo);
	return buf;
}
