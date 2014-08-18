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

#ifndef PLAYERMANAGER_H_
#define PLAYERMANAGER_H_

#include "../IPlayerManager.h"
#include "Player.h"
#include <stdlib.h>

class CPlayerManager : public IPlayerManager {
private:
	CPlayer *m_aPlayer;
	size_t m_iIterator;

public:
	CPlayerManager() : m_aPlayer(NULL),  m_iIterator(0) { }
	virtual ~CPlayerManager() {
		unload();
	}

	bool load(bool bLate);
	void unload();
	void onTick();
	void onConnect(edict_t *pEdict);
	void onDisconnect(edict_t *pEdict);
	void onCommand(edict_t *pEdict);
	void onCvarValue(CPlayer *pPlayer, const char *strName, float flValue);
	CPlayer *getPlayer(edict_t *pEdict) const;
	virtual IPlayer *getPlayer(int iId) const {
		if( iId > m_iMaxClients || iId < 1 )
			return NULL;
		CPlayer *pPlayer = m_aPlayer+iId;
		if( !pPlayer->isInGame() )
			return NULL;
		return pPlayer;
	}
};

extern CPlayerManager g_PlayerManager;

#endif /* PLAYERMANAGER_H_ */
