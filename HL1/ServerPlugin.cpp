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

#include "ServerPlugin.h"
#include "../SBGuardian.h"
#include "PlayerManager.h"
#include "../version.h"
#include "../CStr.h"
#include "Engine.h"
#include "SrcHooks.h"

#ifndef STANDALONE
#define RETURN_VALUE(a, b, c) RETURN_META_VALUE(a, b)
#define RETURN(a, c) RETURN_META(a)
#else
#include <enginecallback.h>
#include <enginecallbacks.h>
#define MRES_IGNORED 1
#define MRES_SUPERCEDE 4
#define RETURN(a, c) do {\
	if(a == MRES_IGNORED) return c; \
	else return; } while(0);
#define RETURN_VALUE(a, b, c) do {\
	if(a == MRES_IGNORED) return c; \
	else return b; } while(0);
// #include <util.h>
#endif

#ifndef STANDALONE
#include <meta_api.h>
#endif

CServerPlugin g_ServerPlugin;
cvar_t g_CvarSbg = {"sbguardian_version", VERSION};

int CServerPlugin::m_iArg;
bool CServerPlugin::m_bMsgFlash;
float CServerPlugin::m_flHoldTime;
float CServerPlugin::m_flFullFlashTime;
edict_t *CServerPlugin::m_pReceiver;

void CServerPlugin::init(bool late) {
	const char *strCvar = CStr::format("sbguardian_version %s\n", VERSION);
	g_Engine.command(strCvar);

		// if(strcmp(STRING(gpGlobals->mapname), "de_dust2") == 0) g_CSBGuardian.SetCheckFuncWall(true);
		// else g_CSBGuardian.SetCheckFuncWall(false);
	if( m_bLoaded )
		return;

	#ifndef STANDALONE
	// for late load
	if( late ) {
		printf("[SBG] LateLoad\n");
	    int size;
	    g_Engine.m_iMsgText = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "TextMsg", &size);
	    g_Engine.m_iMsgScreenFade = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ScreenFade", &size);
	    g_Engine.m_iMsgMenu = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "ShowMenu", &size);
		g_Engine.m_iMsgHLTV = gpMetaUtilFuncs->pfnGetUserMsgID(PLID, "HLTV", &size);
		printf("MsgText:%d\nMsgScreenFade:%d\nMsgMenu:%d\nHLTV:%d\n", g_Engine.m_iMsgText, g_Engine.m_iMsgScreenFade, g_Engine.m_iMsgMenu, g_Engine.m_iMsgHLTV);
	}
	#endif

	g_CvarSbg.flags |= (FCVAR_SPONLY | FCVAR_SERVER);
	CVAR_REGISTER(&g_CvarSbg);
	g_Engine.command(strCvar);
	g_PlayerManager.load(late);
	g_SBG.load(&g_PlayerManager, &g_Engine);
	m_bRestartRound = false;

	g_Engine.registerCmd("sbguardian", &CServerPlugin::hookCmdDummy);

	const char *strGameDir = g_Engine.gameDir();

	if( !strcmp(strGameDir, "cstrike") ) {
		m_eGame = cstrike;
	}
	else if( !strcmp(strGameDir, "tfc") ) {
		m_eGame = tf;
	}
	else if( !strcmp(strGameDir, "dod") ) {
		m_eGame = dod;
	}
	else {
		m_eGame = other;
	}

	printf("SBGuardian loaded\n");

	m_bLoaded = true;

}

void CServerPlugin::unload() {
	m_bLoaded = false;
	g_SBG.unload();
	g_PlayerManager.unload();
}

void CServerPlugin::hookCmdDummy() {
	g_SBG.onCommand();
}

void CServerPlugin::GameFrame() {
	g_PlayerManager.onTick();
	g_SBG.onFrame();
	if( g_ServerPlugin.m_bRestartRound ) {
		g_ServerPlugin.m_bRestartRound = false;
		g_SBG.onRoundStart();
	}
	RETURN(MRES_IGNORED, g_FunctionTable.pfnStartFrame());
}

int ClientConnect(edict_s *pent, char const *s1, char const *s2, char *s3) {
	printf("SBGuardian: Client conenected!\n");
	RETURN_VALUE(MRES_IGNORED, g_FunctionTable.pfnClientConnect(pent, s1, s2, s3), 0);
}

