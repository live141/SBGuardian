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

#include "AntiWallhack.h"
#include <stdio.h>
#include "../../profile.h"

void CCacheMgr::add(IPlayer *pPlayer, IPlayer *pEnemy, int iPos, eCacheStatus cStatus) {
	int iPlayer = pPlayer->getIndex();
	int iEnemy = pEnemy->getIndex();
	m_Cache[iPlayer][iEnemy].setStatus(cStatus);
	m_Cache[iPlayer][iEnemy].addPos(iPos);
}

void CCacheMgr::clear() {
	int iEnd = m_iPtr + m_iMaxClients/2;
	if( iEnd > m_iMaxClients )
		iEnd = m_iMaxClients;
	for( ; m_iPtr <= iEnd; m_iPtr++ ) {
		for( int j = 1; j <= m_iMaxClients; j++ )
			m_Cache[m_iPtr][j].clear();
	}
	if( m_iPtr > m_iMaxClients )
		m_iPtr = 1;
}

bool CCacheMgr::isSeeable(IPlayer *pPlayer, IPlayer *pEnemy) const {
	int iPlayer = pPlayer->getIndex();
	int iEnemy = pEnemy->getIndex();

	// printf("isSeeable: %d %d\n", iPlayer, iEnemy);
	if( m_Cache[iPlayer][iEnemy].m_cStatus == seeable || m_Cache[iEnemy][iPlayer].m_cStatus == seeable )
		return true;
	return false;
}

void CCacheMgr::proceed(vector<IPlayer *>& vecPlayers, vector<IPlayer *>& vecEnemies, int iPos) {
	vector<IPlayer *>::iterator itPlayer = vecPlayers.begin();
	for( ; itPlayer < vecPlayers.end(); itPlayer++ ) {
		for( vector<IPlayer *>::iterator itEnemy = vecEnemies.begin(); itEnemy < vecEnemies.end(); itEnemy++ ) {
			// 0 -> seeable
			if( iPos == 0 ) {
				m_Cache[(*itPlayer)->getIndex()][(*itEnemy)->getIndex()].m_cStatus = seeable;
			}
			else {
				m_Cache[(*itPlayer)->getIndex()][(*itEnemy)->getIndex()].m_cStatus = part_checked;
				m_Cache[(*itPlayer)->getIndex()][(*itEnemy)->getIndex()].addPos(iPos);
			}
		}
	}
}

void CAntiWallhack::_interpolate(IPlayer *pPlayer, IPlayer *pEnemy, CVector *vecOrigin, CVector *vecEye, bool bReceiver) {
	float ping  = pPlayer->getPing();
	if(ping > 200)
		ping = 200;
	float factor = ping/1000;
		if(bReceiver)
			factor += pPlayer->getInterp();

		#if SOURCE_ENGINE >= 4
		factor += pPlayer->getInterp();
		factor *= -1.1;
		#endif

		if( vecOrigin != NULL )
			*vecOrigin = pEnemy->getOrigin() + pEnemy->getVelocity() * factor;

		if( vecEye != NULL )
			*vecEye = pPlayer->getEyeOrigin() + pPlayer->getVelocity() * factor;

}

bool CAntiWallhack::transmitSound(IPlayer *pPlayer, IPlayer *pEnt) {
	return ( !m_Cache.isSeeable(pPlayer, pEnt) );
}

