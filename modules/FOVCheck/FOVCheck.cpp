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

#include "FOVCheck.h"
#include "../../CVector.h"
#include <stdio.h>
#include <mathlib.h>

#define _abs(num) (num > 0)?num:-num

void CFOVCheck::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;

	setEnabled(true);
}

void CFOVCheck::unload() {
	setEnabled(false);
}

bool CFOVCheck::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "fov") )
		return false;

	if( m_pEngine->argc() == 2 ) {
		const char *strText = CStr::format("FOVCheck is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}
	else {
		setEnabled(atoi(m_pEngine->argv(2)));
	}

	return true;
}

bool CFOVCheck::isInVision(IPlayer *pPlayer, IPlayer *pEnemy) {
	if( !isEnabled() )
		return true;

	CVector vecEnemyPlayer(pEnemy->getOrigin() - pPlayer->getEyeOrigin());
	CVector vecAngle(pPlayer->getVecAngle());
	vecEnemyPlayer.m_fZ = 0.0;
	vecAngle.m_fZ = 0.0;
	float flLen1 = vecEnemyPlayer.len();
	float flLen2 = vecAngle.len();
	if( flLen1 == 0 || flLen2 == 0 )
		return true;

	float flDegree = acos(vecAngle*vecEnemyPlayer*(1/(flLen1 * flLen2))) * 180/M_PI;
	// printf("%s (%f, %f, %f), (%f, %f, %f)\n", pPlayer->getNick(), vecAngle.m_fX, vecAngle.m_fY, vecAngle.m_fZ,
			// vecEnemyPlayer.m_fX, vecEnemyPlayer.m_fY, vecEnemyPlayer.m_fZ);
	// printf("%s %f\n", pPlayer->getNick(), flDegree);
	if( flDegree < 95.0 && flDegree > -95.0)
		return true;
	return false;
}
