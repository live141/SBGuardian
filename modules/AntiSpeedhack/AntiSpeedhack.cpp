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

#include "AntiSpeedhack.h"
#include <stdio.h>

void CAntiSpeedhack::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;

	m_iTicks = new int[pPlayerManager->getMaxClients()];

	setEnabled(true);
}

void CAntiSpeedhack::unload() {
	setEnabled(false);

	if( m_iTicks != NULL )
		delete[] m_iTicks;
	m_iTicks = NULL;
}

bool CAntiSpeedhack::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "sh") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("AntiSpeedhack is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		setEnabled(atoi(m_pEngine->argv(2)));
	}

	return true;
}

void CAntiSpeedhack::onFrame() {
	for( int i = 1; i <= m_pPlayerManager->getMaxClients(); i++ ) {
		m_iTicks[i] = 0;
	}
}

void CAntiSpeedhack::onMove(IPlayer *pPlayer) {
	if( !isEnabled() )
		return;

	int iIndex = pPlayer->getIndex();
	printf("%d\n", m_iTicks[iIndex]);
	int& iTicks = m_iTicks[iIndex];
	iTicks++;
	printf("%d %f\n", iTicks, m_pEngine->tickrate());
	if( iTicks > 1.3 * m_pEngine->tickrate() ) {
		m_pEngine->printHud(CStr::format("[SBG]: %s is a using a speedhack!! %d\n", pPlayer->getNick(), iTicks));
	}
}

