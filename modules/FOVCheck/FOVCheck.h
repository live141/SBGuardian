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

#ifndef FOVCHECK_H_
#define FOVCHECK_H_

#include "../../IModule.h"

class CFOVCheck : public IModule {
private:

public:
	virtual ~CFOVCheck() {
		unload();
	}

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	bool isInVision(IPlayer *pPlayer, IPlayer *pEnemy);

	virtual const char *getName() const {
		return "FOVCheck";
	}
};

#endif /* FOVCHECK_H_ */