bool CAntiWallhack::isVisible(IPlayer *pPlayer, IPlayer *pEnemy) {
	if( !isEnabled() )
		return true;

	PROFILE_SETUP(isVisible, 400);

	if( m_Cache.isSeeable(pPlayer, pEnemy) )
		return true;
	if( m_Cache.isChecked(pPlayer, pEnemy) )
		return false;

	vector<IPlayer *> vecPlayers;
	vector<IPlayer *> vecEnemies;

	int poses = m_Cache.checkedPos(pPlayer, pEnemy);
	IRayTrace *trace = m_pEngine->getRayTrace();
	CVector vecIntOrigin;
	CVector vecIntEye;
	this->_interpolate(pPlayer, NULL, NULL, &vecIntEye, true);
	this->_interpolate(pPlayer, pEnemy, &vecIntOrigin, NULL, false);

	CVector targetMaxs = pEnemy->getMaxs() + pEnemy->getOrigin();
	CVector targetMins = pEnemy->getMins() + pEnemy->getOrigin();

	CVector Origin_PolOrigin(vecIntOrigin - CVector(pEnemy->getOrigin()));
	CVector MaxLen(pEnemy->getMaxs().m_f[0], pEnemy->getMaxs().m_f[1], 0);
	CVector IntPol, IntPol2, IntPol3, IntPol4;
	float flPolOriginLen = Origin_PolOrigin.len();

	if( flPolOriginLen != 0 )
		{
			IntPol = Origin_PolOrigin * (1.0 + MaxLen.len()/flPolOriginLen);
			IntPol3 = IntPol + CVector(pEnemy->getEyeOrigin());
			IntPol = IntPol + CVector(pEnemy->getOrigin()) + CVector(0, 0, pEnemy->getMins().m_f[2]);

			IntPol2 = Origin_PolOrigin * (-1);
			IntPol2.normalize();
			IntPol2 = IntPol2 * MaxLen.len();
			IntPol4 = IntPol2 + CVector(pEnemy->getEyeOrigin());
			IntPol2 = IntPol2 + CVector(pEnemy->getOrigin()) + CVector(0, 0, pEnemy->getMins().m_f[2]);

			vecPlayers.clear();
			vecEnemies.clear();

			if( !(poses & POS1) && !trace->traceWall(vecIntEye, IntPol, pPlayer, vecPlayers, vecEnemies) )
			{
				// add player only here to avoid problems with the cache
				vecPlayers.push_back(pPlayer);
				m_Cache.proceed(vecPlayers, vecEnemies, 0);
				PROFILE_UPDATE(isVisible);
				PROFILE_PRINT(isVisible);
				return true;
			}
			else {
				m_Cache.proceed(vecPlayers, vecEnemies, POS1);
			}

			vecPlayers.clear();
			vecEnemies.clear();

			if( !(poses & POS2) && !trace->traceWall(vecIntEye, IntPol3, pPlayer, vecPlayers, vecEnemies) )
			{
				vecPlayers.push_back(pPlayer);
				m_Cache.proceed(vecPlayers, vecEnemies, 0);
				PROFILE_UPDATE(isVisible);
				PROFILE_PRINT(isVisible);
				return true;
			}
			else {
				m_Cache.proceed(vecPlayers, vecEnemies, POS2);
			}

			vecPlayers.clear();
			vecEnemies.clear();

			if( !(poses & POS3) && !trace->traceWall(pPlayer->getEyeOrigin(), IntPol2, pPlayer, vecPlayers, vecEnemies) )
			{
				vecPlayers.push_back(pPlayer);
				m_Cache.proceed(vecPlayers, vecEnemies, 0);
				PROFILE_UPDATE(isVisible);
				PROFILE_PRINT(isVisible);
				return true;
			}
			else {
				m_Cache.proceed(vecPlayers, vecEnemies, POS3);
			}

			vecPlayers.clear();
			vecEnemies.clear();

			if( !(poses & POS4) && !trace->traceWall(pPlayer->getEyeOrigin(), IntPol4, pPlayer, vecPlayers, vecEnemies) )
			{
				vecPlayers.push_back(pPlayer);
				m_Cache.proceed(vecPlayers, vecEnemies, 0);
				PROFILE_UPDATE(isVisible);
				PROFILE_PRINT(isVisible);
				return true;
			}
			else {
				m_Cache.proceed(vecPlayers, vecEnemies, POS4);
			}
		}

		if( flPolOriginLen == 0 /* || LookupCacheOld(pPlayer->GetIndex(), pEnt->GetIndex()) != CANNOT_SEE_ENT */ )
		{
			for(int i = 0; i < 8; i++)
			{
				float m_flOut_NotInt[3];
				m_flOut_NotInt[0] = (i & 1) ? targetMins.m_f[0] : targetMaxs.m_f[0];
				m_flOut_NotInt[1] = (i & 2) ? targetMins.m_f[1] : targetMaxs.m_f[1];
				m_flOut_NotInt[2] = (i & 4) ? targetMins.m_f[2] : targetMaxs.m_f[2];

				vecPlayers.clear();
				vecEnemies.clear();

				CVector vecOut(m_flOut_NotInt);
				if( !(poses & 1<<(4+i)) && !trace->traceWall(vecIntEye, vecOut, pPlayer, vecPlayers, vecEnemies) )
				{
					vecPlayers.push_back(pPlayer);
					m_Cache.proceed(vecPlayers, vecEnemies, 0);
					PROFILE_UPDATE(isVisible);
					PROFILE_PRINT(isVisible);
					return true;
				}
				else {
					m_Cache.proceed(vecPlayers, vecEnemies, 1<<(4+i));
				}
			}
		}

		vecPlayers.clear();
		vecEnemies.clear();

		if( !(poses & POS13) && !trace->traceWall(pPlayer->getEyeOrigin(), pEnemy->getOrigin(), pPlayer, vecPlayers, vecEnemies) )
		{
			vecPlayers.push_back(pPlayer);
			m_Cache.proceed(vecPlayers, vecEnemies, 0);
			PROFILE_UPDATE(isVisible);
			PROFILE_PRINT(isVisible);
			return true;
		}
		else {
			m_Cache.proceed(vecPlayers, vecEnemies, POS13);
		}

	delete trace;
	trace = NULL;

	// check if players are behind the one that is behind a wall and put them into cache

	PROFILE_UPDATE(isVisible);
	PROFILE_PRINT(isVisible);
	return false;
}

void CAntiWallhack::onFrame() {
	m_Cache.clear();
}

void CAntiWallhack::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;
	m_Cache.init(pPlayerManager->getMaxClients());

	setEnabled(true);
}

void CAntiWallhack::unload() {
	setEnabled(false);
}

bool CAntiWallhack::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "wh") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("AntiWH is %s\n", (isEnabled())?"enabled":"disabled");
		printf("%s", strText);
	}
	else {
		setEnabled(atoi(m_pEngine->argv(2)));
	}

	return true;
}

