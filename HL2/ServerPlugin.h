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

#ifndef SERVERPLUGIN_H_
#define SERVERPLUGIN_H_

#include "../IServerPlugin.h"
#include "hl2.h"
#include "Engine.h"
#include "datamap.h"
#include "TakeDamageInfoHack.h"
// #include <cbase.h>
// #include <igamemovement.h>
#include "../SBGuardian.h"
#include "../version.h"

class CVSPCallback : public IServerPluginCallbacks {
	/* For VSP loading */
    bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
    void Unload( void );
    void Pause( void );
    void UnPause( void );
    const char *GetPluginDescription( void );
    void LevelInit( char const *pMapName );
    void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
    void GameFrame( bool simulating );
    void LevelShutdown( void );
    void ClientActive( edict_t *pEntity );
    void ClientDisconnect( edict_t *pEntity );
    void ClientPutInServer( edict_t *pEntity, char const *playername );
    void SetCommandClient( int index );
    void ClientSettingsChanged( edict_t *pEdict );
    PLUGIN_RESULT ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
    PLUGIN_RESULT ClientCommand( edict_t *pEntity, const CCommand &args );
    PLUGIN_RESULT NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
    void OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
};

class CServerPlugin : public IServerPlugin, public ISmmPlugin, public IConCommandBaseAccessor, public IMetamodListener, public IGameEventListener2 {
friend class CPlayer;
friend class CEngine;
friend class CVSPCallback;
private:
	ISmmAPI *m_pISMM;
	int m_iOffsetFlashDuration;
	int m_iOffsetFlashMaxAlpha;
	bool m_bLoadedAsMM;
	int m_iOffsetAngVel;
	int m_iOffsetFOV;
	int m_iOffsetEyeAngle;
	int m_iOffsetMoney;
	eGame m_eGame;
	bool m_bLoaded;
	int m_iMsgFade;
	int m_iLastMsg;
	bf_write *m_pMsgWriter;
	IPlayer **m_aRecipient;
	char m_cNumRecipients;
	vector<IPlayer *> m_vecPlayersFlashed;
	vector<float> m_vecDuration;

public:
	CServerPlugin() : m_bLoadedAsMM(false), m_pISMM(NULL), m_eGame(other), m_bLoaded(false) { }
	virtual ~CServerPlugin() {
		Unload(NULL, 0);
	}

	void init(bool bLate);
	int findInSendTable(const char *ClassName, const char* PropName);
	unsigned int findInDataMap(datamap_t *pMap, const char *strName);
	void printSendTable() const;
	void printDataMap(datamap_t *pMap) const;
	void loadHooks();
	void loadHooksVSP();
	void unloadHooks();
	void unloadHooksVSP();
	void loadPlayerHooks(edict_t *pEdict);
	static void hookCmdDummy(const CCommand &command);

	virtual bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
    virtual bool Unload(char *error, size_t maxlen);
    virtual void AllPluginsLoaded();
    virtual bool Pause(char *error, size_t maxlen);
    virtual bool Unpause(char *error, size_t maxlen);
    virtual const char *GetLicense();
    virtual const char *GetVersion();
    virtual const char *GetDate();
    virtual const char *GetLogTag();
    virtual const char *GetAuthor();
    virtual const char *GetDescription();
    virtual const char *GetName();
    virtual const char *GetURL();

    virtual void OnVSPListening(IServerPluginCallbacks *iface);
    virtual void FireGameEvent(IGameEvent *event);
    virtual int GetEventDebugID() { return 0; }
    virtual bool RegisterConCommandBase(ConCommandBase *pCommandBase);

    bool isMM() const {
    	return m_bLoadedAsMM;
    }

    // hooks
    static void onFrame(bool simulating);
    static void onClientPutInServer(edict_t *pEntity, char const *playername);
    static void onClientDisconnect(edict_t *pent);
    static void onClientCommand(edict_t *pEntity, const CCommand &args);
    static void onServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
    static void onServerDeactivate();
    static void onClientSettingsChanged(edict_t *pent);
    static void onQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
    static void onSetTransmit(CCheckTransmitInfo *pInfo, bool bAlways);
    static void ConCommand_Dispatch(const CCommand &command);
    // static void onProcessMove(CBasePlayer *pBasePlayer, CMoveData *pMove);
    static void onTraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t *pTr);
	static bf_write *onUserMessageBegin( IRecipientFilter *filter, int msg_type );
	static void onMessageEnd( void );
	static void onEmitSound2( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
                float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM,  int iSpecialDSP = 0,
                const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1  );

	static void onEmitSound( IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
                float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM,  int iSpecialDSP = 0,
                const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1  );

};

extern IServerGameDLL *g_pServer;
extern IEngineTrace *g_pEngineTrace;
extern IVEngineServer *g_pEngine;
extern IVEngineServer *engine;
extern IServerGameEnts *g_pGameEnts;
extern IServerGameClients *g_pServerGameClients;
extern IGameEventManager2 *g_pGameEvent;
extern IServerPluginCallbacks *g_pServerPluginCallback;
extern IServerPluginHelpers *g_pServerPluginHelper;
extern ICvar *g_pcvar;
extern IPlayerInfoManager *g_pPlayerInfoMgr;
// extern IGameMovement *g_pGameMovement;
extern IEngineSound *g_pEngineSound;

extern CGlobalVars *g_pGlobals;
extern CGlobalVars *gpGlobals;
extern CServerPlugin g_ServerPlugin;

inline int IndexOfEdict(edict_t *pEdict) {
#if SOURCE_ENGINE >= SE_LEFT4DEAD
	return (int) (pEdict - g_pGlobals->pEdicts);
#else
	return g_pEngine->IndexOfEdict(pEdict);
#endif
}

inline edict_t *PEdictOfIndex(int iIndex) {
#if SOURCE_ENGINE >= SE_LEFT4DEAD
	if( iIndex >= 0 && iIndex <= g_pGlobals->maxEntities )
		return (edict_t *) (g_pGlobals->pEdicts + iIndex);
	return NULL;
#else
	return g_pEngine->PEntityOfEntIndex(iIndex);
#endif
}

#endif /* SERVERPLUGIN_H_ */
