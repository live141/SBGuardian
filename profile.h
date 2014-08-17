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
#include <stdlib.h>
#include <stdio.h>

#ifdef _PROFILE_
#define PROFILE_SETUP(name, len) static Profile name(len)
#define PROFILE_UPDATE(name) do { name.update(); } while(0)
#define PROFILE_MIN(name) name.min()
#define PROFILE_MAX(name) name.max()
#define PROFILE_MEAN(name) name.mean()
#define PROFILE_PRINT(name) do { printf("Profile %s - min: %llu  max: %llu  mean: %llu\n", #name, name.min(), name.max(), name.mean()); } while(0)
#else
#define PROFILE_SETUP(name, len)
#define PROFILE_UPDATE(name)
#define PROFILE_PRINT(name)
#define PROFILE_MIN(name)
#define PROFILE_MAX(name)
#define PROFILE_MEAN(name)
#endif

inline uint64_t _rdtsc() {
	uint64_t tmp;
	asm volatile ("rdtsc" : "=a"(((uint32_t*) &tmp)[0]), "=d"(((uint32_t*) &tmp)[1]) : :);
	return tmp;
}

class Profile {
private:
	uint64_t* _histogram;
	uint64_t _tsc;
	uint16_t _num;
	uint16_t _idx;
public:
	Profile(void) : _histogram(NULL), _tsc(0), _num(0), _idx(0) {}
	Profile(uint16_t num) : _histogram(NULL), _tsc(0), _num(num), _idx(0) {
		_histogram = (uint64_t*) calloc(num, sizeof(uint64_t));
	}
	~Profile() {
		if( _histogram != NULL )
			free(_histogram);
	}
	void start(void) {
		_tsc = _rdtsc();
	}
	void update(void) {
		uint64_t tmp = _rdtsc();
		_histogram[_idx] = tmp - _tsc;
		_idx = (_idx+1)%_num;
		_tsc = tmp;
	}
	uint64_t min(void) {
		uint64_t min = 0xffffffffffffffff;
		uint16_t i;
		for(i = 0; i < _num; ++i) {
			if( min > _histogram[i] )
				min = _histogram[i];
		}
		return min;
	}
	uint64_t max(void) {
		uint64_t max = 0;
		uint16_t i;
		for(i = 0; i < _num; ++i) {
			if( max < _histogram[i] )
				max = _histogram[i];
		}
		return max;
	}
	uint64_t mean(void) {
		uint64_t mean = 0;
		uint16_t i;
		for(i = 0; i < _num; ++i) {
			mean += _histogram[i];
		}
		mean /= _num;
		return mean;
	}
};

#endif
