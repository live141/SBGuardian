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

#ifndef ENGINE_H_
#define ENGINE_H_

#include "../IEngine.h"
#include "hl2.h"

class CEngine : public IEngine {
	friend class CServerPlugin;
	friend class CVSPCallback;
private:
	int m_iMsgText;
	int m_iMsgScreenFade;
	int m_iMsgMenu;
	CCommand *m_pCmd;

public:
	CEngine() : m_iMsgText(0), m_iMsgScreenFade(0), m_iMsgMenu(0), m_pCmd(NULL) { }
	virtual void command(const char *strCmd) const;
	virtual float time() const;
	virtual void registerCmd(const char *strCmd, void(*func)(void)) const;
	virtual IRayTrace *getRayTrace() const;
	virtual int argc() const;
	virtual const char *argv(int iIndex) const;
	virtual void printHud(const char *strText) const;
	virtual const char *gameDir() const;
	virtual void reloadPlugin(const char *strFileName = NULL) const;
	virtual eGame game() const;
	virtual double tickrate() const;
	virtual void recordDemo(const char *strName) const;
	virtual void restartRound() const;
	virtual void status() const;
	virtual void execute() const;
};

extern CEngine g_Engine;

#endif /* ENGINE_H_ */
