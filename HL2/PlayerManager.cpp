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
#include "hl2.h"
#include "ServerPlugin.h"
#include <inetchannelinfo.h>
#include <cbase.h>

// #define IndexOfEdict(a) g_pEngine->(a);
#undef GetClassName

CPlayerManager g_PlayerManager;

bool CPlayerManager::load(bool bLate) {
	m_iMaxClients = g_pGlobals->maxClients;
	m_aPlayer = new CPlayer [m_iMaxClients+1];
	m_iIterator = 0;
	if( !bLate )
		return true;

	// late load
	/* Players are already connected */
	for(int i = 1; i <= m_iMaxClients; i++)
	{
		edict_t *pEdict = PEdictOfIndex( i );
		if( pEdict == NULL || pEdict->IsFree() || strcmp( pEdict->GetClassName(), "player" ) != 0 )
			continue;
		onConnect(pEdict);
	}

	return true;
}

void CPlayerManager::unload() {
	if( m_aPlayer != NULL )
		delete[] m_aPlayer;
	m_aPlayer = NULL;
}

void CPlayerManager::onTick() {
	if( m_iIterator == m_iMaxClients+1 )
		m_iIterator = 1;

	CPlayer *pPlayer = &m_aPlayer[m_iIterator];
	if( pPlayer == NULL || !pPlayer->isInGame() ) {
		m_iIterator++;
		return;
	}

	// get steamid
	const char *strSteamId = g_pEngine->GetPlayerNetworkIDString(pPlayer->m_pEnt);
	if( !pPlayer->m_bAuthed && strcmp(strSteamId, "STEAM_ID_PENDING") ) {
		/* SourceTV has also the steamid BOT, so check SourceTV first */
		IPlayerInfo *pPlayerInfo = g_pPlayerInfoMgr->GetPlayerInfo(pPlayer->m_pEnt);
		if( pPlayerInfo->IsHLTV() ) {
			pPlayer->m_bHLTV = true;
		}
		else {
			if( !strcmp(strSteamId, "BOT") )
				pPlayer->m_SteamId = CSteamId();
			else
				pPlayer->m_SteamId = CSteamId(strSteamId);
		}

		pPlayer->m_bAuthed = true;
	}

	// get ping
	pPlayer->m_iPing = ((int)(g_pEngine->GetPlayerNetInfo(m_iIterator)->GetLatency(0)*1000*0.7));

	// CBaseEntity *pEntity = ((edict_t *) pPlayer->getEdict())->GetUnknown()->GetBaseEntity();
	// int iOff = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_fLerptime");
	// int iOff2 = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_flSpeed");
	// printf("%f, %d\n", *(float *)((char *)pEntity + iOff), *(float *)((char *)pEntity + iOff2));

	m_iIterator++;
}

CPlayer *CPlayerManager::getPlayerByUserId(int iUserId) const {
	for( char i = 1; i <= m_iMaxClients; i++ ) {
		if( m_aPlayer[i].getUserId() == iUserId ) {
			if( m_aPlayer[i].isInGame() )
				return m_aPlayer+i;
		}
	}

	return NULL;
}

void CPlayerManager::onConnect(edict_t *pEdict) {
	int iIndex = IndexOfEdict(pEdict);
	m_aPlayer[iIndex].onConnect(pEdict);
	m_iConnectedPlayers++;
	g_ServerPlugin.loadPlayerHooks(pEdict);

	// get interp
	m_aPlayer[iIndex].queryCvar("cl_interp");

	// CBaseEntity *pEntity = pEdict->GetUnknown()->GetBaseEntity();
	// int iOff = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_fLerpTime");
	// g_ServerPlugin.printDataMap(pEntity->GetDataDescMap());
	// printf("%d\n", iOff);
}

void CPlayerManager::onDisconnect(edict_t *pEdict) {
	int iIndex = IndexOfEdict(pEdict);
	m_aPlayer[iIndex].onDisconnect();
	m_iConnectedPlayers--;
}

void CPlayerManager::onCommand(edict_t *pEdict) {
	CPlayer *pPlayer = getPlayer(pEdict);
	if( pPlayer == NULL || !pPlayer->isInGame() )
		return;

	const char *strCmd = g_Engine.argv(0);
	if( strcmp("cl_interp", strCmd) )
		return;

	// check if its a num
	const char *strArg = g_Engine.argv(1);
	size_t iLen = strlen(strArg);
	for( size_t i = 0; i < iLen; i++ ) {
		if( ( strArg[i] < 0x30 || strArg[i] > 0x39 ) && strArg[i] != '.' )
			return;
	}

	pPlayer->m_flInterp = atof(strArg);
}

void CPlayerManager::onCvarValue(CPlayer *pPlayer, const char *strName, float flValue) {
	if( strcmp("cl_interp", strName) )
		return;

	pPlayer->m_flInterp = flValue;
}

CPlayer *CPlayerManager::getPlayer(edict_t *pEdict) const {
	if( pEdict == NULL )
		return NULL;
	for( char i = 1; i <= m_iMaxClients; i++ ) {
		if( m_aPlayer[i].getEdict() == pEdict ) {
			if( m_aPlayer[i].isInGame() )
				return m_aPlayer+i;
		}
	}

	return NULL;
}

