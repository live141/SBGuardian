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

#if SOURCE_ENGINE >= SE_CSGO
#include "google/protobuf/message.h"
#endif
#include "ServerPlugin.h"
#include "../version.h"
#include "PlayerManager.h"
#include "TakeDamageInfoHack.h"
#include "recipientfilter.h"

CServerPlugin g_ServerPlugin;
ConVar g_Sbg("sbguardian_version", VERSION, FCVAR_NOTIFY | FCVAR_REPLICATED);
ConCommand g_CmdSbg("sbguardian", CServerPlugin::hookCmdDummy);
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CServerPlugin, ISmmPlugin, "ISmmPlugin", g_ServerPlugin );
/********************************************************************************************************/
IServerGameDLL *g_pServer = NULL;
IEngineTrace *g_pEngineTrace = NULL;
IVEngineServer *g_pEngine = NULL;
IVEngineServer *engine = NULL;
IServerGameEnts *g_pGameEnts = NULL;
IServerGameClients *g_pServerGameClients = NULL;
IGameEventManager2 *g_pGameEvent = NULL;
IServerPluginCallbacks *g_pServerPluginCallback = NULL;
IServerPluginHelpers *g_pServerPluginHelper = NULL;
ICvar *g_pcvar = NULL;
ICvar *g_pCvar = NULL;
IPlayerInfoManager *g_pPlayerInfoMgr = NULL;
// IGameMovement *g_pGameMovement = NULL;
IEngineSound *g_pEngineSound = NULL;
CGlobalVars *g_pGlobals= NULL;
CGlobalVars *gpGlobals = NULL;
ISmmAPI *g_SMAPI = NULL;
ISmmPlugin *g_PLAPI = NULL;
PluginId g_PLID = (PluginId)0;
SourceHook::ISourceHook *g_SHPtr = NULL;
/********************************************************************************************************/
/* declare hooks */
SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t *, int, int);
SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, 0);
SH_DECL_HOOK2_void(CBaseEntity, SetTransmit, SH_NOATTRIB, 0, CCheckTransmitInfo *, bool);
SH_DECL_HOOK2_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, edict_t *, const char *);
SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *, const CCommand&);
SH_DECL_HOOK1_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, edict_t *);
SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool);
SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, 0, const CCommand &);
SH_DECL_HOOK5_void(IServerPluginCallbacks, OnQueryCvarValueFinished, SH_NOATTRIB, 0, QueryCvarCookie_t , edict_t *, EQueryCvarValueStatus , const char *, const char *);
// SH_DECL_HOOK2_void(IGameMovement, ProcessMovement, SH_NOATTRIB, 0, CBasePlayer *, CMoveData *);
SH_DECL_HOOK3_void(CBasePlayer, TraceAttack, SH_NOATTRIB, 0, const CTakeDamageInfo &, const Vector&, trace_t *);
#if SOURCE_ENGINE >= SE_CSGO
SH_DECL_HOOK3_void(IVEngineServer, SendUserMessage, SH_NOATTRIB, 0, IRecipientFilter&, int, const google::protobuf::Message&);
#else
SH_DECL_HOOK2(IVEngineServer, UserMessageBegin, SH_NOATTRIB, 0, bf_write *, IRecipientFilter *, int);
#endif
SH_DECL_HOOK0_void(IVEngineServer, MessageEnd, SH_NOATTRIB, 0);
//SH_DECL_HOOK15_void(IEngineSound, EmitSound, SH_NOATTRIB, 0, IRecipientFilter&, int, int, const char *, float, soundlevel_t, int, int, int, const Vector*, const Vector*, CUtlVector< Vector >*, bool, float, int);
//SH_DECL_HOOK15_void(IEngineSound, EmitSound, SH_NOATTRIB, 1, IRecipientFilter&, int, int, const char *, float, soundlevel_t, int, int, int, const Vector*, const Vector*, CUtlVector< Vector >*, bool, float, int);
/********************************************************************************************************/

