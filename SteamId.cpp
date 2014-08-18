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

#include "SteamId.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char CSteamId::bitUniverse = 0;

CSteamId::CSteamId(const char *strId) {
	if( strId == NULL || strlen(strId) < 11 || strId[7] != ':' || strId[9] != ':' || strncmp(strId, "STEAM", 5) )
		return;

	bitUniverse = strId[6] - 0x30;
	m_iId = ( atoi(strId+10) << 1 ) | ( strId[8] == '1' );
}

const char *CSteamId::getStatic() const {
	static char strBuf[64];
	snprintf(strBuf, sizeof(strBuf), "STEAM_%d:%d:%d", bitUniverse, m_iId & 1, m_iId >> 1);
	return strBuf;
}
