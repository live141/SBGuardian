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

#ifndef DLL_H_
#define DLL_H_

#include <stdlib.h>

class CDLL {
private:
	void *m_Handle;

public:
	CDLL() : m_Handle(NULL) { }
	CDLL(const char *strName) : m_Handle(NULL) {
		open(strName);
	}

	void open(const char *strName);
	void *sym(const char *strName) const;
	void close();

	bool isOpen() const {
		return ( m_Handle != NULL );
	}
};

#endif /* DLL_H_ */
