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

#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <stdint.h>

inline uint64_t _rdtsc() {
	uint64_t tmp;
	asm volatile ("rdtsc" : "=a"(((uint32_t*) &tmp)[0]) "=d"(((uint32_t*) &tmp))[1]);
	return tmp;
}

#endif
