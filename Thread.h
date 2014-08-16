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

#ifndef THREAD_H_
#define THREAD_H_

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef void* (*funcThread)(void *);

#ifdef WIN32
typedef DWORD (*winThread)(void *);
#endif

class CThreads {
private:

public:
	static bool newThread(unsigned long int *iId, funcThread pFunc, void *argv);
	static void waitForThread(unsigned long int iId);
};

#endif /* THREAD_H_ */
