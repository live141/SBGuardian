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

#include "DLL.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void CDLL::open(const char *strName) {
	if( m_Handle != NULL )
		return;

#ifdef WIN32
	m_Handle = LoadLibraryA(strName);
#else
	m_Handle = dlopen(strName, RTLD_NOW);
#endif
}

void *CDLL::sym(const char *strName) const {
	if( m_Handle == NULL )
		return NULL;

#ifdef WIN32
	return (void *) GetProcAddress((HMODULE) m_Handle, strName);
#else
	return dlsym(m_Handle, strName);
#endif
}

void CDLL::close() {
	if( m_Handle == NULL )
		return;

#ifdef WIN32
	FreeLibrary((HMODULE) m_Handle);
#else
	dlclose(m_Handle);
#endif

	m_Handle = NULL;
}

