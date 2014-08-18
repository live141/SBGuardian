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

#ifndef TAKEDAMAGEINFOHACK_H_
#define TAKEDAMAGEINFOHACK_H_

#include "hl2.h"
// #include "ehandle.h"
#include "takedamageinfo.h"

class CTakeDamageInfoHack : public CTakeDamageInfo {
public:
	int getAttacker() {
		if( m_hAttacker.IsValid() )
			return m_hAttacker.GetEntryIndex();
		return -1;
	}
};

#endif /* TAKEDAMAGEINFOHACK_H_ */
