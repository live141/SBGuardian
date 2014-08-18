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

#ifndef ANTISPEEDHACK_H_
#define ANTISPEEDHACK_H_

#include "../../IModule.h"

class CAntiSpeedhack : public IModule {
private:
	int *m_iTicks;

public:
	CAntiSpeedhack() : m_iTicks(NULL) { }
	virtual ~CAntiSpeedhack() {
		unload();
	}

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	void onMove(IPlayer *pPlayer);
	void onFrame();

	virtual const char *getName() const {
		return "AntiSpeedhack";
	}

};

#endif /* ANTISPEEDHACK_H_ */
