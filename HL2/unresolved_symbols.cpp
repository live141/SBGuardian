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

#include "hl2.h"
#include <cbase.h>

// CBaseEntityList *g_pEntityList = NULL;

// void CServerGameTags::GetTaggedConVarList( KeyValues *pCvarTagList )
// {
	/*
	if ( pCvarTagList )
	{
		g_pGameRules->GetTaggedConVarList( pCvarTagList );
	}
	*/
// }

void CRecipientFilter::AddAllPlayers( void )
{
	m_Recipients.RemoveAll();

	int i;
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i );
		if ( !pPlayer )
		{
			continue;
		}

		AddRecipient( pPlayer );
	}
}

CRecipientFilter::CRecipientFilter()
{
	Reset();
}

bool CRecipientFilter::IsReliable( void ) const
{
	return m_bReliable;
}

bool CRecipientFilter::IsInitMessage( void ) const
{
	return m_bInitMessage;
}

int CRecipientFilter::GetRecipientCount( void ) const
{
#if SOURCE_ENGINE >= SE_CSGO
	return m_Recipients.Count();
#else
	return m_Recipients.Size();
#endif
}

int	CRecipientFilter::GetRecipientIndex( int slot ) const
{
	if ( slot < 0 || slot >= GetRecipientCount() )
		return -1;

	return m_Recipients[ slot ];
}

void CRecipientFilter::Reset( void )
{
	m_bReliable			= false;
	m_bInitMessage		= false;
	m_Recipients.RemoveAll();
	m_bUsingPredictionRules = false;
	m_bIgnorePredictionCull = false;
}

void CRecipientFilter::AddRecipient( CBasePlayer *player )
{
	Assert( player );

	int index = player->entindex();

	// If we're predicting and this is not the first time we've predicted this sound
	//  then don't send it to the local player again.
	if ( m_bUsingPredictionRules )
	{
		// Only add local player if this is the first time doing prediction
		/*
		if ( g_RecipientFilterPredictionSystem.GetSuppressHost() == player )
		{
			return;
		}
		*/
	}

	// Already in list
	if ( m_Recipients.Find( index ) != m_Recipients.InvalidIndex() )
		return;

	m_Recipients.AddToTail( index );
}

CRecipientFilter::~CRecipientFilter()
{
}

CBasePlayer	*UTIL_PlayerByIndex( int playerIndex )
{
	CBasePlayer *pPlayer = NULL;

	if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients )
	{
		edict_t *pPlayerEdict = INDEXENT( playerIndex );
		if ( pPlayerEdict && !pPlayerEdict->IsFree() )
		{
			pPlayer = (CBasePlayer*)GetContainingEntity( pPlayerEdict );
		}
	}

	return pPlayer;
}

void CRecipientFilter::RemoveRecipientByPlayerIndex( int playerindex )
{
	Assert( playerindex >= 1 && playerindex <= ABSOLUTE_PLAYER_LIMIT );

	m_Recipients.FindAndRemove( playerindex );
}

int CBaseEntity::GetTeamNumber() const
{
	return m_iTeamNum;
}