void CServerPlugin::ClientPutInServer(edict_t *pent) {
	printf("SBGuardian: Client putin!\n");
	g_PlayerManager.onConnect(pent);
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pent);
	if( pPlayer != NULL )
		g_SBG.onJoinGame(pPlayer);
	RETURN(MRES_IGNORED, g_FunctionTable.pfnClientPutInServer(pent));
}

void CServerPlugin::ClientDisconnect(edict_t *pent) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pent);
	if( pPlayer != NULL )
		g_SBG.onJoinGame(pPlayer);
	g_PlayerManager.onDisconnect(pent);
	RETURN(MRES_IGNORED, g_FunctionTable.pfnClientDisconnect(pent));
}

void CServerPlugin::ClientCommand(edict_t *pent) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pent);
	if( pPlayer == NULL ) {
		RETURN(MRES_IGNORED, g_FunctionTable.pfnClientCommand(pent));
	}

	g_PlayerManager.onCommand(pent);

	if( g_SBG.onClientCommand(pPlayer) ) {
		RETURN(MRES_SUPERCEDE,);
	}

	RETURN(MRES_IGNORED, g_FunctionTable.pfnClientCommand(pent));
}

void CServerPlugin::onPM_Move(struct playermove_s *ppmove, int server) {
	CPlayer *pPlayer = (CPlayer *) g_PlayerManager.getPlayer(ppmove->player_index+1);
	printf("%u %f %f\n", ppmove->cmd.msec, ppmove->frametime, ppmove->friction);
	if( pPlayer != NULL )
		g_SBG.onMove(pPlayer);
	RETURN(MRES_IGNORED, g_FunctionTable.pfnPM_Move(ppmove, server));
}

void CServerPlugin::LevelInit() {

}

void CServerPlugin::ClientSettingsChanged(edict_t *pent, char *infobuffer) {

}

void CServerPlugin::OnQueryCvarValueFinished(const edict_t *pEnt, int requestID, const char *cvarName, const char *value) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer((edict_t *) pEnt);
	if( pPlayer != NULL && pPlayer->isInGame() && strncmp("bad", value, 3) ) {
		float flVal = atof(value);
		g_PlayerManager.onCvarValue(pPlayer, cvarName, flVal);
		g_SBG.onCvarValue(pPlayer, cvarName, value, flVal);
	}
	RETURN(MRES_SUPERCEDE, g_NewDllFuncs.pfnCvarValue2(pEnt, requestID, cvarName, value));
}

int CServerPlugin::CheckTransmit(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet) {
	if(player != 1 || host == NULL || ent == NULL) {
		RETURN_VALUE(MRES_IGNORED, 0, g_FunctionTable.pfnAddToFullPack(state, e, ent, host, hostflags, player, pSet));
	}

        // Don't send spectators to other players
		if ( ( ent->v.flags & FL_SPECTATOR ) && ( ent != host ) ) {
			RETURN_VALUE(MRES_SUPERCEDE, 0, 0);
		}

		// Ignore if not the host and not touching a PVS/PAS leaf
		// If pSet is NULL, then the test will always succeed and the entity will be added to the update
		if ( ent != host ) {
			if ( !g_EngFuncs.pfnCheckVisibility( (const struct edict_s *)ent, pSet ) ) {
				RETURN_VALUE(MRES_SUPERCEDE, 0, 0);
			}
		}

		if(!g_SBG.onCheckTransmit(g_PlayerManager.getPlayer(host), g_PlayerManager.getPlayer(ent))) {
	                RETURN_VALUE(MRES_SUPERCEDE, 0, 0);
		}
		RETURN_VALUE(MRES_IGNORED, 0, g_FunctionTable.pfnAddToFullPack(state, e, ent, host, hostflags, player, pSet));
}

void CServerPlugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {
	g_ServerPlugin.init(false);
	RETURN(MRES_IGNORED, g_FunctionTable.pfnServerActivate(pEdictList, edictCount, clientMax));
}

void CServerPlugin::ServerDeactivate() {
	// g_ServerPlugin.unload();
	if( g_ServerPlugin.isLoaded() )
		g_SBG.onMapChange();
	RETURN(MRES_IGNORED, g_FunctionTable.pfnServerDeactivate());
}

