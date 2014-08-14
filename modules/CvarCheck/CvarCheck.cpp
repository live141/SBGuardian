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

#include "CvarCheck.h"
#include <stdio.h>
#include "../../File.h"

eCvarAction CCvar::stringToAction(const char *strAction) {
	if( !strcmp(strAction, "print") )
		return print;
	if( !strcmp(strAction, "kick") )
		return kick;
	if( !strcmp(strAction, "ban") )
		return ban;
	return action_unknown;
}

eCvarType CCvar::stringToType(const char *strType) {
	if( !strcmp(strType, "equal") )
		return equal;
	if( !strcmp(strType, "notequal") )
		return notequal;
	if( !strcmp(strType, "smaller") )
		return smaller;
	if( !strcmp(strType, "smallerequal") )
		return smallerequal;
	if( !strcmp(strType, "bigger") )
		return bigger;
	if( !strcmp(strType, "biggerequal") )
		return biggerequal;
	return type_unknown;
}

const char *CCvar::actionToString(eCvarAction action) {
	switch( action ) {
	case print:
		return "print";
		break;
	case kick:
		return "kick";
		break;
	case ban:
		return "ban";
		break;
	default:
		return NULL;
	}

	return NULL;
}

const char *CCvar::typeToString(eCvarType type) {
	switch( type ) {
	case equal:
		return "equal";
		break;
	case notequal:
		return "noequal";
		break;
	case smaller:
		return "smaller";
		break;
	case smallerequal:
		return "smallerequal";
		break;
	case bigger:
		return "bigger";
		break;
	case biggerequal:
		return "biggerequal";
		break;
	default:
		return NULL;
	}

	return NULL;
}

void CCvarCheck::add(const char *strName, float flValue, eCvarType type, eCvarAction action) {
	if( type == type_unknown || action == action_unknown ) {
		printf("[SBG]: Format is: sbguardian cvar add name value type action\n");
		return;
	}

	// check if already exists
	vector<CCvar>::iterator it = m_vecCvars.find(CCvar(strName, equal, 0, print));
	if( it != m_vecCvars.end() ) {
		printf("[SBG]: CvarCheck: An Entry for %s already exists\n", strName);
		return;
	}
	m_vecCvars.push_back(CCvar(strName, type, flValue, action));
	// std::sort(m_vecCvars.begin(), m_vecCvars.end());
	printf("[SBG]: Successfully added.\n");
	m_it = m_vecCvars.begin();
}

void CCvarCheck::onFrame() {
	/*
	// check if we need to print
	if( !m_vecPrints.empty() ) {
		if( m_pEngine->time() > m_flLastPrint + PRINT_INTERVALL ) {
			m_pEngine->printHud(m_vecPrints.back().data());
			m_vecPrints.pop_back();
			m_flLastPrint = m_pEngine->time();
		}
	}
*/
	// check cvars
	// pro frame 1 cvar
	if( m_iIndex > m_pPlayerManager->getMaxClients() )
		m_iIndex = 1;

	IPlayer *pPlayer = m_pPlayerManager->getPlayer(m_iIndex);
	if( pPlayer == NULL || !pPlayer->isInGame() || m_it == m_vecCvars.end() || pPlayer->isBot() ) {
		m_bInCheck = false;
		m_iIndex++;
		m_it = m_vecCvars.begin(); // can leave while all cvars arent done
		return;
	}

	if( m_pEngine->time() < m_flLastCheck[m_iIndex] + CHECK_INTERVALL && !m_bInCheck ) {
		m_iIndex++;
		return;
	}
	m_bInCheck = true;
	m_flLastCheck[m_iIndex] = m_pEngine->time();

	pPlayer->queryCvar(m_it->m_sName.data());
	m_it++;
}

void CCvarCheck::remove(const char *strName) {
	// vector<CCvar>::iterator it = find(m_vecCvars.begin(), m_vecCvars.end(), CCvar(strName, equal, 0, print));
	vector<CCvar>::iterator it = m_vecCvars.find(CCvar(strName, equal, 0, print));
	if( it == m_vecCvars.end() )
		return;

	m_vecCvars.erase(it);
	m_it = m_vecCvars.begin();
	printf("[SBG]: Successfully removed.\n");
}

