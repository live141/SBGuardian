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
#include <string.h>
#include "Offsets.h"
#include "Engine.h"
#include "../CStr.h"

// #define STRING(offset) (const char *)(g_Globals->pStringBase + (int)offset)

void CPlayer::onConnect(edict_t *pEdict) {
	m_pEnt = pEdict;
	m_cIndex = g_EngFuncs.pfnIndexOfEdict(pEdict);
	m_bHLTV = false;
	m_bHooked = false;
	m_strNickname = STRING(pEdict->v.netname);
	m_iUserId = GETPLAYERUSERID(pEdict);
	const char *strSteamId = GETPLAYERAUTHID(pEdict);
	m_SteamId = CSteamId();
	if( !strcmp(strSteamId, "HLTV") )
		m_bHLTV = true;
	else if( strcmp(strSteamId, "BOT") != 0 ) {
		m_SteamId = CSteamId(strSteamId);
	}

	m_bInGame = true;
}

void CPlayer::onDisconnect() {
	m_bInGame = false;
	m_pEnt = NULL;
	m_bHooked = false;
}

int CPlayer::getMoney() const {
	return *((int *) m_pEnt->pvPrivateData + OFFSET_CSMONEY);
}

ePlayerTeam CPlayer::getTeam() const {
	return (ePlayerTeam) *((int *) m_pEnt->pvPrivateData + OFFSET_TEAM);
}

bool CPlayer::isAlive() const {
	if( m_pEnt->v.deadflag == DEAD_NO && m_pEnt->v.health > 0 )
		return true;
	return false;
}

bool CPlayer::isSpec() const {
	if( m_pEnt->v.flags & FL_SPECTATOR )
		return true;
	return false;
}

bool CPlayer::isSolid() const {
	if( m_pEnt->v.solid == SOLID_SLIDEBOX )
		return true;
	return false;
}

bool CPlayer::isDying() const {
	return ( m_pEnt->v.deadflag == DEAD_DYING );
}

void CPlayer::kick(const char *strReason) const {
	char buf[128];
	snprintf(buf, sizeof(buf), "kick #%d \"%s\"\n", m_iUserId, strReason);
	g_Engine.command(buf);
}

void CPlayer::ban(const char *strReason) const {
	char buf[128];
	snprintf(buf, sizeof(buf), "banid #%d \"%s\"", m_iUserId, strReason);
	g_Engine.command(buf);
	this->kick(strReason);
}

int CPlayer::getIdOfWatchedPlayer() const {
	return ((edict_t *) m_pEnt)->v.iuser2;
}

void CPlayer::printToConsole(const char *strMsg) const {
	g_EngFuncs.pfnClientPrintf((edict_t *) m_pEnt, print_console, strMsg);
}

void CPlayer::queryCvar(const char *strName) const {
	g_EngFuncs.pfnQueryClientCvarValue2(m_pEnt, strName, 0);
}

eSpecMode CPlayer::specMode() const {
	return (eSpecMode) m_pEnt->v.iuser1;
}

CVector& CPlayer::getEyeOrigin() {
	m_vecEyeOrigin.set(m_pEnt->v.origin[0]+m_pEnt->v.view_ofs[0], m_pEnt->v.origin[1]+m_pEnt->v.view_ofs[1],
			m_pEnt->v.origin[2]+m_pEnt->v.view_ofs[2]);
	return m_vecEyeOrigin;
}

CVector& CPlayer::getVecAngle() {
	g_EngFuncs.pfnAngleVectors(&(m_pEnt->v.v_angle.x), &m_vecAngle.m_f[0], NULL, NULL);
	return m_vecAngle;
}

CVector& CPlayer::getMins() {
	m_vecMins.set((float *) &m_pEnt->v.mins.x);
	return m_vecMins;
}

CVector& CPlayer::getMaxs() {
	m_vecMaxs.set((float *) &m_pEnt->v.maxs.x);
	return m_vecMaxs;
}

CVector& CPlayer::getOrigin() {
	m_vecOrigin.set((float *) &m_pEnt->v.origin.x);
	return m_vecOrigin;
}

CVector& CPlayer::getVelocity() {
	m_vecVelocity.set((float *) &m_pEnt->v.velocity.x);
	return m_vecVelocity;
}
