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

#ifndef CSTEAMID_H_
#define CSTEAMID_H_

#ifdef WIN32
#define snprintf _snprintf
#endif

class CSteamId {
private:
	unsigned int m_iId;
	static char bitUniverse;

public:
	CSteamId() : m_iId(0) { }
	CSteamId(unsigned int iId) : m_iId(iId) { }
	CSteamId(const char *strId);

	const char *getStatic() const;

	unsigned int getId() const {
		return m_iId;
	}

	bool operator ==(CSteamId SteamId) const {
		return ( m_iId == SteamId.m_iId );
	}

	CSteamId operator =(CSteamId SteamId) {
		m_iId = SteamId.m_iId;
		return *this;
	}
};

#endif /* CSTEAMID_H_ */