int CServerPlugin::findInSendTable(const char *ClassName, const char* PropName) {
	ServerClass *serverClass = g_pServer->GetAllServerClasses();

	while( serverClass ) {
		if( strcmp(serverClass->GetName(), ClassName ) == 0) {
			for( int i = 0; i < serverClass->m_pTable->GetNumProps(); i++ ) {
				if( strcmp(PropName, serverClass->m_pTable->m_pProps[i].GetName()) == 0 ) {
					printf("[SBG]: Found %s in %s\n", PropName, ClassName);
					return serverClass->m_pTable->m_pProps[i].GetOffset();
				}
			}
		}

		serverClass = serverClass->m_pNext;
	}
	printf("[SBG]: Error: Could not find %s in %s\n", PropName, ClassName);
	return -1;
}

unsigned int CServerPlugin::findInDataMap(datamap_t *pMap, const char *strName) {
	while (pMap) {
		for ( int i = 0; i < pMap->dataNumFields; i++ ) {
			if (pMap->dataDesc[i].fieldName == NULL) {
				continue;
			}
			if ( !strcmp(strName, pMap->dataDesc[i].fieldName) ) {
#if SOURCE_ENGINE >= SE_LEFT4DEAD
				return pMap->dataDesc[i].fieldOffset;
#else
				return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];
#endif
			}
			if (pMap->dataDesc[i].td) {
				unsigned int offset;
				if ( (offset = findInDataMap(pMap->dataDesc[i].td, strName)) != 0 ) {
					return offset;
				}
			}
		}
		pMap = pMap->baseMap;
	}

	// printf("[SBG]: Error: Could not find %s\n", strName);
	return 0;
}

void CServerPlugin::printSendTable() const {
	ServerClass *serverClass = g_pServer->GetAllServerClasses();

	while( serverClass ) {
		for( int i = 0; i < serverClass->m_pTable->GetNumProps(); i++ ) {
			printf("%s %s\n", serverClass->GetName(), serverClass->m_pTable->m_pProps[i].GetName());
		}

		serverClass = serverClass->m_pNext;
	}
}

void CServerPlugin::printDataMap(datamap_t *pMap) const {
	while (pMap) {
		for ( int i = 0; i < pMap->dataNumFields; i++ ) {
			if (pMap->dataDesc[i].fieldName == NULL) {
				continue;
			}

			printf("%s\n", pMap->dataDesc[i].fieldName);
			if (pMap->dataDesc[i].td) {
				printDataMap(pMap->dataDesc[i].td);
			}
		}
		pMap = pMap->baseMap;
	}
}

void CServerPlugin::hookCmdDummy(const CCommand &command) {
	g_SBG.onCommand();
}

