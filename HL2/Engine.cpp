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
#include "ServerPlugin.h"
#include "../CStr.h"
#include <recipientfilter.h>
#include "CRayTrace.h"

CEngine g_Engine;

void CEngine::execute() const {
	g_pEngine->ServerExecute();
}

void CEngine::status() const {
	static ConCommand *pCvar = g_pcvar->FindCommand("status");
	static char strBuf[] = "status\n";
	const char *pBuf = strBuf;
	if( pCvar != NULL ) {
		pCvar->Dispatch(CCommand(1, &pBuf));
	}
}

void CEngine::restartRound() const {
	this->command("mp_restartgame 1\n");
}

void CEngine::recordDemo(const char *strName) const {
	
}

double CEngine::tickrate() const {
	return 1.0/g_pGlobals->interval_per_tick;
}

void CEngine::command(const char *strCmd) const {
	g_pEngine->ServerCommand(strCmd);
}

float CEngine::time() const {
	return g_pGlobals->curtime;
}

void CEngine::registerCmd(const char *strCmd, void(*func)(void)) const {

}

IRayTrace *CEngine::getRayTrace() const {
	return new CRayTrace();
	// return NULL;
}

int CEngine::argc() const {
	return m_pCmd->ArgC();
}

const char *CEngine::argv(int iIndex) const {
	return m_pCmd->Arg(iIndex);
}

void CEngine::printHud(const char *strText) const {
	CRecipientFilter filter;
	filter.AddAllPlayers();
#if SOURCE_ENGINE >= SE_CSGO
	bf_write *pWrite = g_pEngine->EntityMessageBegin(&filter, m_iMsgText, "SayText");
#elif SOURCE_ENGINE >= SE_LEFT4DEAD
	bf_write *pWrite = g_pEngine->UserMessageBegin(&filter, m_iMsgText, "SayText");
#else
	bf_write *pWrite = g_pEngine->UserMessageBegin(&filter, m_iMsgText /* 3 Say_Text */);
#endif
	pWrite->WriteByte(0); // From whom
	pWrite->WriteString(strText); // actual message
	pWrite->WriteByte(0);
	g_pEngine->MessageEnd();
}

eGame CEngine::game() const {
	return g_ServerPlugin.m_eGame;
}

const char *CEngine::gameDir() const {
	static char buf[512];
	memset(buf, 0, sizeof(buf));
	g_pEngine->GetGameDir(buf, sizeof(buf)-1);
	return buf;
}

void CEngine::reloadPlugin(const char *strFileName) const {
	CBuf buf;
	if( !g_ServerPlugin.isMM() ) {
		this->command(buf.thisFormat("plugin_unload \"%s\"; plugin_load \"%s\"\n", strFileName, strFileName));
	}
	else {
		this->command(buf.thisFormat("meta unload %d; meta load \"%s\"\n", g_PLID, strFileName));
	}
}
