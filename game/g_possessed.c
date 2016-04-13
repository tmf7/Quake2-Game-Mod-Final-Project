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

//**********************
// TMF7 BEGIN GHOST MODE
//**********************

#include "g_local.h"
#include "g_possessed.h"

// How player frames are handled ( extra in T_Damage, ChangeWeapon, WeaponGeneric ) would be more precise
// NOTE: "#include" a xyz.c by forward declaring the needed function in <g_local.h> then including <g_local.h> in whatever
// EG: everything that currently includes <g_local.h> has access to ClientEndServerFrame ( ent );

char *take_host_noise;
char *drop_host_noise;

//*************
//   SOLDIER
//*************

// give all three soldiers the same physical moves
// resolve the thinkfunc's differently ( ie shotgun, blaster, etc )
hmove_t soldier[] =
{
	{	"stand1",				soldier_stand1		},
	{	"stand2",				soldier_stand3		},
	{	"walk1",				soldier_walk1		},
	{	"walk2",				soldier_walk2		},
	{	"run_start",			soldier_start_run	},
	{	"run",					soldier_runp		},
	{	"attack1",				soldier_attack1		},
	{	"attack2",				soldier_attack2		},
	{	"attack3",				soldier_attack3		},	//duck attack
	{	"attack4",				soldier_attack4		},
	{	"attack5",				soldier_attack6		},
	{	"duck",					soldier_duck		},
	{	NULL,					NULL				}
};

//*************
//   INFANTRY
//*************

hmove_t infantry[] =
{
//	{	"stand1",				soldier_stand1		},
//	{	"stand2",				soldier_stand3		},
//	{	"walk1",				soldier_walk1		},
//	{	"walk2",				soldier_walk2		},
//	{	"run_start",			soldier_start_run	},
//	{	"run",					soldier_run			},
//	{	"attack1",				soldier_attack1		},
//	{	"attack2",				soldier_attack2		},
//	{	"attack3",				soldier_attack3		},	//duck attack
//	{	"attack5",				soldier_attack6		},
//	{	"duck",					soldier_duck		},
	{	NULL,					NULL				}
};


// all potential hosts according to <g_spawn.c>
host_t hosts[] = 
{
//	{ "monster_berserk",		berserk		},
//	{ "monster_gladiator",		gladiator	},

//	{ "monster_gunner",			gunner		},
	{ "monster_infantry",		infantry,	NULL,					NULL					},

	{ "monster_soldier_light",	soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier",		soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier_ss",		soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},

//	{"monster_tank",			tank		},
//	{ "monster_tank_commander",	tank		},

//	{ "monster_medic",			medic		},
//	{ "monster_flipper",		flipper		},
//	{ "monster_chick",			chick		},
//	{ "monster_parasite",		parasite	},
//	{ "monster_flyer",			flyer		},

//	{ "monster_brain",			brain		},
//	{ "monster_floater",		floater		},
//	{ "monster_hover",			hover		},
//	{ "monster_mutant",			mutant		},	
//	{ "monster_supertank",		supertank	},

//	{ "monster_boss2",			boss2		},	
//	{ "monster_boss3_stand",	boss3_stand	},
//	{ "monster_jorg",			jorg		},

//	{"monster_commander_body", SP_monster_commander_body}, <-----NOT ACTUALLY A MONSTER ( do a case check for this? )...unless it has GOOD animations

	{ NULL,						NULL		}
};

void set_host_target( edict_t *host, trace_t *tr, qboolean show, int control_type );
void SP_Host_Target( edict_t *host, vec3_t origin, qboolean show);

