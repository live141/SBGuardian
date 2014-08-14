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

#include "Warmodus.h"

void CWarmodus::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;

	m_c3rr = 0;
	m_flLastRR = 0.0;

	setEnabled(true);
}

void CWarmodus::unload() {
	setEnabled(false);
	m_c3rr = 0;
}

void CWarmodus::onFrame() {
	if( m_c3rr > 0 && m_pEngine->time() - m_flLastRR > 1.1 ) {
		m_pEngine->restartRound();
		m_c3rr--;
		m_flLastRR = m_pEngine->time();
	}
}

void CWarmodus::onRestartRound() {
	if( m_pEngine->game() != cstrike )
		return;

	// print money on new round
}

bool CWarmodus::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "warmodus") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("Warmodus is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
		return true;
	}

	const char *strArg2 = m_pEngine->argv(2); // subcmd

	if( !strcmp(strArg2, "3rr") ) {
		m_c3rr = 3;
		return true;
	}

	const char *strArg3 = m_pEngine->argv(3); // reason

	// kick all cts
	if( !strcmp(strArg2, "kickcts") ) {
		int iMax = m_pPlayerManager->getMaxClients();
		for( int i = 1; i <= iMax; i++ ) {
			IPlayer *pPlayer = m_pPlayerManager->getPlayer(i);
			if( pPlayer != NULL && pPlayer->getTeam() == team_ct ) {
				if( strArg3 != NULL && strArg3 != 0 )
					pPlayer->kick(strArg3);
				else
					pPlayer->kick("");
			}
		}
	}

	// kick all ts
	else if( !strcmp(strArg2, "kickts") ) {
		int iMax = m_pPlayerManager->getMaxClients();
		for( int i = 1; i <= iMax; i++ ) {
			IPlayer *pPlayer = m_pPlayerManager->getPlayer(i);
			if( pPlayer != NULL && pPlayer->getTeam() == team_t ) {
				if( strArg3 != NULL && strArg3 != 0 )
					pPlayer->kick(strArg3);
				else
					pPlayer->kick("");
			}
		}
	}
	
	// kick alls specs
	else if( !strcmp(strArg2, "kickspecs") ) {
		int iMax = m_pPlayerManager->getMaxClients();
		for( int i = 1; i <= iMax; i++ ) {
			IPlayer *pPlayer = m_pPlayerManager->getPlayer(i);
			if( pPlayer != NULL && pPlayer->getTeam() == team_spectator ) {
				if( strArg3 != NULL && strArg3 != 0 )
					pPlayer->kick(strArg3);
				else
					pPlayer->kick("");
			}
		}
	}

	else if( !strcmp(strArg2, "status") ) {
		printf("====================================\n");
		// normal rcon status
		m_pEngine->status();
		printf("\n\n");
		// sbguardian status
		IModule **pModule = m_pModule;
		while( pModule != NULL && *pModule != NULL ) {
			printf("%s: %s\n", pModule[0]->getName(), (pModule[0]->isEnabled())?"enabled":"disabled");
			pModule++;
		}
		printf("====================================\n");
	}

	else if( m_pEngine->argc() == 3 ) {
		setEnabled(atoi(m_pEngine->argv(2)));
		return true;
	}

	return true;
}

