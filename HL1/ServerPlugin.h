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

#ifndef CSERVERPLUGIN_H_
#define CSERVERPLUGIN_H_

#include "../IServerPlugin.h"
#include "hl1.h"
#include "Engine.h"
#include "Message.h"

class CServerPlugin : public IServerPlugin {
	friend class CPlayer;
	friend class CEngine;
private:
	bool m_bLoaded;

	eGame m_eGame;

	// flash msg
	static int m_iArg;
	static bool m_bMsgFlash;
	static float m_flHoldTime;
	static float m_flFullFlashTime;
	static edict_t *m_pReceiver;

public:
	CMessage m_Msg;
	bool m_bRestartRound;

	CServerPlugin() : m_bLoaded(false), m_eGame(other), m_bRestartRound(false) { }
	~CServerPlugin() {
		unload();
	}

	void init(bool late);
	void unload();

	bool isLoaded() const {
		return m_bLoaded;
	}

	static void hookCmdDummy();
	static void GameFrame();
	static int ClientConnect(edict_s *pent, char const *s1, char const *s2, char *s3);
	static void ClientPutInServer(edict_t *pent);
	static void ClientDisconnect(edict_t *pent);
	static void ClientCommand(edict_t *pent);
	static void LevelInit();
	static void ClientSettingsChanged(edict_t *pent, char *infobuffer);
	static void OnQueryCvarValueFinished(const edict_t *pEnt, int requestID, const char *cvarName, const char *value);
	static int CheckTransmit(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
	static void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	static void ServerDeactivate();
	static int RegUserMsg(const char *msg, int len);
	static void TraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr);
	static void onPM_Move(struct playermove_s *ppmove, int server);
	static void MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
	static void MessageEnd();
	static void WriteShort(int iValue);
	static void WriteByte(int iValue);
	static void WriteString(const char *sz);
	static void WriteLong(int iValue);
	static void WriteAngle(float flValue);
	static void WriteEntity(int iValue);
	static void WriteCoord(float flValue);
	static void WriteChar(int iValue);

};

extern CServerPlugin g_ServerPlugin;

#endif /* CSERVERPLUGIN_H_ */
