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

#include "CRayTrace.h"
#include "Player.h"
#include "hl1.h"
#undef min
#undef max
#include "../profile.h"

//#define STRING(offset) (const char *)(g_Globals->pStringBase + (int)offset)

bool CRayTrace::traceWall(CVector& vecSrc, CVector& vecDest, IPlayer *pIgnore, vector<IPlayer *>& conHitHead, vector<IPlayer *>& conHitEnemy) {
	PROFILE_SETUP(RAYTRACE, 300);

	CVector vecDirection = vecDest - vecSrc;
	vecDirection.normalize();
	// m_vecDest = vecSrc + vecDirection * CVector((float *) &g_Globals->v_forward.x).len();
	m_vecDest = vecDest;
	m_conHead = &conHitHead;
	m_conFoe = &conHitEnemy;
	m_pIgnore = (CPlayer *) pIgnore;
	TraceResult tr;
	CPlayer *pPlayer = (CPlayer *) pIgnore;
	g_EngFuncs.pfnTraceLine(&vecSrc.m_f[0], &vecDest.m_f[0], 1, (edict_t *) pPlayer->getEdict(), &tr);
	m_flFraction = tr.flFraction;
	m_pHit = tr.pHit;
	m_vecEnd.set((float *) &tr.vecEndPos.x);
	m_iTraces = 1;

	if( tr.iHitgroup == 1 ) // head
		m_bHitHead = true;
	else
		m_bHitHead = false;

	if( m_flFraction < 1.0 && hitWorld() ) {
		PROFILE_UPDATE(RAYTRACE);
		PROFILE_PRINT(RAYTRACE);
		return true;
	}
	else {
		PROFILE_UPDATE(RAYTRACE);
		PROFILE_PRINT(RAYTRACE);
		return false;
	}
}

bool CRayTrace::_traceWall(CVector& vecSrc, CVector& vecDest, edict_t *pIgnore) {
	PROFILE_SETUP(RAYTRACE, 300);

	if( m_iTraces == MAX_TRACES )
		return true;
	m_iTraces++;
	TraceResult tr;
	g_EngFuncs.pfnTraceLine(&vecSrc.m_f[0], &vecDest.m_f[0], 1, pIgnore, &tr);
	m_flFraction = tr.flFraction;
	m_pHit = tr.pHit;
	m_vecEnd.set((float *) &tr.vecEndPos.x);

	if( tr.iHitgroup == 1 )
		m_bHitHead = true;
	else
		m_bHitHead = false;

	if( m_flFraction < 1.0 && hitWorld() ) {
		PROFILE_UPDATE(RAYTRACE);
		PROFILE_PRINT(RAYTRACE);
		return true;
	}
	else {
		PROFILE_UPDATE(RAYTRACE);
		PROFILE_PRINT(RAYTRACE);
		return false;
	}
}

bool CRayTrace::hitWorld() {
	if( m_pHit == NULL || !strcmp("worldspawn", STRING(m_pHit->v.classname)) ||
			!strcmp("func_door_rotating", STRING(m_pHit->v.classname)) )
		return true;

	if( !strcmp("player", STRING(m_pHit->v.classname)) ) {
		// return false;
		CPlayer *pHit = g_PlayerManager.getPlayer(m_pHit);

		if( pHit->getTeam() == m_pIgnore->getTeam() && m_bHitHead ) {
			// m_conHitHead->push_back(pHit);
			m_conHead->push_back(pHit);
		}
		else if( pHit->getTeam() != m_pIgnore->getTeam() ) {
			// m_conEnemy->push_back(pHit);
			m_conFoe->push_back(pHit);
		}
	}

	return _traceWall((CVector&) m_vecEnd, (CVector&) m_vecDest, (edict_t *) m_pHit);

	/*
	if(!!strcmp("player", STRING(ent->v.classname))) {
		return TraceWallIntern((float *) &m_vecEnd.x, (float *) &m_vecDest, ent);
	}
	else
		return false;
		*/
}
