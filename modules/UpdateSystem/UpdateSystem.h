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

#ifndef UPDATESYSTEM_H_
#define UPDATESYSTEM_H_

#include "../../IModule.h"
#include "../../Socket.h"
#include "../../Thread.h"
#include "../../File.h"

#define _HOST "http://storage.steambans.com"
#define URL_REV "http://storage.steambans.com/sbguardian2/rev.txt"
#define _DL_FOLDER "sbguardian2"
#define UPDATE_INTERVALL 60*5
// ---------------------------------------------------------------
#ifdef WIN32
#define _EXT ".dll"
#define _FS '\\'
#else
#define _EXT ".so"
#define _FS '/'
#endif
// ---------------------------------------------------------------
#if SOURCE_ENGINE == 8
#define _DL_FILE "dl_source_8"
#elif SOURCE_ENGINE == 6
#define _DL_FILE "dl_source_6"
#elif SOURCE_ENGINE == 4
#define _DL_FILE "dl_source_4"
#elif SOURCE_ENGINE == 0
#define _DL_FILE "dl_goldsrc"
#endif
#define DL_FILE _HOST "/" _DL_FOLDER "/" _DL_FILE _EXT
#define S_TMPNAME "20f029fh902fh0"
#define S_FILENAME S_FILE _EXT
// ---------------------------------------------------------------
class CUpdateSystem : public IModule {
private:
	float m_flLastCheck;
	int m_iHeadRev;
	long unsigned int m_iThreadId;
	bool m_bReload;
	char *m_strFile;
	char *m_strPath;

public:
	CUpdateSystem() : m_flLastCheck(0.0), m_iHeadRev(-1), m_iThreadId(0), m_bReload(false), m_strFile(NULL),
	m_strPath(NULL) {

	}
	virtual ~CUpdateSystem() {
		unload();
	}

	void onFrame();
	void onMapChange();
	virtual void init(IEngine *pEngine, IPlayerManager *pPlayerManager);
	virtual void unload();
	virtual bool onCommand();
	static void *dlRev(void *argv);

	int getHeadRev() const {
		return m_iHeadRev;
	}

	virtual const char *getName() const {
		return "UpdateSystem";
	}
};

#endif /* UPDATESYSTEM_H_ */
