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

#ifndef CVARCHECK_H_
#define CVARCHECK_H_

#include "../../IModule.h"

#define CHECK_INTERVALL 10.0 // 0.03
#define PRINT_INTERVALL 10.0

enum eCvarType {
	type_unknown = 0,
	equal, // if client's cvar is
	notequal, // if client's cvar is not
	bigger,  // if client's cvar is bigger
	smaller,
	biggerequal,
	smallerequal
};

enum eCvarAction {
	action_unknown = 0,
	print,
	kick,
	ban
};

class CCvar {
public:
	CStr m_sName;
	eCvarType m_type;
	float m_flValue;
	eCvarAction m_action;

public:
	CCvar(const char *strName, eCvarType type, float flValue, eCvarAction action) :
		m_type(type), m_flValue(flValue), m_action(action) {
		m_sName = strName;
	}

	~CCvar() {
		m_sName.clear();
	}

	static eCvarAction stringToAction(const char *strAction);
	static eCvarType stringToType(const char *strType);
	static const char *actionToString(eCvarAction action);
	static const char *typeToString(eCvarType type);

	bool operator ==(const CCvar& cvar) const {
		return ( !strcmp(m_sName.data(), cvar.m_sName.data()) );
	}

	CCvar& operator =(const CCvar& cvar) {
		m_sName = cvar.m_sName;
		m_type = cvar.m_type;
		m_flValue = cvar.m_flValue;
		m_action = cvar.m_action;
		return *this;
	}

	bool operator <(const CCvar& cvar) const {
		return ( strcmp(m_sName.data(), cvar.m_sName.data()) < 0 );
	}
};

class CCvarCheck : public IModule {
private:
	vector<CCvar> m_vecCvars;
	int m_iIndex;
	vector<CCvar>::iterator m_it;
	float *m_flLastCheck;
	vector<CStr> m_vecPrints;
	float m_flLastPrint;
	bool m_bInCheck;

public:
	CCvarCheck() : m_iIndex(1), m_flLastCheck(NULL), m_flLastPrint(0.0), m_bInCheck(false) {
		m_it = m_vecCvars.end();
	}
	virtual ~CCvarCheck() {
		unload();
	}

	void add(const char *strName, float flValue, eCvarType type, eCvarAction action);
	void onFrame();
	void remove(const char *strName);
	void onCvarValue(IPlayer *pPlayer, const char *strName, const char *strValue, float flValue);

	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();

	virtual const char *getName() const {
			return "CVarCheck";
	}
};

#endif /* CVARCHECK_H_ */
