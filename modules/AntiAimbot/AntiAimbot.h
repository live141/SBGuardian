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

#ifndef ANTIAIMBOT_H_
#define ANTIAIMBOT_H_

#include "../../IModule.h"
#include "../../CVector.h"

#define NUM_DATA 16

struct CVecAim {
public:
	CVector vecAim[NUM_DATA];
	CVector vecVelocity[NUM_DATA];
	float flDegree[NUM_DATA];
	float flDegreeVel[NUM_DATA];
	float flDegreeVelProjected[NUM_DATA];
	bool bCheat;

public:
	CVecAim() {
		clear();
	}

	void clear() {
		for( int i = NUM_DATA-1; i >= 0; i-- ) {
			vecAim[i] = CVector();
			vecVelocity[i] = CVector();
			flDegree[i] = 0.0;
			flDegreeVel[i] = 0.0;
		}
		bCheat = false;
	}
};

class CAntiAimbot : public IModule {
private:
	CVecAim *m_Aims;
	char m_cMode;

public:
	CAntiAimbot() : m_Aims(NULL), m_cMode(0) {
		m_pEngine = 0;
		m_pPlayerManager = 0;
	}
	virtual ~CAntiAimbot() {
		unload();
	}
	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	void onFrame();
	void onJoin(IPlayer *pPlayer);
	void onHit(IPlayer *pVictim, IPlayer *pAttacker, eHitBox hit);

	virtual const char *getName() const {
		return "AntiAimbot";
	}
};

#endif /* ANTIAIMBOT_H_ */
