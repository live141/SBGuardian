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
#include "PlayerManager.h"

/***********************************************************/
CVSPCallback g_VSP;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CVSPCallback, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_VSP )
/***********************************************************/
#define GAMESERVERVERSION_MAX 99

#define MAKESTRING(a) #a

#define MAKE_VERSION(str, ver) MAKESTRING(str## ##ver)

inline void MAKEVERSION(int version, char *buf, char *strVersion)
{
	if(version < 10)
	{
		char *str = (char *) calloc(strlen(strVersion), sizeof(char));
		strncpy(str, strVersion, strlen(strVersion));
		str[strlen(strVersion)-1] = '\0';
		sprintf(buf, "%s%d\0", str, version);
		free(str);
	}
	else if(version < 100)
	{
		char *str = (char *) calloc(strlen(strVersion), sizeof(char));
		strncpy(str, strVersion, strlen(strVersion)-1);
		str[strlen(strVersion)-2] = '\0';
		sprintf(buf, "%s%d\0", str, version);
		free(str);
	}
}

#define GET_IFACE_ANY(getFactory, ptr, version, type)																			\
	do																															\
	{																															\
		int ver = GAMESERVERVERSION_MAX;																						\
		char *buf = (char *) calloc(strlen(version)+1, sizeof(char));															\
		while(ver >= 0)																											\
		{																														\
			MAKEVERSION(ver, buf, version);																						\
			ptr = (type*) getFactory(buf, NULL);																				\
			if(ptr != NULL)																										\
			{																													\
				printf("[SBG] found %s\n", buf);															\
				break;																											\
			}																													\
			ver--;																												\
		}																														\
																																\
		free(buf);																												\
	}																															\
	while(0);
/***********************************************************/
#include <sourcehook.h>
#include <sourcehook_impl.h>
static SourceHook::Impl::CSourceHookImpl s_SHImpl;
static SourceHook::ISourceHook *s_SHPtr = &s_SHImpl;
static int s_PluginId = 0;
/***********************************************************/
bool CVSPCallback::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
	bool bSuccess = true;
	g_ServerPlugin.m_bLoadedAsMM = false;
	g_SHPtr = s_SHPtr;
	SH_GLOB_PLUGPTR = s_PluginId;

	GET_IFACE_ANY(gameServerFactory, g_pServer, INTERFACEVERSION_SERVERGAMEDLL, IServerGameDLL);
	GET_IFACE_ANY(interfaceFactory, g_pEngine, INTERFACEVERSION_VENGINESERVER, IVEngineServer);
	GET_IFACE_ANY(gameServerFactory, g_pGameEnts, INTERFACEVERSION_SERVERGAMEENTS, IServerGameEnts);
	GET_IFACE_ANY(interfaceFactory, g_pEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER, IEngineTrace);
	GET_IFACE_ANY(interfaceFactory, g_pcvar, CVAR_INTERFACE_VERSION, ICvar);
	GET_IFACE_ANY(gameServerFactory, g_pServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS, IServerGameClients);
	GET_IFACE_ANY(interfaceFactory, g_pServerPluginHelper, INTERFACEVERSION_ISERVERPLUGINHELPERS, IServerPluginHelpers);
	GET_IFACE_ANY(interfaceFactory, g_pGameEvent, INTERFACEVERSION_GAMEEVENTSMANAGER2, IGameEventManager2);
	GET_IFACE_ANY(gameServerFactory, g_pPlayerInfoMgr, INTERFACEVERSION_PLAYERINFOMANAGER, IPlayerInfoManager);
	// GET_IFACE_ANY(gameServerFactory, g_pGameMovement, INTERFACENAME_GAMEMOVEMENT, IGameMovement);
	GET_IFACE_ANY(interfaceFactory, g_pEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION, IEngineSound);

	engine = g_pEngine;

	if(g_pEngine == NULL) {
		printf("[SBG] Could not get a pointer for g_pEngine\n");
		bSuccess = false;
	}

	if(g_pServer == NULL)	{
		printf("[SBG] Could not get a pointer for g_pServer\n");
		bSuccess = false;
	}

	if(g_pGameEnts == NULL) {
		printf("[SBG] Could not get a pointer for g_pGameEnts\n");
		bSuccess = false;
	}

	if(g_pEngineTrace == NULL) {
		printf("[SBG] Could not get a pointer for g_pEngineTrace\n");
		bSuccess = false;
	}

	if(g_pcvar == NULL) {
		printf("[SBG] Could not get a pointer for g_pcvar\n");
		bSuccess = false;
	}

	if(g_pServerGameClients == NULL) {
		printf("[SBG] Could not get a pointer for g_pServerGameClients\n");
		bSuccess = false;
	}

	if(g_pServerPluginHelper == NULL) {
		printf("[SBG] Could not get a pointer for g_pServerPluginHelper\n");
		bSuccess = false;
	}

	if(g_pGameEvent == NULL) {
		printf("[SBG] Could not get a pointer for g_pGameEvent\n");
		bSuccess = false;
	}

	if(g_pPlayerInfoMgr == NULL) {
		printf("[SBG] Could not get a pointer for g_pPlayerInfoMgr\n");
		bSuccess = false;
	}
