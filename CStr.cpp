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

#include "CStr.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

const char* CStr::format(const char *fmt, ...) {
	va_list ap;
	static char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	
	return buf;
}

const char* CBuf::thisFormat(const char *fmt, ...) {
	clear();
	va_list ap;
	size_t len = strlen(fmt);
	m_strBuf = new char[len+1024];

	va_start(ap, fmt);
	vsnprintf(m_strBuf, len+1024, fmt, ap);
	va_end(ap);

	return m_strBuf;
}
