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

#ifndef IMODULE_H_
#define IMODULE_H_

#include "CommandCallback.h"
#include "IEngine.h"
#include "IPlayerManager.h"
#include "CStr.h"

class IModule {
protected:
	IEngine *m_pEngine;
	IPlayerManager *m_pPlayerManager;
	bool m_bEnabled;

public:
	virtual ~IModule() { }
	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager) = 0;
	virtual void unload() = 0;
	virtual bool onCommand() = 0;
	virtual const char *getName() const = 0;

	void setEnabled(bool bEnabled) {
		m_bEnabled = bEnabled;
	}

	bool isEnabled() const {
		return m_bEnabled;
	}

};

#endif /* IMODULE_H_ */
