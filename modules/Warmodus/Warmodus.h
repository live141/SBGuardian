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

#ifndef WARMODUS_H_
#define WARMODUS_H_

#include "../../IModule.h"

class CWarmodus : public IModule {
private:
	char m_c3rr;
	float m_flLastRR;
	IModule **m_pModule;

public:
	CWarmodus() : m_c3rr(0), m_flLastRR(0.0), m_pModule(NULL) { }
	CWarmodus(IModule **pModule) : m_c3rr(0), m_flLastRR(0.0), m_pModule(pModule) { }
	virtual ~CWarmodus() {
		unload();
	}

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	void onFrame();
	void onRestartRound();

	virtual const char *getName() const {
		return "Warmodus";
	}
};

#endif /* WARMODUS_H_ */
