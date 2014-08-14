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

#include "AntiFlashhack.h"
#include <stdio.h>

void CAntiFlashhack::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;
	m_flFlashTill = new float[pPlayerManager->getMaxClients()+1];

	setEnabled(true);
}

void CAntiFlashhack::unload() {
	setEnabled(false);
	if( m_flFlashTill != NULL )
		delete[] m_flFlashTill;
	m_flFlashTill = NULL;
}

bool CAntiFlashhack::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "flash") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("AntiFlashhack is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		setEnabled(atoi(m_pEngine->argv(2)));
	}

	return true;
}

bool CAntiFlashhack::isFlashed(IPlayer *pPlayer) {
	if( !isEnabled() || m_pEngine->game() != cstrike )
		return false;
	return ( m_pEngine->time() < m_flFlashTill[pPlayer->getIndex()] );
}

void CAntiFlashhack::onJoin(IPlayer *pPlayer) {
	m_flFlashTill[pPlayer->getIndex()] = 0.0;
}

void CAntiFlashhack::onLeave(IPlayer *pPlayer) {
	m_flFlashTill[pPlayer->getIndex()] = 0.0;
}

void CAntiFlashhack::onFlash(IPlayer *pPlayer, float flTill) {
	m_flFlashTill[pPlayer->getIndex()] = flTill;
}
