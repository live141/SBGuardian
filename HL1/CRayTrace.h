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

#ifndef CRAYTRACE_H_
#define CRAYTRACE_H_

#include "../IRayTrace.h"
#include "../CVector.h"
#include "../Vector.h"
#include "hl1.h"
#include "PlayerManager.h"
// #include <vector>

#define MAX_TRACES 5

class CRayTrace : public IRayTrace {
private:
	CVector m_vecStart;
	CVector m_vecEnd;
	CVector m_vecDest;
	float m_flFraction;
	int m_iTraces;
	edict_t *m_pHit;
	CPlayer *m_pIgnore;
	bool m_bHitHead;
	vector<IPlayer *> *m_conHead;
	vector<IPlayer *> *m_conFoe;

	bool _traceWall(CVector& vecSrc, CVector& vecDest, edict_t *pIgnore);

public:
	CRayTrace() : m_vecStart(0,0,0), m_vecEnd(0,0,0), m_vecDest(0,0,0), m_flFraction(0.0), m_iTraces(0), m_pHit(NULL),
	m_pIgnore(NULL), m_bHitHead(false), m_conHead(NULL), m_conFoe(NULL) { }
	virtual bool traceWall(CVector& vecSrc, CVector& vecDest, IPlayer *pIgnore, vector<IPlayer *>& conHitHead, vector<IPlayer *>& conHitEnemy);
	bool hitWorld();
};

#endif /* CRAYTRACE_H_ */