// unlike G_SetClientFrame
// this only sets the currentmove and lets the monster resolve it
// instead of resolving individual frames here
// However, it does determine the conditions for currentmove interrupt/change
// on a per-frame basis
void set_host_move( edict_t *host, const pmove_t *pm ) {

	// find_host_move ( host, possible_move )
	// make it a little random if multiple versions of the same thing ( re-do if not found )
	//  m->hmove( host );	

	// host fidgets in place if aifunc or endfunc cant resolve ( double check this )*************
	// change: ai_func, thinkfunc, endfunc
	// change: m_move.c stuff ( because they contain goalentities and no_water_enter stuff )
	// change: g_combat.c stuff ( reaction to damage, etc )
	// verifgy m_monster.c stuff


	// how does the player handle swimming?
	// how do monsters handle fly/swim moves?

	// set the host's v_angle according to pm ( this will set the aim direction for shots and movement )
	// trouble with fly/swim ( or inadvertent fly/swim )?

	// clear out the host's enemies, etc ( re: M_ReactToDamage  from <g_combat.c> )


	float xyspeed;
	qboolean	duck, run, attack;
	qboolean	allowInterrupt;

	if ( host->host_anim_priority == ANIM_DEATH || host->host_anim_priority == ANIM_PAIN )
		{ return; }		// stay there

	xyspeed = sqrt( (float)(pm->s.velocity[0])*(float)(pm->s.velocity[0]) + (float)(pm->s.velocity[1])*(float)(pm->s.velocity[1]) );
	allowInterrupt = false;

	// only set when on the ground ( yoda ain't on the ground ... prollem? )***********
	// solution: put the host->owner inside the host for reals and match their origin & v_angle
	// and develop a real chasecam
	if ( pm->s.pm_flags & PMF_DUCKED ) 
		{ duck = true; }
	else 
		{ duck = false; }

	if (xyspeed) 
		{ run = true; }
	else 
		{ run = false; }

	if ( pm->cmd.buttons & BUTTON_ATTACK )
		{ attack = true; }
	else 
		{ attack = false; }

	// check for new attack, stand/duck and stop/go transitions
	if ( attack != host->host_anim_attack && host->host_anim_priority < ANIM_ATTACK ) // => either JUMP or BASIC
		{ allowInterrupt = true; }	
	else if ( duck != host->host_anim_duck && host->host_anim_priority < ANIM_DEATH ) // => either ATTACK, JUMP, or BASIC
		{ allowInterrupt = true; }
	else if ( run != host->host_anim_run && host->host_anim_priority == ANIM_BASIC )
		{ allowInterrupt = true; }
	else if ( !host->groundentity && host->host_anim_priority == ANIM_BASIC )
		{ allowInterrupt = true; }

//	if ( host->host_anim_priority == ANIM_JUMP )							//fix this nonsense 1of2
///	{
//		if (!host->groundentity)
//			return;		// stay there
//		ent->client->anim_priority = ANIM_BASIC;
//		ent->s.frame = FRAME_jump3;											//set the currentmove to jump *landing* frames
//		ent->client->anim_end = FRAME_jump6;								//create monster-specific helper func that sets the frame
//		return;
//	}

	if ( allowInterrupt ) {

		// find and begin a new monsterinfo.currentmove
		if ( attack ) { host->host_anim_priority = ANIM_ATTACK; }
		else { host->host_anim_priority = ANIM_BASIC; }

		host->host_anim_duck = duck;
		host->host_anim_run = run;
		host->host_anim_attack = attack;

//		if (!ent->groundentity)													//fix this nonsense 2of2
//		{
//			client->anim_priority = ANIM_JUMP;
//			if (ent->s.frame != FRAME_jump2)									//cycle a jump move first two frames
//				ent->s.frame = FRAME_jump1;										//create monster-specific helper func that sets the frame
//			client->anim_end = FRAME_jump2;
//		}
		if (run)
		{	// running
			if (duck)
			{
				if (attack)
				{
				//do a moving crouch attack
				}
				else
				{
				//do an moving crouch ( no attack )
				}
			}
			else
			{
				if (attack)
				{
				//do a running upright attack
				}
				else
				{
				//do a running move ( no attack )
				}
			}
		}
		else
		{	// standing
			if (duck)
			{
				if (attack)
				{
				//do a holding-still crouch attack
				}
				else
				{
				//do an holding-still crouch ( no attack )
				}
			}
			else
			{
				if (attack)
				{
				//do a holding-still standing attack
				}
				else
				{
				//do an holding-still stand ( no attack )
				}
			}
		}
	}

}


hmove_t * find_host_move ( edict_t *host, char *possible_move ) {		// possible issue with wrong address ( function or data )

	hmove_t		*m;

	for ( m = host->hmove_list; m->move_name; m++ ) {
		if ( !strcmp( m->move_name, possible_move ) ) {	
			// found it
			m->hmove ( host );		// test to verify i havent broken it yet
			return m;
		}
	}

	//gi.dprintf ( "%s doesn't have that specific host move defined\n", host->classname );
	return NULL;
}