void CCvarCheck::onCvarValue(IPlayer *pPlayer, const char *strName, const char *strValue, float flValue) {
	if( !isEnabled() )
		return;

	// vector<CCvar>::iterator it = find(m_vecCvars.begin(), m_vecCvars.end(), CCvar(strName, equal, 0, print));
	vector<CCvar>::iterator it = m_vecCvars.find(CCvar(strName, equal, 0, print));
	if( it == m_vecCvars.end() )
		return;

	const char *strText = NULL;

	switch( it->m_type ) {
	case equal:
		if( it->m_flValue == flValue )
			strText = CStr::format("%f is not allowed\n", flValue);
		break;
	case notequal:
		if( it->m_flValue != flValue )
			strText = CStr::format("%s must be %f\n", strName, it->m_flValue);
		break;
	case bigger:
		if( flValue > it->m_flValue )
			strText = CStr::format("%s must be smaller/equal than/to %f\n", strName, it->m_flValue);
		break;
	case smaller:
		if( flValue < it->m_flValue )
			strText = CStr::format("%s must be bigger/equal than/to %f\n", strName, it->m_flValue);
		break;
	case biggerequal:
		if( flValue >= it->m_flValue )
			strText = CStr::format("%s must be smaller than %f\n", strName, it->m_flValue);
		break;
	case smallerequal:
		if( flValue <= it->m_flValue )
			strText = CStr::format("%s must be bigger than %f\n", strName, it->m_flValue);
		break;
	}

	CBuf buf;

	if( strText != NULL ) {
		switch( it->m_action ) {
		case print:
			buf.thisFormat("[SBG]: %s: %s and not %f", pPlayer->getNick(), strText, flValue);
			// m_vecPrints.push_back(std::string(buf.data()));
			m_pEngine->printHud(buf.data());
			break;
		case kick:
			pPlayer->kick(strText);
			buf.thisFormat("Kicking %s (%s) for %s %f", pPlayer->getNick(), pPlayer->getSteamId().getStatic(),
				it->m_sName.data(), flValue);
			CFileMgr::log(this->getName(), buf.data());
			break;
		case ban:
			buf.thisFormat("Banning %s (%s) for %s %f", pPlayer->getNick(), pPlayer->getSteamId().getStatic(),
				it->m_sName.data(), flValue);
			CFileMgr::log(this->getName(), buf.data());
			pPlayer->ban(strText);
			break;
		}
	}
}

void CCvarCheck::init(IEngine *pEngine, IPlayerManager *pPlayerManager) {
	m_pEngine = pEngine;
	m_pPlayerManager = pPlayerManager;
	m_iIndex = 1;
	m_flLastCheck = new float[pPlayerManager->getMaxClients()+1];
	m_flLastPrint = 0.0;
	m_bInCheck = false;
	m_it = m_vecCvars.end();

	setEnabled(true);
}

void CCvarCheck::unload() {
	setEnabled(false);
	if( m_flLastCheck != NULL )
		delete[] m_flLastCheck;
	m_flLastCheck = NULL;

	m_vecCvars.clear();
	m_vecPrints.clear();
	m_bInCheck = false;
}

bool CCvarCheck::onCommand() {
	const char *strArg0 = m_pEngine->argv(1);
	if( strcmp(strArg0, "cvar") )
		return false;
	int iArgc = m_pEngine->argc();

	if( iArgc > 2 ) {
		// example: sbguardian cvar add ex_interp 0.01 notequal kick
		const char *strCmd = m_pEngine->argv(2);
		if( !strcmp(strCmd, "add") ) {
			if( iArgc < 7 )
				return true;

			const char *strName = m_pEngine->argv(3);
			float flValue = atof(m_pEngine->argv(4));
			eCvarType type = (eCvarType) CCvar::stringToType(m_pEngine->argv(5));
			eCvarAction action = (eCvarAction) CCvar::stringToAction(m_pEngine->argv(6));
			add(strName, flValue, type, action);
		}
		else if( !strcmp(strCmd, "remove") ) {
			if( iArgc < 4 )
				return true;

			const char *strName = m_pEngine->argv(3);
			remove(strName);
		}
		else if( !strcmp(strCmd, "list") ) {
			vector<CCvar>::iterator it = m_vecCvars.begin();
			while( it != m_vecCvars.end() ) {
				printf("%s %f %s %s\n", it->m_sName.data(), it->m_flValue,
						CCvar::typeToString(it->m_type), CCvar::actionToString(it->m_action));
				it++;
			}
			printf("%u Cvars\n", m_vecCvars.size());
		}
		else {
				setEnabled(atoi(m_pEngine->argv(2)));
		}

		return true;
	}

	if( iArgc == 2 ) {
		const char *strText = CStr::format("CvarCheckk is %s\n", (isEnabled())?"enabled":"disabled");
		printf(strText);
	}

	return true;
}

