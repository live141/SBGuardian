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
// #include "itempents.h"
#include "Player.h"
// #include "ServerPlugin.h"

extern IEngineTrace *g_pEngineTrace;

class CFilterWall : public ITraceFilter {
public:
	IHandleEntity *pIgnore;

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask ) {
		// edict_t *ent = INDEXENT(pServerEntity->GetRefEHandle().GetEntryIndex());
		if( pIgnore == pServerEntity /*|| !streq("worldspawn", ent->GetClassName())*/ )
			return false;
		return true;
	}

	virtual TraceType_t	GetTraceType() const {
		return TRACE_EVERYTHING;
	}
};

CFilterWall g_CFilterWall;

bool CRayTrace::traceWall(CVector& vecSrc, CVector& vecDest, IPlayer *pIgnore, vector<IPlayer *>& conHitHead, vector<IPlayer *>& conHitEnemy) {
	CVector vecDirection = vecDest - vecSrc;
	vecDirection.normalize();
	// m_vecDest = vecSrc + vecDirection * MAX_TRACE_LENGTH;
	m_vecDest = vecDest;
	m_conHead = &conHitHead;
	m_conFoe = &conHitEnemy;
	m_pIgnore = (CPlayer *) pIgnore;
	CPlayer *pPlayer = (CPlayer *) pIgnore;
	Ray_t ray;
	Vector src(vecSrc.m_f[0], vecSrc.m_f[1], vecSrc.m_f[2]), dest(vecDest.m_f[0], vecDest.m_f[1], vecDest.m_f[2]);
	ray.Init(src, dest);
	trace_t tr;
	g_CFilterWall.pIgnore = ((CPlayer *) pIgnore)->m_pEnt->GetUnknown()->GetBaseEntity()->GetCollideable()->GetEntityHandle();
	g_pEngineTrace->TraceRay(ray, MASK_SOLID, &g_CFilterWall, &tr);
	m_flFraction = tr.fraction;
	m_pHit = tr.m_pEnt;
	m_vecEnd.set((float *) &tr.endpos.x);
	m_iTraces = 1;

	if( tr.hitgroup == 1 ) // head
		m_bHitHead = true;
	else
		m_bHitHead = false;

	if( m_flFraction < 1.0 && hitWorld() )
		return true;
	else
		return false;
}

bool CRayTrace::_traceWall(CVector& vecSrc, CVector& vecDest, edict_t *pIgnore) {
	if( m_iTraces == MAX_TRACES )
		return true;
	m_iTraces++;
	Ray_t ray;
	Vector src(vecSrc.m_f[0], vecSrc.m_f[1], vecSrc.m_f[2]), dest(vecDest.m_f[0], vecDest.m_f[1], vecDest.m_f[2]);
	ray.Init(src, dest);
	trace_t tr;
	g_CFilterWall.pIgnore = pIgnore->GetUnknown()->GetBaseEntity()->GetCollideable()->GetEntityHandle();
	g_pEngineTrace->TraceRay(ray, MASK_SOLID, &g_CFilterWall, &tr);
	m_flFraction = tr.fraction;
	m_pHit = tr.m_pEnt;
	m_vecEnd.set((float *) &tr.endpos.x);

	if( tr.hitgroup == 1 )
		m_bHitHead = true;
	else
		m_bHitHead = false;

	if( m_flFraction < 1.0 && hitWorld() )
		return true;
	else
		return false;
}

bool CRayTrace::hitWorld() {
	if( m_pHit == NULL || !strcmp("worldspawn", m_pHit->GetClassname()) ||
			!strcmp("func_door_rotating", m_pHit->GetClassname()) )
		return true;

	if( !strcmp("player", m_pHit->GetClassname()) ) {
		// return false;
		CPlayer *pHit = (CPlayer *) g_PlayerManager.getPlayer((edict_t *) m_pHit->edict());

		if( pHit->getTeam() == m_pIgnore->getTeam() && m_bHitHead ) {
			// m_conHitHead->push_back(pHit);
			m_conHead->push_back(pHit);
		}
		else if( pHit->getTeam() != m_pIgnore->getTeam() ) {
			// m_conEnemy->push_back(pHit);
			m_conFoe->push_back(pHit);
		}
	}

	return _traceWall((CVector&) m_vecEnd, (CVector&) m_vecDest, m_pHit->edict());

	/*
	if(!!strcmp("player", STRING(ent->v.classname))) {
		return TraceWallIntern((float *) &m_vecEnd.x, (float *) &m_vecDest, ent);
	}
	else
		return false;
		*/
}

