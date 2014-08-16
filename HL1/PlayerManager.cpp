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

#include "PlayerManager.h"
#include "hl1.h"
#include "SrcHooks.h"
//#define STRING(offset) (const char *)(g_Globals->pStringBase + (int)offset)

CPlayerManager g_PlayerManager;

bool CPlayerManager::load(bool bLate) {
	m_iMaxClients = g_Globals->maxClients;
	m_aPlayer = new CPlayer [m_iMaxClients+1];
	m_iIterator = 0;
	if( !bLate )
		return true;

	// late load
	/* Players are already connected */
	for(int i = 1; i <= m_iMaxClients; i++)
	{
		edict_t *pEdict = g_EngFuncs.pfnPEntityOfEntIndex( i );
		if( pEdict == NULL || pEdict->free || strcmp( STRING(pEdict->v.classname), "player" ) != 0 )
			continue;
		onConnect(pEdict);
	}
	printf("SBGuardian: PlayerManager loaded\r\n");
	return true;
}

void CPlayerManager::unload() {
	if( m_aPlayer != NULL )
		delete[] m_aPlayer;
	m_aPlayer = NULL;
}

void CPlayerManager::onTick() {
	if( m_iIterator == (size_t) m_iMaxClients+1 )
		m_iIterator = 1;

	CPlayer *pPlayer = &m_aPlayer[m_iIterator];
	if( pPlayer == NULL || !pPlayer->isInGame() ) {
		m_iIterator++;
		return;
	}

	if( !pPlayer->m_bHooked ) {
		/* hook here to make sure CBaseEntity is allocated */
		CSrcHooks::loadHooks((edict_t *) pPlayer->m_pEnt);
		pPlayer->m_bHooked = true;
	}

	// get ping
	int ping = 0, loss = 0;
	g_EngFuncs.pfnGetPlayerStats(pPlayer->m_pEnt, &ping, &loss);
	pPlayer->m_iPing = ping;

	// query interp - hook cmd instead
	// pPlayer->queryCvar("ex_interp");

	m_iIterator++;
}

void CPlayerManager::onCommand(edict_t *pEdict) {
	CPlayer *pPlayer = getPlayer(pEdict);
	if( pPlayer == NULL || !pPlayer->isInGame() )
		return;

	const char *strCmd = g_EngFuncs.pfnCmd_Argv(0);
	if( strcmp("ex_interp", strCmd) )
		return;

	// check if its a num
	const char *strArg = g_EngFuncs.pfnCmd_Argv(1);
	size_t iLen = strlen(strArg);
	for( size_t i = 0; i < iLen; i++ ) {
		if( ( strArg[i] < 0x30 || strArg[i] > 0x39 ) && strArg[i] != '.' )
			return;
	}

	pPlayer->m_flInterp = atof(strArg);
}

CPlayer *CPlayerManager::getPlayer(edict_t *pEdict) const {
	for( int i = 1; i <= m_iMaxClients; i++ ) {
		if( m_aPlayer[i].getEdict() == pEdict ) {
			if( m_aPlayer[i].isInGame() )
				return m_aPlayer+i;
		}
	}

	return NULL;
}

void CPlayerManager::onCvarValue(CPlayer *pPlayer, const char *strName, float flValue) {
	if( strcmp("ex_interp", strName) )
		return;

	pPlayer->m_flInterp = flValue;
}

void CPlayerManager::onConnect(edict_t *pEdict) {
	int iIndex = g_EngFuncs.pfnIndexOfEdict(pEdict);
	m_aPlayer[iIndex].onConnect(pEdict);
	m_iConnectedPlayers++;
	// get interp
	m_aPlayer[iIndex].queryCvar("ex_interp");
}

void CPlayerManager::onDisconnect(edict_t *pEdict) {
	int iIndex = g_EngFuncs.pfnIndexOfEdict(pEdict);
	m_aPlayer[iIndex].onDisconnect();
	m_iConnectedPlayers--;
	CSrcHooks::unloadHooks(pEdict);
}
