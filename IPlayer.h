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

#ifndef IPLAYER_H_
#define IPLAYER_H_

#include "SteamId.h"
#include "CVector.h"

enum ePlayerTeam {
	team_unassigned = 0,
	team_t,
	team_ct,
	team_spectator
};

enum eSpecMode {
	spec_fp = 4
};

enum eHitBox {
	hit_generic = 0,
	hit_head,
	hit_chest
};

class IPlayer {
protected:
	CSteamId m_SteamId;
	char m_cIndex;
	bool m_bInGame;
	int m_iUserId;
	int m_iPing;
	float m_flInterp;
	bool m_bHLTV;
	CVector m_vecEyeOrigin;
	CVector m_vecAngle;
	CVector m_vecMins;
	CVector m_vecMaxs;
	CVector m_vecOrigin;
	CVector m_vecVelocity;

public:
	virtual ~IPlayer() { }
	virtual const char *getNick() const = 0;
	virtual bool isAlive() const = 0;
	virtual bool isSpec() const = 0;
	virtual bool isDying() const = 0;
	virtual bool isSolid() const = 0;
	virtual void kick(const char *strReason) const = 0;
	virtual void ban(const char *strReason) const = 0;
	virtual int getIdOfWatchedPlayer() const = 0;
	virtual ePlayerTeam getTeam() const = 0;
	virtual void printToConsole(const char *strMsg) const = 0;
	virtual void queryCvar(const char *strName) const = 0;
	virtual eSpecMode specMode() const = 0;
	virtual CVector& getEyeOrigin() = 0;
	virtual CVector& getVecAngle() = 0;
	virtual CVector& getMins() = 0;
	virtual CVector& getMaxs() = 0;
	virtual CVector& getOrigin() = 0;
	virtual CVector& getVelocity() = 0;
	virtual int getMoney() const = 0;

	CSteamId& getSteamId() {
		return m_SteamId;
	}

	unsigned int getIndex() const {
		return (unsigned int) m_cIndex;
	}

	int getUserId() const {
		return m_iUserId;
	}

	bool isInGame() const {
		return m_bInGame;
	}

	bool isHLTV() const {
		return m_bHLTV;
	}

	bool isBot() const {
		return ( !m_bHLTV && m_SteamId.getId() == 0 );
	}

	float getInterp() const {
		return m_flInterp;
	}

	int getPing() const {
		return m_iPing;
	}
};

#endif /* IPLAYER_H_ */
