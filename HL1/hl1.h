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

#ifndef HL1_H_
#define HL1_H_

#include <extdll.h>
#if defined WIN32 && defined STANDALONE
#include <util.h>
#endif
#if ( !defined STANDALONE || !defined WIN32 ) && !defined _SOURCEHOOK_
#include <meta_api.h>
#elif !defined STANDALONE
// include this to avoid definition from both metamods
// #include "comp_dep.h"
#include "dllapi.h"
#include "engine_api.h"
#include "plinfo.h"
#include "osdep.h"
#include "mutil.h"
#endif
#include <cbase.h>
#include <h_export.h>
#include "pm_shared/pm_defs.h"

#ifdef WIN32
#define DLLEXP __declspec(dllexport)
#define WINAPI2 WINAPI
#else
#define WINAPI2
#define DLLEXP
#endif

inline CBaseEntity *BaseEntityOfEdict(edict_t *pEdict) {
	if( pEdict == NULL )
		return NULL;
	return (CBaseEntity *) pEdict->pvPrivateData;
}

extern globalvars_t *g_Globals;
extern enginefuncs_t g_EngFuncs;
extern DLL_FUNCTIONS g_FunctionTable;
extern NEW_DLL_FUNCTIONS g_NewDllFuncs;

#endif /* HL1_H_ */
