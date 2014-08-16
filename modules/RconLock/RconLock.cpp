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

#include "RconLock.h"
#include <stdio.h>

void CRconLock::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;

	setEnabled(false);
}

void CRconLock::unload() {
	setEnabled(false);
}

bool CRconLock::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( isEnabled() && !strcmp(strArg0, "rcon_password") ) {
		if( m_pEngine->argc() > 1 ) {
			return true;
		}
		return false;
	}

	if( strcmp(strArg0, "rconlock") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("RconLock is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		int iEnable = atoi(m_pEngine->argv(2));
		if( iEnable == 1 )
			setEnabled(true);
	}

	return true;
}
