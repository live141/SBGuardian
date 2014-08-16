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

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../IPlayer.h"
#include "hl2.h"

class CPlayer : public IPlayer {
	friend class CPlayerManager;
	friend class CServerPlugin;
	friend class CRayTrace;
private:
	edict_t *m_pEnt;
	bool m_bAuthed;

public:
	CPlayer() : m_pEnt(NULL), m_bAuthed(false) {
		m_bInGame = false;
	}
	CPlayer(edict_t *pEdict) : m_pEnt(pEdict) {
		onConnect(pEdict);
	}

	virtual ~CPlayer() {
		onDisconnect();
	}

	void onConnect(edict_t *pEdict);
	void onDisconnect();

	bool operator ==(CPlayer& pPlayer) const {
		return ( m_pEnt == pPlayer.m_pEnt );
	}

	virtual bool isAlive() const ;
	virtual bool isSpec() const;
	virtual bool isDying() const;
	virtual bool isSolid() const;
	virtual void kick(const char *strReason) const;
	virtual void ban(const char *strReason) const;
	virtual int getIdOfWatchedPlayer() const;
	virtual ePlayerTeam getTeam() const;
	virtual void printToConsole(const char *strMsg) const;
	virtual void queryCvar(const char *strName) const;
	virtual eSpecMode specMode() const;
	virtual CVector& getEyeOrigin();
	virtual CVector& getVecAngle();
	virtual CVector& getMins();
	virtual CVector& getMaxs();
	virtual CVector& getOrigin();
	virtual CVector& getVelocity();
	virtual const char *getNick() const;
	virtual int getMoney() const;

	const edict_t *getEdict() const {
		return m_pEnt;
	}
};

#endif /* PLAYER_H_ */
