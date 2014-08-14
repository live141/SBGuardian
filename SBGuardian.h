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

#ifndef SBGUARDIAN_H_
#define SBGUARDIAN_H_

#include "IServerPlugin.h"
#include "IPlayer.h"
#include "IEngine.h"
#include "IPlayerManager.h"
#include "IModule.h"
#include "stdlib.h"
#include "modules/AntiWallhack/AntiWallhack.h"
#include "modules/AntiFlashhack/AntiFlashhack.h"
#include "modules/FOVCheck/FOVCheck.h"
#include "modules/CvarCheck/CvarCheck.h"
#include "modules/UpdateSystem/UpdateSystem.h"
#include "modules/AntiAimbot/AntiAimbot.h"
#include "modules/AntiSpeedhack/AntiSpeedhack.h"
#include "modules/Warmodus/Warmodus.h"

class CSBGuardian {
private:
	IPlayerManager *m_pPlayerManager;
	IEngine *m_pEngine;
	bool m_bLoaded;

	CAntiWallhack *m_pWh;
	CAntiFlashhack *m_pFlash;
	CFOVCheck *m_pFov;
	CCvarCheck *m_pCvar;
	CUpdateSystem *m_pUpdate;
	CAntiAimbot *m_pAb;
	CWarmodus *m_pWm;
	// CAntiSpeedhack *m_pSh;

	IModule *m_void; // always NULL

public:
	CSBGuardian() : m_pPlayerManager(NULL), m_pEngine(NULL), m_bLoaded(false), m_pWh(NULL),
	m_pFlash(NULL), m_pFov(NULL), m_pCvar(NULL), m_void(NULL), m_pUpdate(NULL), m_pAb(NULL)/*,
	m_pSh(NULL)*/ { }
	bool load(IPlayerManager *pPlayerManager, IEngine *pEngine);
	void unload();

	// hooks
	void onFrame();
	void onJoinGame(IPlayer *pPlayer);
	void onLeaveGame(IPlayer *pPlayer);
	void onCvarValue(IPlayer *pPlayer, const char *strName, const char *strValue, float flValue);
	bool onCheckTransmit(IPlayer *pPlayer, IPlayer *pEnt);
	bool onClientCommand(IPlayer *pPlayer);
	void onPlayerFlash(IPlayer *pPlayer, float flTime);
	void onCommand();
	void onMapChange();
	void onMove(IPlayer *pPlayer);
	void onTakeDamage(IPlayer *pVicitm, IPlayer *pAttacker);
	void onRoundStart();
	void onEmitSound(vector<IPlayer *> &vecRecipients, int iEntIndex, int iChannel, float flVolume, float flAttenuation, CVector *vecOrigin);
};

extern CSBGuardian g_SBG;

#endif /* SBGUARDIAN_H_ */