void monster_think_possesed( edict_t *self, edict_t *host, const pmove_t *pm )
{ 
	trace_t		tr;
	edict_t		*target;
	char		*selected_move;

	if ( !self || !host ) { return; }

	if ( host->host_target ) { target = host->host_target; }
	else { target = NULL; }	

	if ( host->deadflag != DEAD_NO ) { 
		
		DropHost( self, HOST_DEATH ); 
		return; 
	}

	// hmove_list will not be set if a host is taken while in RODEO mode then switch to UBERHOST mode******( not really an issue )
	if ( self->client->soul_abilities & UBERHOST && host->hmove_list != NULL ) { 

		//set_host_move( host, pm );

		
		// works, host defaults to last move, host stops if target is too close ( as expected )
		if ( pm->cmd.buttons & BUTTON_ATTACK ) { 

			tr = GhostMuzzleTrace( self );	
			set_host_target( host, &tr, false, UBER_ATTACK );
			find_host_move ( host, "attack3" );
		} 
		
		//else { set_host_move ( host, "stand1" ); }	//set every ClientThink => doesn't give time to resolve all frames
	}
	else {

		//RODEO HOST CONTROLS
		if ( pm->cmd.buttons & BUTTON_ATTACK ) {

			tr = GhostMuzzleTrace( self );

			//set the movement goal, and enemy, based on the muzzle trace
			if ( tr.fraction < 1.0f ) { 

				if ( tr.ent && tr.ent->classname && !Q_strncasecmp( tr.ent->classname, "monster_", 8 ) ) {
				
					gi.centerprintf( self, "ATTACK %s!", tr.ent->classname );
					set_host_target( host, &tr, false, RODEO_ENEMY );

					// get rid of the benign target edict
					if ( target ) { G_FreeEdict( host->host_target ); }

				} else { 
					gi.centerprintf( self, "INTO THE LIGHT!" ); 
					set_host_target( host, &tr, true, RODEO_BENIGN );
				}

				//only call this once per player click
				//do a check for FLY|SWIM and call a better ai_func********************************
				if ( host && host->monsterinfo.run ) { host->monsterinfo.run( host ); } 
			}
		}
	}
}

void TakeHost ( edict_t *self, edict_t *host, int take_style ) { 

	qboolean foundit =  false;
	host_t	*h;

	for ( h = hosts; h->host_name; h++ ) {

		if ( !strcmp( h->host_name, host->classname ) ) {

			// found it, set the available monsterinfo.currentmove(s)
			foundit = true;
			if ( self->client->soul_abilities & UBERHOST ) { host->hmove_list = h->host_moves; }
			else { host->hmove_list = NULL; }	// Rodeo host controls
			
			take_host_noise = h->takeNoise;
			drop_host_noise = h->dropNoise;
		}
	}

	// only monster_ classnames are taken ( should only !foundit for the headless strogg_commander )
	if ( !foundit ) { 
		gi.dprintf ( "%s doesn't have any host moves defined\n", host->classname );
		return;
	} else {
		//some debug prints
		if ( host->hmove_list ) { gi.dprintf( "UBERHOST\n" ); }
		else { gi.dprintf( "GIDDYUP\n" ); }
	}

	//make this monster-specific
	gi.sound ( host, CHAN_VOICE, gi.soundindex (take_host_noise), 1, ATTN_NORM, 0);		//potential crash issue if NULL?

	self->client->host		= host;
	host->possesed			= true;
	host->old_owner			= host->owner;				// potentially null, but sould be okay
	host->owner				= self;						// to prevent clipping
	self->client->ghostmode = false;
	self->client->hostmode	= true;

	host->possesed_think = monster_think_possesed;		// should this be left hanging when host is dropped?
				
	//develop a proper chasecam
	SetChaseTarget( self, host );

	switch ( take_style ) {
		case HOST_TOUCH:	{ gi.centerprintf (self, "TOUCH POSSESSION OF: %s\n", host->classname ); break; }
		case HOST_RADIAL:	{ gi.centerprintf( self, "RADIAL POSSESSION OF: %s\n", host->classname ); break; }
		case HOST_TARGETED:	{ gi.centerprintf (self, "TARGETED POSSESSION OF: %s\n", host->classname ); break;}
		case HOST_CHEAT:	{ break;}
	}
}

