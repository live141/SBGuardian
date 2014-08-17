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

inline uint64_t _rdtsc() {
	uint64_t tmp;
	asm volatile ("rdtsc" : "=a"(((uint32_t*) &tmp)[0]), "=d"(((uint32_t*) &tmp)[1]) : :);
	return tmp;
}

class Profile {
private:
	uint32_t* _histogram;
	uint32_t _tsc;
	uint16_t _num;
	uint16_t _idx;
public:
	Profile() : _histogram(NULL), _tsc(0), _num(0), _idx(0) {}
	Profile(uint16_t num) : _histogram(NULL), _tsc(0), _num(num), _idx(0) {
		_histogram = (uint32_t*) malloc(sizeof(uint32_t)*num);
	}
	~Profile() {
		if( _histogram != NULL )
			free(_histogram);
	}
	void start() {
		_tsc = _rdtsc();
	}
	void update() {
		uint32_t tmp = _rdtsc();
		_histogram[++_idx%_num] = tmp - _tsc;
		_tsc = tmp;
	}
	uint32_t min() {
		uint32_t min = 0xffffffff;
		uint16_t i;
		for(i = 0; i < _num; ++i) {
			if( min > _histogram[i] )
				min = _histogram[i];
		}
		return min;
	}
	uint32_t max() {
		uint32_t max = 0;
		uint16_t i;
		for(i = 0; i < _num; ++i) {
			if( max < _histogram[i] )
				max = _histogram[i];
		}
		return max;
	}
	uint32_t mean() {
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
