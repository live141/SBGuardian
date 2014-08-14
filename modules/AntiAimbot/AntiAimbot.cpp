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

#include "AntiAimbot.h"
#include "../../File.h"
#define _USE_MATH_DEFINES
#include <math.h>

void CAntiAimbot::onJoin(IPlayer *pPlayer) {
	m_Aims[pPlayer->getIndex()].clear();
}

void CAntiAimbot::onFrame() {
	if( !isEnabled() )
		return;

	for( short i = 1; i <= m_pPlayerManager->getMaxClients(); i++ ) {
		IPlayer *pPlayer = m_pPlayerManager->getPlayer(i);
		if( pPlayer == NULL || pPlayer->isBot() || pPlayer->isHLTV() )
			continue;

		if( !pPlayer->isAlive() ) {
			m_Aims[i].clear();
			continue;
		}

		CVecAim& Aim = m_Aims[i];

		for( int i = NUM_DATA-1; i > 0; i-- ) {
			Aim.vecAim[i] = Aim.vecAim[i-1];
			Aim.vecVelocity[i] = Aim.vecVelocity[i-1];
			Aim.flDegree[i] = Aim.flDegree[i-1];
			Aim.flDegreeVel[i] = Aim.flDegreeVel[i-1];
			Aim.flDegreeVelProjected[i] = Aim.flDegreeVelProjected[i-1];
		}

		Aim.vecAim[0] = pPlayer->getVecAngle();
		Aim.vecVelocity[0] = Aim.vecAim[0] - Aim.vecAim[1];
		if( Aim.vecAim[0] != Aim.vecAim[1] )
			Aim.flDegree[0] = 180/M_PI * acos((Aim.vecAim[0] * Aim.vecAim[1])/(Aim.vecAim[0].len() * Aim.vecAim[1].len()));
		else
			Aim.flDegree[0] = 0.0;
		if( Aim.vecVelocity[0] != CVector() && Aim.vecVelocity[1] != CVector() )
			Aim.flDegreeVel[0] = 180/M_PI * acos((Aim.vecVelocity[0] * Aim.vecVelocity[1])/(Aim.vecVelocity[0].len() * Aim.vecVelocity[1].len()));
		else
			Aim.flDegreeVel[0] = 0.0;

		CVector vecVel0 = Aim.vecVelocity[0];
		CVector vecVel1 = Aim.vecVelocity[1];
		CVector& vecNorm = Aim.vecAim[0];

		// project vecVel0 to plane
		float t = -(vecVel0 * vecNorm)/(vecNorm * vecNorm);
		vecVel0 = vecVel0 + t*vecNorm;

		t = -(vecVel1 * vecNorm)/(vecNorm * vecNorm);
		vecVel1 = vecVel1 + t*vecNorm;

		vecVel0.normalize();
		vecVel1.normalize();

		if( vecVel0 != CVector() && vecVel1 != CVector() )
			Aim.flDegreeVelProjected[0] = 180.0/M_PI * acos((vecVel0 * vecVel1));
		else
			Aim.flDegreeVelProjected[0] = 0.0;

		/*
		// ignore recently spawned/joined player
		if( Aim.vecAim[NUM_DATA-1] == CVector() || Aim.vecVelocity[1] == CVector() )
			continue;

		if( Aim.vecAim[0] != Aim.vecAim[1] ) {
			// m_pEngine->printHud(CStr::format("%s: %f° %f° VelLastLen: %f VelNewLen: %f\n", pPlayer->getNick(), Aim.flDegree[0], Aim.flDegreeVel[0], Aim.vecVelocity[1].len(), Aim.vecVelocity[0].len()));
			printf(CStr::format("%s: %f° %f° VelLastLen: %f VelNewLen: %f\n", pPlayer->getNick(), Aim.flDegree[0], Aim.flDegreeVel[0], Aim.vecVelocity[1].len(), Aim.vecVelocity[0].len()));
			if( Aim.flDegreeVel[0] > 120 && Aim.flDegree[0] >= Aim.flDegree[1]*0.9 && Aim.flDegree[0] > 8.0 && Aim.flDegreeVel[1] < 45.0 ) {
				Aim.bCheat = true;
				m_pEngine->printHud(CStr::format("[SBG]: %s is using an aimbot!\n", pPlayer->getNick()));
			}
		}
		*/

	}
}

void CAntiAimbot::onHit(IPlayer *pVictim, IPlayer *pAttacker, eHitBox hit) {
	if( !isEnabled() )
		return;

	if( pAttacker->getSteamId() == CSteamId() )
		return;
	short iIndex = pAttacker->getIndex();
	CVecAim& Aim = m_Aims[iIndex];

	// if( hit != hit_head && hit != hit_chest )
		// return;

	// printf("Attacker: %s Victim: %s\n", pAttacker->getNick(), pVictim->getNick());
	// ignore recently spawned/joined player
	// if( Aim.vecAim[NUM_DATA-1] == CVector() || Aim.vecVelocity[NUM_DATA-1] == CVector() )
		// return;

	for( int i = NUM_DATA-1; i > 0; i-- ) {
		if( Aim.vecAim[i-1] != Aim.vecAim[i] ) {
			// m_pEngine->printHud(CStr::format("%s: %f° %f° VelLastLen: %f VelNewLen: %f\n", pAttacker->getNick(), Aim.flDegree[i-1], Aim.flDegreeVelProjected[i-1], Aim.vecVelocity[i].len(), Aim.vecVelocity[i-1].len()));
			// printf(CStr::format("%s: %f° %f° VelLastLen: %f VelNewLen: %f\n", pAttacker->getNick(), Aim.flDegree[i-1], Aim.flDegreeVelProjected[i-1], Aim.vecVelocity[i].len(), Aim.vecVelocity[i-1].len()));
			if( Aim.flDegreeVelProjected[i-1] > 100.0 && Aim.flDegree[i-1] >= Aim.flDegree[i]*0.7 && Aim.flDegreeVelProjected[i] < 45.0 ) {
				Aim.bCheat = true;
				CFileMgr::log(this->getName(), CStr::format("%s(%s) is using an aimbot!\n", pAttacker->getNick(), pAttacker->getSteamId().getStatic()));
				m_pEngine->printHud(CStr::format("[SBG]: %s is using an aimbot!\n", pAttacker->getNick()));
				if( m_cMode == 1 ) {
					// kick
					pAttacker->kick("[SBG]: Kicked for using an Aimbot");
				}
				else if( m_cMode == 2 ) {
					// ban
					pAttacker->ban("[SBG]: Banned for using an Aimbot");
				}
				break;
			}
		}
	}
}

void CAntiAimbot::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;
	m_Aims = new CVecAim[pPlayerManager->getMaxClients()+1];
	m_cMode = 0;

	for(int i = 1; i <= m_pPlayerManager->getMaxClients(); i++) {
		m_Aims[i].clear();
	}

	setEnabled(true);
}

void CAntiAimbot::unload() {
	setEnabled(false);
	if( m_Aims != NULL )
		delete[] m_Aims;
	m_Aims = NULL;
}

bool CAntiAimbot::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "ab") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("AntiAimbot is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		int iMode = atoi(m_pEngine->argv(2));
		if( !isEnabled() && iMode > 0 ) {
			for(int i = 1; i <= m_pPlayerManager->getMaxClients(); i++) {
				m_Aims[i].clear();
			}
		}
		setEnabled(iMode);
		m_cMode = iMode;
	}

	return true;
}
