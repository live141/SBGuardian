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

#define _SOURCEHOOK_

#include "SrcHooks.h"
#include "PlayerManager.h"
#include "../SBGuardian.h"

/***********************************************************************************/
#include <sourcehook.h>
#include <sourcehook_impl.h>
static SourceHook::Impl::CSourceHookImpl s_SHImpl;
static SourceHook::ISourceHook *g_SHPtr = &s_SHImpl;
static int g_PLID = 0;
/***********************************************************************************/
SH_DECL_HOOK5_void(CBaseEntity, TraceAttack, SH_NOATTRIB, 0, entvars_t *, float, Vector, TraceResult *, int);
/***********************************************************************************/

void CSrcHooks::loadHooks(edict_t *pEnt) {
	CBaseEntity *pBase = BaseEntityOfEdict(pEnt);
	SH_ADD_HOOK(CBaseEntity, TraceAttack, pBase, &CSrcHooks::onTraceAttack, false);
}

void CSrcHooks::unloadHooks(edict_t *pEnt) {
	CBaseEntity *pBase = BaseEntityOfEdict(pEnt);
	SH_ADD_HOOK(CBaseEntity, TraceAttack, pBase, &CSrcHooks::onTraceAttack, false);
}

void CSrcHooks::onTraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {
	edict_t *edictAttacker = pevAttacker->pContainingEntity;
	edict_t *edictVictim = META_IFACEPTR(CBaseEntity)->edict();
	CPlayer *pAttacker = (CPlayer *) g_PlayerManager.getPlayer(edictAttacker);
	CPlayer *pVictim = (CPlayer *) g_PlayerManager.getPlayer(edictVictim);
	if( pAttacker != NULL && pVictim != NULL )
		g_SBG.onTakeDamage(pVictim, pAttacker);
	RETURN_META(MRES_IGNORED);
}