int CServerPlugin::RegUserMsg(const char *msg, int len) {
	int buf = g_engfuncs.pfnRegUserMsg(msg, len);
	if( strcmp(msg, "ShowMenu") == 0 ) {
	    g_Engine.m_iMsgMenu = buf;
	    printf("ShowMenu:%d\n", g_Engine.m_iMsgMenu);
	}
	else if( strcmp(msg, "TextMsg") == 0 ) {
	    g_Engine.m_iMsgText = buf;
	    printf("TextMsg:%d\n", g_Engine.m_iMsgText);
	}
	else if( strcmp(msg, "ScreenFade") == 0 ) {
	    g_Engine.m_iMsgScreenFade = buf;
	    printf("ScreenFade:%d\n", g_Engine.m_iMsgScreenFade);
	}
	else if( strcmp(msg, "HLTV") == 0 ) {
		g_Engine.m_iMsgHLTV = buf;
		printf("HLTV:%d\n", g_Engine.m_iMsgHLTV);
	}

	RETURN_VALUE(MRES_SUPERCEDE, buf, 0);
}

void CServerPlugin::TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr) {

}

void CServerPlugin::MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed) {
	g_ServerPlugin.m_Msg.init(msg_type, ed);

	if( msg_type == g_Engine.m_iMsgScreenFade /* 98 */ ) {
		m_iArg = 0;
		m_bMsgFlash = true;
		m_pReceiver = ed;
	}

	RETURN(MRES_IGNORED, g_EngFuncs.pfnMessageBegin(msg_dest, msg_type, pOrigin, ed));
}

void CServerPlugin::MessageEnd() {
	CMessage& Msg = g_ServerPlugin.m_Msg;
	if( m_bMsgFlash ) {
		m_bMsgFlash = false;
		CPlayer *pPlayer = g_PlayerManager.getPlayer(m_pReceiver);
		if( pPlayer != NULL )
			g_SBG.onPlayerFlash(pPlayer, m_flFullFlashTime);
	}
	else if( Msg.getMsg() == g_Engine.m_iMsgHLTV ) {
		// wait for next frame to make it "post hook"
		if( *((int *) Msg.arg(1)) == 0 && *((int *) Msg.arg(2)) == 0 )
			g_ServerPlugin.m_bRestartRound = true;
	}

	Msg.clear();

	RETURN(MRES_IGNORED, g_EngFuncs.pfnMessageEnd());
}

void CServerPlugin::WriteShort(int iValue) {
	if(m_bMsgFlash) {
		m_iArg++;
		if(m_iArg == 2) m_flHoldTime = (float) iValue;
	}

	g_ServerPlugin.m_Msg.add(new CMsgInt(iValue));

	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteShort(iValue));
}

void CServerPlugin::WriteByte(int iValue) {
	if(m_bMsgFlash)	{
		m_iArg++;
		if(m_iArg == 7 && iValue > 250) {
			m_flFullFlashTime = (float) ((float) m_flHoldTime/(1<<12) + g_Engine.time());
		}
	}
	
	g_ServerPlugin.m_Msg.add(new CMsgInt(iValue));

	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteByte(iValue));
}

void CServerPlugin::WriteString(const char *sz) {
	g_ServerPlugin.m_Msg.add(new CMsgString(sz));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteString(sz));
}

void CServerPlugin::WriteLong(int iValue) {
	g_ServerPlugin.m_Msg.add(new CMsgInt(iValue));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteLong(iValue));
}

void CServerPlugin::WriteAngle(float flValue) {
	g_ServerPlugin.m_Msg.add(new CMsgFloat(flValue));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteAngle(flValue));
}

void CServerPlugin::WriteEntity(int iValue) {
	g_ServerPlugin.m_Msg.add(new CMsgInt(iValue));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteEntity(iValue));
}

void CServerPlugin::WriteCoord(float flValue) {
	g_ServerPlugin.m_Msg.add(new CMsgFloat(flValue));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteCoord(flValue));
}

void CServerPlugin::WriteChar(int iValue) {
	g_ServerPlugin.m_Msg.add(new CMsgInt(iValue));
	RETURN(MRES_IGNORED, g_EngFuncs.pfnWriteChar(iValue));
}