void CServerPlugin::FireGameEvent(IGameEvent *event) {
	const char *strEvent = event->GetName();
	if( !strcmp(strEvent, "player_blind") ) {
		int iUserId = event->GetInt("userid");
		CPlayer *pPlayer = g_PlayerManager.getPlayerByUserId(iUserId);
		if( pPlayer == NULL )
			return;
		float alpha = *((float *) ((unsigned char *) ((edict_t *) pPlayer->m_pEnt)->GetUnknown() + m_iOffsetFlashMaxAlpha));
		float duration = *((float *) ((unsigned char *) ((edict_t *) pPlayer->m_pEnt)->GetUnknown() + m_iOffsetFlashDuration));
		if( alpha == 255 ) {
			m_vecPlayersFlashed.push_back(pPlayer);
			m_vecDuration.push_back(duration);
		}
		return;
	}

	if( !strcmp(strEvent, "flashbang_detonate") ) {
		while( !m_vecPlayersFlashed.empty() ) {
			IPlayer *pPlayer = *m_vecPlayersFlashed.back();
			float flDuration = *m_vecDuration.back();
			CVector vecDetonation(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
			// printf("BANG: %f %f %f\n", vecDetonation.m_fX, vecDetonation.m_fY, vecDetonation.m_fZ);
			CVector vecLen = pPlayer->getEyeOrigin() - vecDetonation;
			// printf("duration: %f durvar: %f\n", (1<<SCREENFADE_FRACBITS)/vecLen.len(), flDuration);
			g_SBG.onPlayerFlash(pPlayer, g_pGlobals->curtime + (1<<SCREENFADE_FRACBITS)/vecLen.len());
			m_vecPlayersFlashed.pop_back();
			m_vecDuration.pop_back();
		}
		
		return;
	}

	if( !strcmp(strEvent, "round_start") ) {
		g_SBG.onRoundStart();
		return;
	}
}

bool CServerPlugin::RegisterConCommandBase(ConCommandBase *pCommandBase) {
	if( !m_bLoadedAsMM ) {
		g_pcvar->RegisterConCommand(pCommandBase);
		return true;
	}
	else
		return META_REGCVAR(pCommandBase);
}

void CServerPlugin::OnVSPListening(IServerPluginCallbacks *iface) {
	g_pServerPluginCallback = iface;
	SH_ADD_HOOK(IServerPluginCallbacks, OnQueryCvarValueFinished, g_pServerPluginCallback, &CServerPlugin::onQueryCvarValueFinished, false);
}

void CServerPlugin::loadPlayerHooks(edict_t *pEntity) {
	SH_ADD_HOOK(CBasePlayer, TraceAttack, (CBasePlayer *) pEntity->GetUnknown()->GetBaseEntity(), &CServerPlugin::onTraceAttack, false);
	SH_ADD_HOOK(CBaseEntity, SetTransmit, pEntity->GetNetworkable()->GetBaseEntity(), &CServerPlugin::onSetTransmit, false);
}

void CServerPlugin::loadHooks() {
	SH_ADD_HOOK(IServerGameClients, ClientCommand, g_pServerGameClients, &CServerPlugin::onClientCommand, false);
	// SH_ADD_HOOK(IServerGameClients, ClientDisconnect, g_pServerGameClients, &CServerPlugin::onClientDisconnect, false);
	SH_ADD_HOOK(IServerGameDLL, GameFrame, g_pServer, &CServerPlugin::onFrame, false);
	SH_ADD_HOOK(IServerGameDLL, ServerActivate, g_pServer, &CServerPlugin::onServerActivate, false);
	// SH_ADD_HOOK(IGameMovement, ProcessMovement, g_pGameMovement, &CServerPlugin::onProcessMove, false);
#if SOURCE_ENGINE >= SE_CSGO
	SH_ADD_HOOK(IVEngineServer, SendUserMessage, g_pEngine, &CServerPlugin::onSendUserMessage, false);
#else
	SH_ADD_HOOK(IVEngineServer, UserMessageBegin, g_pEngine, &CServerPlugin::onUserMessageBegin, false);
#endif
	SH_ADD_HOOK(IVEngineServer, MessageEnd, g_pEngine, &CServerPlugin::onMessageEnd, false);
	//SH_ADD_HOOK(IEngineSound, EmitSound, g_pEngineSound, &CServerPlugin::onEmitSound, false);
	//SH_ADD_HOOK(IEngineSound, EmitSound, g_pEngineSound, &CServerPlugin::onEmitSound2, false);
	loadHooksVSP();
}

void CServerPlugin::loadHooksVSP() {
	SH_ADD_HOOK(IServerGameClients, ClientPutInServer, g_pServerGameClients, &CServerPlugin::onClientPutInServer, false);
	SH_ADD_HOOK(IServerGameDLL, LevelShutdown, g_pServer, &CServerPlugin::onServerDeactivate, false);
	SH_ADD_HOOK(IServerGameClients, ClientDisconnect, g_pServerGameClients, &CServerPlugin::onClientDisconnect, false);
	g_pGameEvent->AddListener(this, "flashbang_detonate", true);
	g_pGameEvent->AddListener(this, "player_blind", true);
	g_pGameEvent->AddListener(this, "round_start", true);
}

void CServerPlugin::unloadHooks() {
	SH_REMOVE_HOOK(IServerGameClients, ClientCommand, g_pServerGameClients, &CServerPlugin::onClientCommand, false);
	// SH_REMOVE_HOOK(IServerGameClients, ClientDisconnect, g_pServerGameClients, &CServerPlugin::onClientDisconnect, false);
	SH_REMOVE_HOOK(IServerGameDLL, GameFrame, g_pServer, &CServerPlugin::onFrame, false);
	SH_REMOVE_HOOK(IServerGameDLL, ServerActivate, g_pServer, &CServerPlugin::onServerActivate, false);
	// SH_REMOVE_HOOK(IGameMovement, ProcessMovement, g_pGameMovement, &CServerPlugin::onProcessMove, false);
#if SOURCE_ENGINE >= SE_CSGO
	SH_REMOVE_HOOK(IVEngineServer, SendUserMessage, g_pEngine, &CServerPlugin::onSendUserMessage, false);
#else
	SH_REMOVE_HOOK(IVEngineServer, UserMessageBegin, g_pEngine, &CServerPlugin::onUserMessageBegin, false);
#endif
	SH_REMOVE_HOOK(IVEngineServer, MessageEnd, g_pEngine, &CServerPlugin::onMessageEnd, false);
	//SH_REMOVE_HOOK(IEngineSound, EmitSound, g_pEngineSound, &CServerPlugin::onEmitSound, false);
	//SH_REMOVE_HOOK(IEngineSound, EmitSound, g_pEngineSound, &CServerPlugin::onEmitSound2, false);
}

void CServerPlugin::unloadHooksVSP() {
	SH_REMOVE_HOOK(IServerGameClients, ClientPutInServer, g_pServerGameClients, &CServerPlugin::onClientPutInServer, false);
	SH_REMOVE_HOOK(IServerGameDLL, LevelShutdown, g_pServer, &CServerPlugin::onServerDeactivate, false);
	SH_REMOVE_HOOK(IServerGameClients, ClientDisconnect, g_pServerGameClients, &CServerPlugin::onClientDisconnect, false);
	g_pGameEvent->RemoveListener(this);
}

void CServerPlugin::init(bool bLate) {
	const char *strCvar = CStr::format("sbguardian_version %s\n", VERSION);
	g_Engine.command(strCvar);

	if( m_bLoaded )
		return;


	const char *strGameDir = g_Engine.gameDir();
	printf("Using Game: %s\n", strGameDir);

	m_iOffsetFOV = this->findInSendTable("CBasePlayer", "m_iFOV");

	if( strstr(strGameDir, "/cstrike") ) {
		m_iOffsetFlashMaxAlpha = this->findInSendTable("CCSPlayer", "m_flFlashMaxAlpha");
		m_iOffsetFlashDuration = this->findInSendTable("CCSPlayer", "m_flFlashDuration");
		m_iOffsetAngVel = this->findInSendTable("CCSPlayer", "m_angRotation");
		m_iOffsetEyeAngle = this->findInSendTable("CCSPlayer", "m_angEyeAngles[0]");
		m_iOffsetMoney = this->findInSendTable("CCSPlayer", "m_iAccount");
		m_eGame = cstrike;
	}
	else if( strstr(strGameDir, "/tf") ) {
		m_iOffsetAngVel = this->findInSendTable("CTFPlayer", "m_angRotation");
		m_iOffsetEyeAngle = this->findInSendTable("CTFPlayer", "m_angEyeAngles[0]");
		m_eGame = tf;
	}
	else if( strstr(strGameDir, "/dods") ) {
		m_iOffsetAngVel = this->findInSendTable("CDODPlayer", "m_angRotation");
		m_iOffsetEyeAngle = this->findInSendTable("CDODPlayer", "m_angEyeAngles[0]");
		m_eGame = dod;
	}
	else if( strstr(strGameDir, "/nucleardawn") ) {
		m_iOffsetAngVel = this->findInSendTable("CNDPlayer", "m_angRotation");
		m_iOffsetEyeAngle = this->findInSendTable("CNDPlayer", "m_angEyeAngles[0]");
		m_eGame = nucleardawn;
	}
	else {
		m_eGame = other;
	}

	int msg = 0;
	char msgbuf[64];
	int size;
#if SOURCE_ENGINE < SE_CSGO
	while( g_pServer->GetUserMessageInfo(msg, msgbuf, sizeof(msgbuf), size) ) {
		// printf("%d %s\n", msg, msgbuf);
		if( strcmp("ShowMenu", msgbuf) == 0 ) {
			g_Engine.m_iMsgMenu = msg;
		}

	    else if( strcmp("SayText", msgbuf) == 0 )
	    	g_Engine.m_iMsgText = msg;
		else if( strcmp("SayText", msgbuf) == 0 )
			m_iMsgFade = msg;
		msg++;
	}
#else
	g_Engine.m_iMsgMenu = DIALOG_MENU;
	g_Engine.m_iMsgText = DIALOG_TEXT;
#endif
	printf("MsgText: %d\n", g_Engine.m_iMsgText);
	printf("MsgMenu: %d\n", g_Engine.m_iMsgMenu);
	printf("MsgMenu: %d\n", m_iMsgFade);

	m_vecDuration.clear();
	m_vecPlayersFlashed.clear();

	SH_ADD_HOOK(ConCommand, Dispatch, &g_CmdSbg, &CServerPlugin::ConCommand_Dispatch, false);

	m_bLoaded = true;
	g_PlayerManager.load(bLate);
	g_SBG.load(&g_PlayerManager, &g_Engine);
}

bool CServerPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
	PLUGIN_SAVEVARS();

	GET_V_IFACE_CURRENT(GetEngineFactory, g_pcvar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_pServer, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_ANY(GetServerFactory, g_pGameEnts, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	GET_V_IFACE_ANY(GetEngineFactory, g_pEngineTrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_ANY(GetServerFactory, g_pServerGameClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_ANY(GetEngineFactory, g_pServerPluginHelper, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS)
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameEvent, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
	GET_V_IFACE_ANY(GetServerFactory, g_pPlayerInfoMgr, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);
	// GET_V_IFACE_ANY(GetServerFactory, g_pGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT);
	GET_V_IFACE_ANY(GetEngineFactory, g_pEngineSound, IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION);
	engine = g_pEngine;

	if ( (g_pServerPluginCallback = ismm->GetVSPInfo(NULL)) == NULL ) {
		ismm->AddListener(this, this);
		ismm->EnableVSPListener();
	}
	else
		SH_ADD_HOOK(IServerPluginCallbacks, OnQueryCvarValueFinished, g_pServerPluginCallback, &CServerPlugin::onQueryCvarValueFinished, false);

	m_bLoadedAsMM = true;

	g_pGlobals = ismm->GetCGlobals();
	gpGlobals = g_pGlobals;
	g_pCvar = g_pcvar;
	g_pCVar = g_pcvar;
	ConVar_Register(0, this);
	loadHooks();

	/* late load */
	if(g_pGlobals->framecount > 1) {
		init(true);
	}
	return true;
}

bool CServerPlugin::Unload(char *error, size_t maxlen) {
	m_bLoaded = false;
	g_SBG.unload();
	g_PlayerManager.unload();
	unloadHooks();
	return true;
}

void CServerPlugin::AllPluginsLoaded() {
	/* This is where we'd do stuff that relies on the mod or other plugins
	 * being initialized (for example, cvars added and events registered).
	 */
}

bool CServerPlugin::Pause(char *error, size_t maxlen) {
	return true;
}

bool CServerPlugin::Unpause(char *error, size_t maxlen) {
	return true;
}

const char *CServerPlugin::GetLicense() {
	return "Public";
}

const char *CServerPlugin::GetVersion() {
	return VERSION;
}

const char *CServerPlugin::GetDate() {
	return __DATE__;
}

const char *CServerPlugin::GetLogTag() {
	return PLUGINTAG;
}

const char *CServerPlugin::GetAuthor() {
	return AUTHOR;
}

const char *CServerPlugin::GetDescription() {
	return NAME;
}

const char *CServerPlugin::GetName() {
	return NAME;
}

const char *CServerPlugin::GetURL() {
	return URL;
}


/********************************************************************************************************/
/* hooks */
void CServerPlugin::onFrame(bool simulating) {
	g_PlayerManager.onTick();
	g_SBG.onFrame();
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onClientPutInServer(edict_t *pEntity, char const *playername) {
	SH_CALL(g_pServerGameClients, &IServerGameClients::ClientPutInServer)(pEntity, playername);
	g_PlayerManager.onConnect(pEntity);
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pEntity);
	if( pPlayer != NULL ) {
		// g_ServerPlugin.printDataMap(pEntity->GetUnknown()->GetBaseEntity()->GetDataDescMap());
		g_SBG.onJoinGame(pPlayer);
	}
	RETURN_META(MRES_SUPERCEDE);
}

void CServerPlugin::onClientDisconnect(edict_t *pent) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pent);
	if( pPlayer != NULL ) {
		// g_ServerPlugin.printDataMap(pEntity->GetUnknown()->GetBaseEntity()->GetDataDescMap());
		g_SBG.onLeaveGame(pPlayer);
	}
	// SH_REMOVE_HOOK(CBaseEntity, SetTransmit, pent->GetNetworkable()->GetBaseEntity(), &CServerPlugin::onSetTransmit, false);
	g_PlayerManager.onDisconnect(pent);
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onClientCommand(edict_t *pEntity, const CCommand &args) {
	g_Engine.m_pCmd = (CCommand *) &args;
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pEntity);

	if( pPlayer == NULL )
		RETURN_META(MRES_IGNORED);

	g_PlayerManager.onCommand(pEntity);

	if( g_SBG.onClientCommand(pPlayer) ) {
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {
	g_ServerPlugin.init(false);
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onServerDeactivate() {
	g_SBG.onMapChange();
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onClientSettingsChanged(edict_t *pent) {
	RETURN_META(MRES_IGNORED);
}
/*
void CServerPlugin::onProcessMove(CBasePlayer *pBasePlayer, CMoveData *pMove) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pBasePlayer->edict());
	if( pPlayer != NULL )
		g_SBG.onMove(pPlayer);
	RETURN_META(MRES_IGNORED);
}
*/
void CServerPlugin::onTraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t *pTr) {
	edict_t *pBaseVictim = META_IFACEPTR(CBaseEntity)->edict();
	CTakeDamageInfoHack& infoHack = (CTakeDamageInfoHack&) info;
	// edict_t *pBaseAttacker = info.GetAttacker()->edict();
	// CVector vecStart(&pTr->startpos.x);
	CPlayer *pAttacker = (CPlayer *) g_PlayerManager.getPlayer(infoHack.getAttacker());
	CPlayer *pVictim = g_PlayerManager.getPlayer(pBaseVictim);
	// printf("%x %x\n", pAttacker, pVictim);
	if( pVictim != NULL && pAttacker != NULL ) {
		g_SBG.onTakeDamage(pVictim, pAttacker);
	}
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ) {
	CPlayer *pPlayer = g_PlayerManager.getPlayer(pPlayerEntity);
	if( eStatus != 1 /* CvarNotFound */ && pPlayer != NULL ) {
		g_PlayerManager.onCvarValue(pPlayer, pCvarValue, atof(pCvarValue));
		g_SBG.onCvarValue(pPlayer, pCvarName, pCvarValue, atof(pCvarValue));
	}
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onSetTransmit(CCheckTransmitInfo *pInfo, bool bAlways) {
	CBaseEntity *p = META_IFACEPTR(CBaseEntity);
    CPlayer *pPlayer = g_PlayerManager.getPlayer(pInfo->m_pClientEnt);
    // printf("test %x %d %s\n", p->edict(), IndexOfEdict(p->edict()), (char *) g_pEngine->GetClientConVarValue(p->entindex(), "name"));
    if( pPlayer != NULL && !g_SBG.onCheckTransmit(pPlayer, g_PlayerManager.getPlayer(p->edict())) )
    	RETURN_META(MRES_SUPERCEDE);
    RETURN_META(MRES_IGNORED);
}

void CServerPlugin::ConCommand_Dispatch(const CCommand &command) {
	g_Engine.m_pCmd = (CCommand *) &command;
	RETURN_META(MRES_IGNORED);
}

#ifdef SOURCE_ENGINE >= SE_CSGO
void CServerPlugin::onSendUserMessage( IRecipientFilter &filter, int message, const google::protobuf::Message &msg ) {
	/*g_ServerPlugin.m_pMsgWriter =*/ SH_CALL(g_pEngine, &IVEngineServer::SendUserMessage)(filter, message, msg);
	g_ServerPlugin.m_iLastMsg = message;
	
	// get all recipients
	char cNum = filter.GetRecipientCount();
	g_ServerPlugin.m_cNumRecipients = cNum;
	g_ServerPlugin.m_aRecipient = new IPlayer*[cNum];
	for(short i = 0; i < cNum; i++ ) {
		g_ServerPlugin.m_aRecipient[i] = g_PlayerManager.getPlayer(filter.GetRecipientIndex(i));
	}

	RETURN_META(MRES_SUPERCEDE);
}
#else

bf_write *CServerPlugin::onUserMessageBegin( IRecipientFilter *filter, int msg_type ) {
	g_ServerPlugin.m_pMsgWriter = SH_CALL(g_pEngine, &IVEngineServer::UserMessageBegin)(filter, msg_type);
	g_ServerPlugin.m_iLastMsg = msg_type;
	
	// get all recipients
	char cNum = filter->GetRecipientCount();
	g_ServerPlugin.m_cNumRecipients = cNum;
	g_ServerPlugin.m_aRecipient = new IPlayer*[cNum];
	for(short i = 0; i < cNum; i++ ) {
		g_ServerPlugin.m_aRecipient[i] = g_PlayerManager.getPlayer(filter->GetRecipientIndex(i));
	}

	RETURN_META_VALUE(MRES_SUPERCEDE, g_ServerPlugin.m_pMsgWriter);
}
#endif

void CServerPlugin::onMessageEnd( void ) {
	// process msgdata here
	// bf_read read(g_ServerPlugin.m_pMsgWriter->GetData(), g_ServerPlugin.m_pMsgWriter->m_nDataBytes);

	delete[] g_ServerPlugin.m_aRecipient;
	g_ServerPlugin.m_aRecipient = NULL;
	RETURN_META(MRES_IGNORED);
}

//void CServerPlugin::onEmitSound2( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample, 
//		float flVolume, float flAttenuation, int iFlags, int iPitch, 
//		const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity ) {
void CServerPlugin::onEmitSound2(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
                float flVolume, soundlevel_t iSoundlevel, int iFlags, int iPitch,  int iSpecialDSP,
                const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity )
{
	// send sound from players which are not transmitted
	CRecipientFilter &Filter = (CRecipientFilter&) filter;
	vector<IPlayer *> vecRecipients;
	for( int i = 0; i < filter.GetRecipientCount(); i++ ) {
		IPlayer *pPlayer = g_PlayerManager.getPlayer(filter.GetRecipientIndex(i));
		if( pPlayer != NULL ) {
			vecRecipients.push_back(pPlayer);
			Filter.RemoveRecipientByPlayerIndex(pPlayer->getIndex());
		}
	}

	CVector vecOrigin;
	if( pOrigin != NULL )
		vecOrigin.set(pOrigin->x, pOrigin->y, pOrigin->z);
	g_SBG.onEmitSound(vecRecipients, iEntIndex, iChannel, flVolume, (float) iSoundlevel, &vecOrigin);
	vector<IPlayer *>::iterator it = vecRecipients.begin();
	for( ; it < vecRecipients.end(); it++ ) {
		Filter.AddRecipient((CBasePlayer *) ((edict_t *) ((CPlayer *) *it)->getEdict())->GetUnknown()->GetBaseEntity());
	}
	vecRecipients.clear();
	RETURN_META(MRES_IGNORED);
}

void CServerPlugin::onEmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
                float flVolume, soundlevel_t iSoundlevel, int iFlags, int iPitch,  int iSpecialDSP,
                const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity )
{
	// send sound from players which are not transmitted
	CRecipientFilter &Filter = (CRecipientFilter&) filter;
	vector<IPlayer *> vecRecipients;
	for( int i = 0; i < filter.GetRecipientCount(); i++ ) {
		IPlayer *pPlayer = g_PlayerManager.getPlayer(filter.GetRecipientIndex(i));
		if( pPlayer != NULL ) {
			vecRecipients.push_back(pPlayer);
			Filter.RemoveRecipientByPlayerIndex(pPlayer->getIndex());
		}
	}

	CVector vecOrigin;
	if( pOrigin != NULL )
		vecOrigin.set(pOrigin->x, pOrigin->y, pOrigin->z);
	g_SBG.onEmitSound(vecRecipients, iEntIndex, iChannel, flVolume, 0.0, &vecOrigin);
	vector<IPlayer *>::iterator it = vecRecipients.begin();
	for( ; it < vecRecipients.end(); it++ ) {
		Filter.AddRecipient((CBasePlayer *) ((edict_t *) ((CPlayer *) *it)->getEdict())->GetUnknown()->GetBaseEntity());
	}
	vecRecipients.clear();
	RETURN_META(MRES_IGNORED);
}


