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

#ifndef HL2_H_
#define HL2_H_

// #include "CSmmAPI.h"
#define NULL 0
#define GAME_DLL 1
#include "isaverestore.h"
#include "ehandle.h"
#include "shareddefs.h"
#include "util.h"
#include "ISmmPlugin.h"
#include "cbase.h"
#include "engine/IEngineSound.h"
// #include "gameinterface.h"
#include "IEngineTrace.h"
#include "iplayerinfo.h"
// #include "sh_vector.h"
#include "igameevents.h"

#define MAX_CLIENTMSG_LEN 128

#if defined WIN32 && !defined snprintf
#define snprintf _snprintf
#endif

// #define ENTINDEX(a) g_pEngine->IndexOfEdict(a)

PLUGIN_GLOBALVARS();

#endif /* HL2_H_ */