/*
	if( g_pGameMovement == NULL ) {
		printf("[SBG] Could not get a pointer for g_pGameMovement\n");
		bSuccess = false;
	}
*/

	if( g_pEngineSound == NULL ) {
		printf("[SBG] Could not get a pointer for g_pEngineSound\n");
		bSuccess = false;
	}

	if(bSuccess) {
		g_pCVar = g_pcvar;
		ConVar_Register(0, &g_ServerPlugin);
		g_ServerPlugin.loadHooksVSP();

		g_pGlobals = g_pPlayerInfoMgr->GetGlobalVars();
		gpGlobals = g_pGlobals;

		/* late load */
		if(g_pGlobals->framecount > 1) {
			g_ServerPlugin.init(true);
		}

	}

	return bSuccess;
}

void CVSPCallback::Unload( void ) {
	g_ServerPlugin.unloadHooksVSP();
	g_ServerPlugin.Unload(NULL, 0);

}

void CVSPCallback::Pause( void ) {

}

void CVSPCallback::UnPause( void ) {

}

const char *CVSPCallback::GetPluginDescription( void ) {
	return g_ServerPlugin.GetDescription();
}

void CVSPCallback::LevelInit( char const *pMapName ) {

}

void CVSPCallback::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ) {
	g_ServerPlugin.init(false);
}

void CVSPCallback::GameFrame( bool simulating ) {
	g_PlayerManager.onTick();
	g_SBG.onFrame();
}

void CVSPCallback::LevelShutdown( void ) {

}

void CVSPCallback::ClientActive( edict_t *pEntity ) {

}

void CVSPCallback::ClientDisconnect( edict_t *pEntity ) {
	// g_SBG.onLeaveGame(g_PlayerManager.getPlayer(pEntity));
	// SH_REMOVE_HOOK(CBaseEntity, SetTransmit, pent->GetNetworkable()->GetBaseEntity(), &CServerPlugin::onSetTransmit, false);
	// g_PlayerManager.onDisconnect(pEntity);
}

void CVSPCallback::ClientPutInServer( edict_t *pEntity, char const *playername ) {
	// pEntity's membervars seems not to be accessable => crash

	/*
	if(pEntity == NULL) return;

	g_CPlayerManager.OnClientPutInServer(pEntity);
	// SH_ADD_HOOK(CBaseEntity, SetTransmit, pEntity->GetNetworkable()->GetBaseEntity(), &CServerPlugin::hooked_SetTransmit, false);
	CServerPlugin::LoadPlayerHooks(pEntity);
	*/
}

void CVSPCallback::SetCommandClient( int index ) {

}

void CVSPCallback::ClientSettingsChanged( edict_t *pEdict ) {

}

PLUGIN_RESULT CVSPCallback::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ) {
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT CVSPCallback::ClientCommand( edict_t *pEntity, const CCommand &args ) {
	g_Engine.m_pCmd = (CCommand *) &args;
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pEntity);

	if( pPlayer == NULL )
		return PLUGIN_CONTINUE;

	g_PlayerManager.onCommand(pEntity);

	if( g_SBG.onClientCommand(pPlayer) ) {
		return PLUGIN_STOP;
	}

	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT CVSPCallback::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) {
	return PLUGIN_CONTINUE;
}

void CVSPCallback::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pPlayerEntity);
	if( eStatus != 1 /* CvarNotFound */ && pPlayer != NULL ) {
		g_PlayerManager.onCvarValue(pPlayer, pCvarValue, atof(pCvarValue));
		g_SBG.onCvarValue(pPlayer, pCvarName, pCvarValue, atof(pCvarValue));
	}
}

