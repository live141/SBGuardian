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

#ifndef ANTIWALLHACK_H_
#define ANTIWALLHACK_H_

#include "../../IModule.h"
#include "../../IPlayerManager.h"
#include "../../IRayTrace.h"
#include "../../Vector.h"
// #include <vector>

#define POS1 1 << 0
#define POS2 1 << 1
#define POS3 1 << 2
#define POS4 1 << 3
#define POS5 1 << 4
#define POS6 1 << 6
#define POS7 1 << 7
#define POS8 1 << 8
#define POS9 1 << 9
#define POS10 1 << 10
#define POS11 1 << 11
#define POS12 1 << 12
#define POS13 1 << 13

enum eCacheStatus {
	nothing = 0,
	notseeable,
	seeable,
	part_checked
};

class CCache {
	friend class CCacheMgr;
private:
	eCacheStatus m_cStatus;
	int m_iPosChecked;

public:
	CCache() : m_cStatus(notseeable), m_iPosChecked(0) { }
	void clear() {
		m_cStatus = nothing;
		m_iPosChecked = 0;
	}

	void setStatus(eCacheStatus status) {
		m_cStatus = status;
	}

	void addPos(int pos) {
		m_iPosChecked |= pos;
	}

	bool posChecked(int pos) {
		return ( m_iPosChecked & pos );
	}

	int getCheckedPos() const {
		return m_iPosChecked;
	}
};

class CCacheMgr {
private:
	int m_iMaxClients;
	CCache **m_Cache;
	int m_iPtr;

public:
	CCacheMgr() : m_iMaxClients(0), m_Cache(NULL), m_iPtr(1) { }
	~CCacheMgr() {
		if( m_Cache != NULL )
			delete[] m_Cache;
		m_Cache = NULL;
	}

	void init(int iMaxClients) {
		m_iMaxClients = iMaxClients;
		m_Cache = new CCache*[iMaxClients+1];
		for( int i = 0; i < iMaxClients+1; i++ )
			m_Cache[i] = new CCache[iMaxClients+1];
		m_iPtr = 1;
	}

	void add(IPlayer *pPlayer, IPlayer *pEnemy, int iPos, eCacheStatus cStatus);
	void proceed(vector<IPlayer *>& vecPlayers, vector<IPlayer *>& vecEnemies, int iPos);
	void clear();
	bool isSeeable(IPlayer *pPlayer, IPlayer *pEnemy) const;
	int checkedPos(IPlayer *pPlayer, IPlayer *pEnemy) const {
		return m_Cache[pPlayer->getIndex()][pEnemy->getIndex()].getCheckedPos();
	}
	bool isChecked(IPlayer *pPlayer, IPlayer *pEnemy) const {
		return ( m_Cache[pPlayer->getIndex()][pEnemy->getIndex()].m_cStatus != nothing );
	}
};

class CAntiWallhack : public IModule {
private:
	CCacheMgr m_Cache;

	void _interpolate(IPlayer *pPlayer, IPlayer *pEnemy, CVector *vecOrigin, CVector *vecEeye, bool bReceiver);

public:
	CAntiWallhack() { }
	virtual ~CAntiWallhack() {
		unload();
	}

	virtual const char *getName() const {
		return "AntiWH";
	}

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	void onFrame();
	bool isVisible(IPlayer *pPlayer, IPlayer *pEnemy);
	bool transmitSound(IPlayer *pPlayer, IPlayer *pEnt);
};

#endif /* ANTIWALLHACK_H_ */
