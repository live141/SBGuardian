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

#ifndef IENGINE_H_
#define IENGINE_H_

#include "IRayTrace.h"

enum eGame {
	cstrike = 0,
	tf,
	dod,
	nucleardawn,
	other
};

class IEngine {
protected:

public:
	virtual void command(const char *strCmd) const = 0;
	virtual void registerCmd(const char *strCmd, void(*func)(void)) const = 0;
	virtual float time() const = 0;
	virtual IRayTrace *getRayTrace() const = 0;
	virtual int argc() const = 0;
	virtual const char *argv(int iIndex) const = 0;
	virtual void printHud(const char *strText) const = 0;
	virtual const char *gameDir() const = 0;
	virtual void reloadPlugin(const char *strFileName = NULL) const = 0;
	virtual eGame game() const = 0;
	virtual double tickrate() const = 0;
	virtual void recordDemo(const char *strName) const = 0;
	virtual void restartRound() const = 0;
	virtual void status() const = 0;
	virtual void execute() const = 0;
};

#endif /* IENGINE_H_ */
