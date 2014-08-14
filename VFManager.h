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

#ifndef VFMANAGER_H_
#define VFMANAGER_H_

#include <stdio.h>

#define CreateFunc(i, x, y) \
	virtual int get##i##x##y() { return i+x+y; }

#define CreateFunc10(x, y) \
		CreateFunc(0, x, y) \
		CreateFunc(1, x, y) \
		CreateFunc(2, x, y) \
		CreateFunc(3, x, y) \
		CreateFunc(4, x, y) \
		CreateFunc(5, x, y) \
		CreateFunc(6, x, y) \
		CreateFunc(7, x, y) \
		CreateFunc(8, x, y) \
		CreateFunc(9, x, y)

#define CreateFunc100(x) \
	CreateFunc10(0, x) \
	CreateFunc10(10, x) \
	CreateFunc10(20, x) \
	CreateFunc10(30, x) \
	CreateFunc10(40, x) \
	CreateFunc10(50, x) \
	CreateFunc10(60, x) \
	CreateFunc10(70, x) \
	CreateFunc10(80, x) \
	CreateFunc10(90, x)

#define CreateFunc1000() \
	CreateFunc100(0) \
	CreateFunc100(100) \
	CreateFunc100(200) \
	CreateFunc100(300) \
	CreateFunc100(400) \
	CreateFunc100(500) \
	CreateFunc100(600) \
	CreateFunc100(700) \
	CreateFunc100(800) \
	CreateFunc100(900)


#define VF_CALL(Class, Instance, Function) \
	(((unsigned int **) Instance)[0][2])

template<class T, typename F>
class CVFManager {
private:

public:
	void call();
	static int getIndex(F function);
};

#endif /* VFMANAGER_H_ */