void DropHost ( edict_t *self, int drop_style ) 
{
	//make monster-specific
	gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( drop_host_noise ), 1, ATTN_NORM, 0);	//potential crash issue if NULL?

	switch ( drop_style ) {

		case HOST_NO_HARM: { gi.centerprintf( self, "HOST LEFT UNHARMED, GHOST MODE ENABLED\n" ); break; }

		case HOST_KILL: { 

			// nail it ( courtesy of KillBox )
			T_Damage ( self->client->host, self, self, vec3_origin, self->s.origin, vec3_origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
			gi.centerprintf( self, "HOST OBLITERATED, GHOST MODE ENABLED\n" );
			break; 
		}

		case HOST_DEATH: { gi.centerprintf( self, "HOST DIED ON ITS OWN, GHOST MODE ENABLED\n" ); break; }
	}

	if ( self->client->host->host_target ) { G_FreeEdict( self->client->host->host_target ); }

	self->client->host->possesed		= false;
	self->client->host->owner			= self->client->host->old_owner;	// potentially null
	self->client->host					= NULL;
	self->client->hostmode				= false;
	self->client->ghostmode				= true;
	self->client->nextPossessTime		= level.time + 3.0f;
}

void host_target_touch( edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf ) {

	if ( self && other && other->possesed ) {

		Com_Printf( "STOP MOVING HOST!\n" );

		// check for fly | swim too**********************************************
		if ( other->monsterinfo.stand ) {

			other->monsterinfo.aiflags = 0;
			other->monsterinfo.aiflags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);

			other->goalentity = 
			other->movetarget = 
			other->target_ent = 
			other->oldenemy =
			other->enemy = NULL;

			other->monsterinfo.stand( other ); 
		} 
		G_FreeEdict ( self );
	}

	return;
}

// USE CASES:
// 1) Rodeo controls spawn/move a ball of light to chase						( covered )
// 2) Rodeo controls set a monster to a direct-enemy							( covered )
// 3) Uberhost controls attack the clicked point ( similar to player attacks )	(  )
// 4) Uberhost controls move to a specified origin								(  )
void set_host_target( edict_t *host, trace_t *tr, qboolean show, int control_type ) {

	// move/spawn somthing for the host to chase/attack
	if ( host->host_target && !Q_strcasecmp( host->host_target->classname, "host_target" ) ) {

		VectorCopy ( tr->endpos, host->host_target->s.origin );
		gi.linkentity ( host->host_target );

	} else { SP_Host_Target( host, tr->endpos, show ); }

	host->monsterinfo.aiflags = 0;

	switch ( control_type ) {

		case RODEO_BENIGN: {
			host->goalentity = host->movetarget = host->host_target;
			host->target_ent = host->enemy = host->oldenemy = NULL;	
			host->monsterinfo.aiflags = AI_COMBAT_POINT;
			break;
		}

		case RODEO_ENEMY: {
			host->goalentity = 	host->movetarget =	host->oldenemy = host->enemy = tr->ent;
			host->target_ent = NULL;
			break;
		}

		case UBER_ATTACK: {
			host->oldenemy = host->enemy = host->host_target;
			host->goalentity = 	host->movetarget = host->target_ent = NULL;
			break;
		}

		case UBER_MOVE: {

			break;
		}
	}
}

void SP_Host_Target ( edict_t *host, vec3_t origin, qboolean show ) {
	
	vec3_t		ht_mins		= {-16, -16, -24};
	vec3_t		ht_maxs		= {16, 16, 32};
	int			savedEntNumber;

	edict_t		*targ;

	host->host_target	= G_Spawn();
	targ				= host->host_target;
	savedEntNumber		= targ->s.number;

	//prevent memory overload, and make sure there's nothing leftover
	memset( targ, 0, sizeof(*targ) );

	targ->inuse			= true;
	targ->gravity		= host->gravity;
	targ->s.number		= savedEntNumber;

	VectorCopy( origin, targ->s.origin );

	VectorCopy( ht_mins, targ->mins );
	VectorCopy( ht_maxs, targ->maxs );

	if ( show ) { targ->s.modelindex	= gi.modelindex ("sprites/s_bfg1.sp2"); }
	
	targ->solid			= SOLID_TRIGGER;
	targ->clipmask		= (CONTENTS_PLAYERCLIP|CONTENTS_MONSTERCLIP|CONTENTS_TRANSLUCENT);
	targ->s.effects		= EF_BFG|EF_ANIM_ALLFAST;

	targ->flags		   |= FL_NO_KNOCKBACK;
	targ->takedamage	= DAMAGE_NO;
	targ->movetype		= MOVETYPE_NONE;
	targ->touch			= host_target_touch;			// change this depending on UBERHOST status???
	//targ->owner		= host->owner;					// not necessary
	targ->classname		= "host_target";
	
	gi.linkentity( targ );
}

//*******************
// TMF7 END GHOST MODE
//*******************