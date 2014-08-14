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

#ifndef ANTIFLASHHACK_H_
#define ANTIFLASHHACK_H_

#include "../../IModule.h"

class CAntiFlashhack : public IModule {
private:
	float *m_flFlashTill;

public:
	CAntiFlashhack() : m_flFlashTill(NULL) { }
	virtual ~CAntiFlashhack() {
		unload();
	}

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	bool isFlashed(IPlayer *pPlayer);
	void onJoin(IPlayer *pPlayer);
	void onLeave(IPlayer *pPlayer);
	void onFlash(IPlayer *pPlayer, float flTill);

	virtual const char *getName() const {
		return "AntiFlashhack";
	}
};

#endif /* ANTIFLASHHACK_H_ */
