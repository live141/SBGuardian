/**
* SBGuardian v2.0
* Copyright (c) 2011 Fabian Fischer
* Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
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

#ifndef LINKENT_H_
#define LINKENT_H_

#include "hl1.h"

// Comments from SDK dlls/util.h:
//! This is the glue that hooks .MAP entity class names to our CPP classes.
//! The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress().
//! The function is used to intialize / allocate the object for the entity.

#define STRINGIZE(name, len)		#name+len

typedef void (*ENTITY_FN) (entvars_t *);

// Function to perform common code of LINK_ENTITY_TO_GAME.
void do_link_ent(ENTITY_FN *pfnEntity, int *missing, char *entStr,
		entvars_t *pev);

#define LINK_ENTITY_TO_GAME(entityName) \
	extern "C" DLLEXP void entityName(entvars_t *pev); \
	void entityName(entvars_t *pev) { \
		static ENTITY_FN pfnEntity = NULL; \
		static int missing=0; \
		char *entStr; \
		entStr = STRINGIZE(entityName, 0); \
		do_link_ent(&pfnEntity, &missing, entStr, pev); \
	}

#endif /* LINKENT_H_ */
