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

#include "Engine.h"
#include "hl1.h"
#include "CRayTrace.h"
#include "ServerPlugin.h"

#ifndef STANDALONE
#include <meta_api.h>
#endif

CEngine g_Engine;

void CEngine::execute() const {
	g_EngFuncs.pfnServerExecute();
}

void CEngine::status() const {
	this->command("status\n");
	g_EngFuncs.pfnServerExecute();
}

void CEngine::restartRound() const {
	this->command("sv_restart 1\n");
}

void CEngine::recordDemo(const char *strName) const {
	
}

double CEngine::tickrate() const {
	cvar_t *pCvar = g_EngFuncs.pfnCVarGetPointer("sys_ticrate");
	return pCvar->value;
}

void CEngine::command(const char *strCmd) const {
	g_EngFuncs.pfnServerCommand((char *) strCmd);
}

float CEngine::time() const {
	return g_Globals->time;
}

void CEngine::registerCmd(const char *strCmd, void(*func)(void)) const {
	g_EngFuncs.pfnAddServerCommand((char *) strCmd, func);
}

IRayTrace *CEngine::getRayTrace() const {
	return new CRayTrace();
}

int CEngine::argc() const {
	return g_EngFuncs.pfnCmd_Argc();
}

const char *CEngine::argv(int iIndex) const {
	return g_EngFuncs.pfnCmd_Argv(iIndex);
}

void CEngine::reloadPlugin(const char *strFilename) const {
#ifndef STANDALONE
	char buf[128];
	snprintf(buf, sizeof(buf), "meta reload %s\n", Plugin_info.logtag);
	this->command(buf);
#endif
}

const char *CEngine::gameDir() const {
	static char strGameDir[512];
	g_EngFuncs.pfnGetGameDir(strGameDir);
	return strGameDir;
}

void CEngine::printHud(const char *strText) const {
	g_EngFuncs.pfnMessageBegin( MSG_ALL, m_iMsgText /*77 gmsgTextMsg*/, NULL, NULL );
	g_EngFuncs.pfnWriteByte( HUD_PRINTTALK );
	g_EngFuncs.pfnWriteString( strText );
	g_EngFuncs.pfnMessageEnd();
}

eGame CEngine::game() const {
	return g_ServerPlugin.m_eGame;
}

