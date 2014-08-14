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

#include "Player.h"
#include "../CVector.h"
#include "ServerPlugin.h"
#include "Engine.h"
// #include <baseentity.h>
#define GAME_DLL 1
#include <cbase.h>

#define ENTINDEX(a) g_pEngine->IndexOfEdict(a);

void CPlayer::onConnect(edict_t *pEnt) {
	m_pEnt = pEnt;
	m_cIndex = IndexOfEdict(pEnt);
	m_iUserId = g_pEngine->GetPlayerUserId(pEnt);
	m_flInterp = 0;
	m_iUserId = g_pEngine->GetPlayerUserId(pEnt);

    /* make him bot till real steamid is evaluated */
	m_SteamId = CSteamId();
    m_bHLTV = false;
    m_bAuthed = false;
	m_bInGame = true;
}

void CPlayer::onDisconnect() {
	m_bAuthed = false;
	m_bInGame = false;
	m_pEnt = NULL;
}

bool CPlayer::isAlive() const {
	// return( g_pPlayerInfoMgr->GetPlayerInfo(m_pEnt)->GetHealth() > 0
			// && g_pGameEnts->EdictToBaseEntity(m_pEnt)->m_lifeState.Get() == LIFE_ALIVE );
	return ((CBaseEntity *) m_pEnt->GetUnknown()->GetBaseEntity())->IsAlive();
}

bool CPlayer::isSpec() const {
	return( g_pGameEnts->EdictToBaseEntity(m_pEnt)->GetTeamNumber() == TEAM_SPECTATOR );
}

bool CPlayer::isDying() const {
	return ( g_pGameEnts->EdictToBaseEntity(m_pEnt)->m_lifeState.Get() == LIFE_DYING );
}

bool CPlayer::isSolid() const {
	return ( g_pGameEnts->EdictToBaseEntity(m_pEnt)->GetMoveType() == MOVETYPE_WALK );
}

void CPlayer::kick(const char *strReason) const {
	char buf[128];
	snprintf(buf, sizeof(buf), "kickid %d \"%s\"\n", m_iUserId, strReason);
	g_Engine.command(buf);
}

void CPlayer::ban(const char *strReason) const {
	char buf[128];
	snprintf(buf, sizeof(buf), "banid 0.0 %d \"%s\"\n", m_iUserId, strReason);
	g_Engine.command(buf);
	this->kick(strReason);
}

int CPlayer::getIdOfWatchedPlayer() const {
	static unsigned int iOff = -1;
	CBaseEntity *pEntity = m_pEnt->GetUnknown()->GetBaseEntity();
	if( iOff == -1 ) {
		iOff = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_OnUser2");
		if( iOff == 0 )
			printf("Could not find m_OnUser2");
	}

	return (eSpecMode) *(int *)((char *)pEntity + iOff);
	// return (g_pGameEnts->EdictToBaseEntity(m_pEnt))->m_OnUser2.m_Value.Int();
}

int CPlayer::getMoney() const {
	return *((float *) ((unsigned char *) (m_pEnt)->GetUnknown() + g_ServerPlugin.m_iOffsetMoney));
}


ePlayerTeam CPlayer::getTeam() const {
	int iTeam = g_pPlayerInfoMgr->GetPlayerInfo(m_pEnt)->GetTeamIndex();
	switch( iTeam ) {
		default:
			return team_unassigned;
		break;
		case 1:
			return team_spectator;
		break;
		case 2:
			return team_t;
		break;
		case 3:
			return team_ct;
		break;
	}
}

void CPlayer::printToConsole(const char *strMsg) const {
	g_pEngine->ClientPrintf(m_pEnt, strMsg);
}

void CPlayer::queryCvar(const char *strName) const {
	if(!isInGame() || isBot() || isHLTV())
		return;
	g_pServerPluginHelper->StartQueryCvarValue(m_pEnt, strName);
}

eSpecMode CPlayer::specMode() const {
	static unsigned int iOff = -1;
	CBaseEntity *pEntity = m_pEnt->GetUnknown()->GetBaseEntity();
	if( iOff == -1 ) {
		iOff = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_OnUser1");
		if( iOff == 0 )
			printf("Could not find m_OnUser1");
	}

	return (eSpecMode) *(int *)((char *)pEntity + iOff);

	// return (eSpecMode) (g_pGameEnts->EdictToBaseEntity(m_pEnt))->m_OnUser1.m_Value.Int();
}

CVector& CPlayer::getEyeOrigin() {
	g_pServerGameClients->ClientEarPosition(m_pEnt, (Vector *) &m_vecEyeOrigin);
	return m_vecEyeOrigin;
}

CVector& CPlayer::getVecAngle() {
	Vector buf;
	float *ang = ((float *) ((unsigned char *) (m_pEnt)->GetUnknown() + g_ServerPlugin.m_iOffsetEyeAngle));
	QAngle buf2(ang[0], ang[1], 0);
	AngleVectors(buf2, &buf);
	m_vecAngle = (CVector&) buf;
	return m_vecAngle;
}

CVector& CPlayer::getMins() {
	m_vecMins.m_fX = m_pEnt->GetCollideable()->OBBMins().x * 1.8;
	m_vecMins.m_fY = m_pEnt->GetCollideable()->OBBMins().y * 1.8;
	m_vecMins.m_fZ = m_pEnt->GetCollideable()->OBBMins().z;
	return m_vecMins;
}

CVector& CPlayer::getMaxs() {
	m_vecMaxs.m_fX = m_pEnt->GetCollideable()->OBBMaxs().x * 1.8;
	m_vecMaxs.m_fY = m_pEnt->GetCollideable()->OBBMaxs().y * 1.8;
	m_vecMaxs.m_fZ = m_pEnt->GetCollideable()->OBBMaxs().z * 1.2;
	return m_vecMaxs;
}

CVector& CPlayer::getOrigin() {
	// memcpy(&m_vecOrigin.m_fX, &m_pEnt->GetNetworkable()->GetBaseEntity()->m_vecAbsOrigin.x, 3*sizeof(float));
	m_vecOrigin = (CVector&) m_pEnt->GetCollideable()->GetCollisionOrigin();
	return m_vecOrigin;
}

CVector& CPlayer::getVelocity() {
	// memcpy(&m_vecVelocity.m_fX, &m_pEnt->GetNetworkable()->GetBaseEntity()->m_vecAbsVelocity.x, 3*sizeof(float));
	static unsigned int iOff = -1;
	CBaseEntity *pEntity = m_pEnt->GetUnknown()->GetBaseEntity();
	if( iOff == -1 ) {
		iOff = g_ServerPlugin.findInDataMap(pEntity->GetDataDescMap(), "m_vecSmoothedVelocity");
		if( iOff == 0 )
			printf("Could not find m_vecSmoothedVelocity");
	}

	m_vecVelocity.set((float *)((char *)pEntity + iOff));
	return m_vecVelocity;
}

const char *CPlayer::getNick() const {
	return (char *) g_pEngine->GetClientConVarValue(m_cIndex, "name");
}

