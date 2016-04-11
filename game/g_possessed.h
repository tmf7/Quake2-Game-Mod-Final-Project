/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// g_local.h -- local definitions for game module
//#include "q_shared.h"


//*************
//   SOLDIER
//*************
void soldier_stand1		( edict_t *self );
void soldier_stand3		( edict_t *self );
void soldier_walk1		( edict_t *self );
void soldier_walk2		( edict_t *self );
void soldier_start_run	( edict_t *self );	// primary
void soldier_runp		( edict_t *self );	// secondary

// self->pain = soldier_pain; ( each type makes a different pain noise )
// Let T_Damage take care of these frames
// void soldier_pain1		( edict_t *self );
// void soldier_pain2		( edict_t *self );
// void soldier_pain3		( edict_t *self );
// void soldier_pain4		( edict_t *self );

void soldier_attack1	( edict_t *self ); // s & sl
void soldier_attack2	( edict_t *self ); // s & sl
void soldier_attack3	( edict_t *self ); // all, duck-attack
void soldier_attack4	( edict_t *self ); // ss
void soldier_attack6	( edict_t *self ); // all on-sight

// skill level dependent
void soldier_duck		( edict_t *self );

// self->die = soldier_die; ( each type makes a different death noise )
// Let T_Damage take care of these frames
// void soldier_death1		( edict_t *self );
// void soldier_death2		( edict_t *self );
// void soldier_death3		( edict_t *self );
// void soldier_death4		( edict_t *self );
// void soldier_death5		( edict_t *self );
// void soldier_death6		( edict_t *self );


//*************
//   INFANTRY
//*************