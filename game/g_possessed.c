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
#include "m_player.h"

void set_host_target( edict_t *host, edict_t *other, vec3_t origin, qboolean show, int control_type );
void SP_Host_Target( edict_t *host, vec3_t origin, qboolean show);

// all potential hosts according to <g_spawn.c>
host_t hosts[] = 
{
	{ "monster_berserk",		berserk,	NULL,					NULL,						2, {-16, -16, -24}, {16, 16, 32}, "Beserker Soul" 	},
	{ "monster_gladiator",		gladiator,	NULL,					NULL,						4, {-32, -32, -24}, {32, 32, 64}, "Gladiator Soul"	},

	{ "monster_gunner",			gunner,		NULL,					NULL,						2, {-16, -16, -24}, {16, 16, 32}, "Gunner Soul"		},
	{ "monster_infantry",		infantry,	NULL,					NULL,						1, {-16, -16, -24}, {16, 16, 32}, "Infantry Soul"	},

	{ "monster_soldier_light",	soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav",		1, {-16, -16, -24}, {16, 16, 32}, "Solider Soul"	},
	{ "monster_soldier",		soldier,	"soldier/solpain1.wav", "soldier/solpain1.wav",		1, {-16, -16, -24}, {16, 16, 32}, "Solider Soul"	},
	{ "monster_soldier_ss",		soldier,	"soldier/solpain3.wav", "soldier/solpain3.wav",		1, {-16, -16, -24}, {16, 16, 32}, "Solider Soul"	},

	{ "monster_tank",			tank,		NULL,					NULL,						4, {-32, -32, -16}, {32, 32, 72}, "Tank Soul"		},
	{ "monster_tank_commander",	tank,		NULL,					NULL,						4, {-32, -32, -16}, {32, 32, 72}, "Commander Soul"	},

	{ "monster_medic",			medic,		NULL,					NULL,						4, {-24, -24, -24}, {24, 24, 32}, "Medic Soul"		},
	{ "monster_flipper",		flipper,	NULL,					NULL,						2, {-16, -16, 0 },  {16, 16, 32}, "Flipper Soul"	},
	{ "monster_chick",			chick,		NULL,					NULL,						3, {-16, -16, 0},   {16, 16, 56}, "Maiden Soul"		},
	{ "monster_parasite",		parasite,	NULL,					NULL,						2, {-16, -16, -24}, {16, 16, 24}, "Parasite Soul" 	},
	{ "monster_flyer",			flyer,		NULL,					NULL,						2, {-16, -16, -24}, {16, 16, 32}, "Flyer Soul"		},

	{ "monster_brain",			brain,		NULL,					NULL,						4, {-16, -16, -24}, {16, 16, 32}, "Brain Soul"		},
	{ "monster_floater",		floater,	NULL,					NULL,						3, {-24, -24, -24}, {24, 24, 32}, "Floater Soul"	},
	{ "monster_hover",			hover,		NULL,					NULL,						3, {-24, -24, -24}, {24, 24, 32}, "Hover Soul"		},
	{ "monster_mutant",			mutant,		NULL,					NULL,						3, {-32, -32, -24}, {32, 32, 48}, "Mutant Soul"		},	
	{ "monster_supertank",		supertank,	NULL,					NULL,						5, {-64, -64, 0},  {64, 64, 112}, "Supertank Soul"	},

	{ "monster_boss2",			boss2,		NULL,					NULL,						5, {-56, -56, 0},  {56, 56, 80},  "Hornet Soul"		},	
	{ "monster_boss3",			boss3,		NULL,					NULL,						5, {-30, -30, 0},  {30, 30, 90},  "Makron Soul"		},
	{ "monster_jorg",			jorg,		NULL,					NULL,						5, {-80, -80, 0},  {80, 80, 140}, "Jorg Soul"		},

//	{"monster_boss3_stand", SP_monster_boss3_stand},	   <----NOT A MONSTER WITH PROPER MOVES
//	{"monster_commander_body", SP_monster_commander_body}, <-----NOT ACTUALLY A MONSTER ( do a case check for this? )...unless it has GOOD animations

	{ NULL,	NULL, NULL, NULL, NULL, {0,0,0}, {0,0,0}, NULL }
};

char * GetMonsterByIndex( int index )
{
	if ( index < 0 || index >= UNIQUE_SOUL_TYPES+2 )
		return NULL;

	return hosts[index].soul_name;
}

int GetIndexByMonster( char *classname ) {

	host_t *monster;

	for ( monster = hosts; monster->host_class; monster++ )
		if ( !strcmp( monster->host_class, classname ) )
			return (monster-hosts);
	
	return -1;
}

const char * GetSoulByMonster( char *classname ) {

	host_t *monster;

	for ( monster = hosts; monster->host_class; monster++ )
		if ( !strcmp( monster->host_class, classname ) )
			return monster->soul_name;
	
	return NULL;
}

// group the soldiers into one index
int GetCollectionIndex( char *classname ) {

	host_t *monster;

	for ( monster = hosts; monster->host_class; monster++ ) {
		if ( !strcmp( monster->soul_name, classname ) ) {

			if ( (monster-hosts) == SOLDIER || (monster-hosts) == SOLDIER_SS )
				return SOLDIER_LIGHT;

			else if ( (monster-hosts) > SOLDIER_SS )
				return (monster-hosts)-2;

			else
				return (monster-hosts);
		}
	}
}

char * GetCollectionHudName( int index ) {

	if ( index > SOLDIER_LIGHT )
		return hosts[index+2].soul_name;
	else
		return hosts[index].soul_name;
}

hmove_t * find_host_move ( edict_t *host, char *possible_move ) {

	hmove_t		*m;

	if ( host->hmove_list == NULL ) { return; }

	for ( m = host->hmove_list; m->move_name; m++ ) {
		if ( !strcmp( m->move_name, possible_move ) ) {	
			// found it
			return m;
		}
	}

	//gi.dprintf ( "%s doesn't have that specific host move defined\n", host->classname );
	return NULL;
}

// unlike G_SetClientFrame
// this only sets the currentmove and lets the monster resolve it
// instead of resolving individual frames here
// However, it does determine the conditions for currentmove interrupt/change
// on a per-frame basis
void set_host_move( edict_t *self, edict_t *host, const pmove_t *pm ) {

	// change: ai_func, thinkfunc, endfunc
	// change: m_move.c stuff ( because they contain goalentities and no_water_enter stuff )
	// change: g_combat.c stuff ( reaction to damage, etc )
	// verify m_monster.c stuff

	float xyspeed;
	qboolean	duck, run, slow, attack;
	qboolean	allowInterrupt;
	hmove_t		*perform_move;

	if ( host->host_anim_priority == ANIM_DEATH || host->host_anim_priority == ANIM_PAIN )
		{ return; }		// stay there

	// set the host angles for the aim direction and movement ( see: ClientThink )
	VectorCopy( pm->viewangles, host->s.angles ); // PITCH YAW ROLL
	host->monsterinfo.aiflags = 0;

	// tested: 0 = stand, 200 = walk, 400 = run ( holding forward and side causes a run-fast also )
	xyspeed = sqrt( (float)(pm->cmd.forwardmove)*(float)(pm->cmd.forwardmove) + (float)(pm->cmd.sidemove)*(float)(pm->cmd.sidemove) );
	allowInterrupt = false;
	perform_move = NULL;

	gi.linkentity( host );		// the angles were changed

	if ( pm->cmd.upmove < 0 ) //if ( pm->s.pm_flags & PMF_DUCKED ) // PMF_DUCKED only set when user has a groundentity
		{ duck = true; }
	else 
		{ duck = false; }

	if (xyspeed) { 
		run = true;

		if ( xyspeed == 200 ) { slow = true; }
		else { slow = false; } // xyspeed == 400
	}
	else 
		{ run = false; }

	if ( self->client->latched_buttons & BUTTON_ATTACK ) {
		self->client->latched_buttons &= ~BUTTON_ATTACK;
		attack = true; 
	} 
	else 
		{ attack = false; }

	// check for new attack, stand/duck and stop/go transitions
	if ( attack != host->host_anim_attack && host->host_anim_priority == ANIM_BASIC )
		{ allowInterrupt = true; }	
	if ( duck != host->host_anim_duck && host->host_anim_priority <= ANIM_ATTACK) // => either ATTACK or BASIC
		{ allowInterrupt = true; }
	if ( run != host->host_anim_run && host->host_anim_priority == ANIM_BASIC )
		{ allowInterrupt = true; }
	if ( slow != host->host_anim_walk && host->host_anim_priority == ANIM_BASIC )
		{ allowInterrupt = true; }
//	if ( !host->groundentity && host->host_anim_priority == ANIM_BASIC )		// jumping, flying, swimming ?
//		{ allowInterrupt = true; }

	if ( allowInterrupt ) {

		// find and begin a new monsterinfo.currentmove
		if ( attack ) { host->host_anim_priority = ANIM_ATTACK; }
		else { host->host_anim_priority = ANIM_BASIC; }

		host->host_anim_duck = duck;
		host->host_anim_run = run;
		host->host_anim_walk = slow;
		host->host_anim_attack = attack;

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
			{	if (slow) 
				{
					if (attack)
					{
						//do a "walking" upright attack ( really just stand still )
						perform_move = find_host_move ( host, "attack1" ); // or try "attack4" or "attack2"
					}
					else
					{
						//do a walking move ( no attack )
						perform_move = find_host_move ( host, "walk2" ); // or try "walk1"
					}
				}
				else
				{
					if (attack)
					{
						//do a running upright attack
						perform_move = find_host_move ( host, "run_attack" );
					}
					else
					{
						//do a running move ( no attack )
						perform_move = find_host_move ( host, "run_start" ); // or try just "run"
					}
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
					perform_move = find_host_move ( host, "duck_attack" );
				}
				else
				{
					//do an holding-still crouch ( no attack )
					perform_move = find_host_move ( host, "duck" );
				}
			}
			else
			{
				if (attack)
				{
					//do a holding-still standing attack
					perform_move = find_host_move ( host, "attack1" ); // or try "attack4" or "attack2"
				}
				else
				{
					//do an holding-still stand ( no attack )
					perform_move = find_host_move ( host, "stand1" ); // or try "stand2"
				}
			}
		}
	}

	if ( perform_move && perform_move->hmove ) { perform_move->hmove( host ); }
}

edict_t * CheckEnemy( edict_t *ent ) {

	edict_t *other;

	//return the first entity that sees ent as an enemy
	for ( other = &g_edicts[1]; other < &g_edicts[globals.num_edicts]; other++ ) {
			
		if ( !other || !other->inuse )
			continue;

		if ( other->deadflag != DEAD_NO )
			continue;

		if ( other->enemy == ent )
			return other;
	}

	return NULL;
}

// tr.ent/host_follower needs to know who its listening to ( owner )
// self needs to know who its ordering around ( host_follower )
void HostSpeak( edict_t *self, int input_type) {

	int num, helpCount;
	trace_t tr;
	edict_t *other;
	edict_t *enemy;

	switch ( input_type ) {

		case MOUSE_ONE: {

			tr = GhostMuzzleTrace( self );

			if ( tr.fraction < 1.0f && tr.ent ) {

				if ( !Q_strncasecmp( tr.ent->classname, "monster_", 8 )	&& tr.ent->deadflag == DEAD_NO ) {

					// its listening to/possessed by another good guy right now
					if ( !self->client->host_follower && tr.ent->owner && tr.ent->owner->client && tr.ent->owner != self ) { 
						self->client->host->hostLaugh = true; 
					} 

					// have follower attack this monster
					else if ( self->client->host_follower ) { 
						set_host_target( self->client->host_follower, tr.ent, NULL, false, RODEO_ENEMY );
						self->client->orderIssued  = true;
					}

					// get this monster's attention
					else { 
						gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/hi.wav" ), 1, ATTN_NORM, 0);
						TakeFollower( self, tr.ent );
					}
				} 
				
				// tell follower to shoot a barrel or a non-owner client ( or its husk )
				else if ( self->client->host_follower 
					&& ( !Q_strcasecmp( tr.ent->classname, "misc_explobox" ) || !Q_strncasecmp( tr.ent->classname, "player", 6 ) ) ) {

					set_host_target( self->client->host_follower, tr.ent, NULL, false, RODEO_ENEMY );
					self->client->orderIssued  = true;
				} 
				
				// tell follower to move somewhere or chase owner
				else if ( tr.ent == world && self->client->host_follower ) {
					
					if ( level.time < self->client->chaseHostTime )
						set_host_target( self->client->host_follower, self, NULL, false, RODEO_FOLLOW );
					else
						set_host_target( self->client->host_follower, NULL, tr.endpos, true, RODEO_BENIGN );

					self->client->orderIssued  = true;
				} 

				// lonely moment ( no follower )
				else { 
					num = (rand()%2)+1;
					gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( va("slighost/buzz%i.wav", num) ), 1, ATTN_NORM, 0); 
				}
			}
			break;
		}

		case MOUSE_THREE: { 

			// go away
			if ( self->client->buttons & BUTTON_SHIFT ) {
				gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/stay.wav" ), 1, ATTN_NORM, 0);
				DropFollower( self );

			} 
			
			// everybody run
			else if ( self->client->buttons & BUTTON_ALT ) {
				gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/lookout.wav" ), 1, ATTN_NORM, 0);

				other = NULL;
				while ( ( other = findradius( other, self->s.origin, LIFE_RANGE ) ) != NULL ) {

					if ( other->client || other == self->client->host )
						continue; 

					if ( !(other->svflags & SVF_MONSTER) )
						continue;

					if ( other->deadflag != DEAD_NO )
						continue;

					if ( other->monsterinfo.run ) {
						other->monsterinfo.aiflags = AI_COMBAT_POINT;
						other->monsterinfo.run( other );
					}
				}
			} 
			
			// get some temporary aid
			// opponents can steal the aid
			else {

				// defend the husk, the host, or the follower ( in that order )
				enemy = CheckEnemy( self->client->player_husk );

				if ( !enemy )
					enemy = CheckEnemy( self->client->host );

				if ( !enemy && self->client->host_follower )
					enemy = CheckEnemy( self->client->host_follower ); 

				if ( enemy ) {
					gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/help.wav" ), 1, ATTN_NORM, 0);

					//set enemy of all monsters in range to current enemy
					helpCount = 0;
					other = NULL;
					while ( ( other = findradius( other, self->s.origin, LIFE_RANGE ) ) != NULL ) {

						if ( other == self || other == enemy )
							continue; 

						if ( !(other->svflags & SVF_MONSTER) || (other->client) )
							continue;

						if ( other->deadflag != DEAD_NO )
							continue;

						if ( !Q_strncasecmp( other->classname, "monster_", 8 ) ) {
							set_host_target( other, enemy, NULL, true, RODEO_ENEMY );
							helpCount++;
						}
					}

					if ( !helpCount ) { gi.centerprintf ( self, "YOUR ON YOUR OWN\n" ); }
				}
				else
					gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/what.wav" ), 1, ATTN_NORM, 0);
			}
			break;
		}
	}

	self->client->chaseHostTime = level.time + DOUBLE_CLICK;
}

// precondition: a valid host exists
void monster_think_possesed( edict_t *self, edict_t *host, const pmove_t *pm )
{ 
	char		*selected_move;
	float		yaw;
	trace_t		tr;

	// keep the player centered on the host
	// if host->owner != self, then self will get stuck
	// x and y match fine, but z must be checked because not all hosts origins are created at BBOX center
	// just align the BBOX bottoms
	self->s.origin[0] = self->client->host->s.origin[0];
	self->s.origin[1] = self->client->host->s.origin[1];

	//essenstially absmin[2] - mins[2], except using absmin[2] causes vertical shake
	self->s.origin[2] = (self->client->host->mins[2] + self->client->host->s.origin[2]) - self->mins[2]; 
	gi.linkentity(self);
	
	if ( (self->client->soul_abilities & UBERHOST) && host->hmove_list ) { 

		self->client->orderIssued = false;

		if ( (self->client->latched_buttons & BUTTON_ATTACK) && ( self->client->buttons & BUTTON_SHIFT ) ) {
			
			self->client->latched_buttons &= ~BUTTON_ATTACK;

			// hostspeak 1of2
			HostSpeak( self, MOUSE_ONE );
		} 
		
		// normal uberhost controls
		else { set_host_move( self, host, pm ); }

		if ( self->client->orderIssued ) { self->client->tempListener = false; }

		if ( level.time > self->client->giveOrdersTime 
			&& self->client->tempListener
			&& !self->client->orderIssued ) {

			DropFollower( self );
		}
	} 
	
	// rodeo host controls
	else {

		self->client->host->s.angles[ROLL] = 0;
		self->client->host->s.angles[PITCH] = 0;

		if ( self->client->latched_buttons & BUTTON_ATTACK ) {

			self->client->latched_buttons &= ~BUTTON_ATTACK;

			tr = GhostMuzzleTrace( self );

			//set the movement goal, and enemy, based on the muzzle trace
			if ( tr.fraction < 1.0f ) { 

				if ( tr.ent->svflags & SVF_MONSTER ||  !Q_strncasecmp( tr.ent->classname, "player", 6 ) || !Q_strcasecmp( tr.ent->classname, "misc_explobox" ) ) {
				
					//gi.centerprintf( self, "ATTACK %s!", tr.ent->classname );
					set_host_target( host, tr.ent, NULL, false, RODEO_ENEMY );

				} else { 
					//gi.centerprintf( self, "INTO THE LIGHT!" ); 
					set_host_target( host, NULL, tr.endpos, true, RODEO_BENIGN );
				}
			}
		}
	}

	if ( host->hostLaugh ) {
		gi.sound( host, CHAN_VOICE, gi.soundindex( "slighost/chuckle.wav" ), 1, ATTN_NORM, 0.1 );
		host->hostLaugh = false;
	}
}

void TakeHost ( edict_t *self, edict_t *host, int take_style ) { 

	host_t	*h;

	host->hmove_list = NULL;

	// coop soul collectors
	if ( host->owner && host->owner->client && host->owner->client->hostmode && host->owner != self ) {
		gi.centerprintf ( self, "%s occupied by %s\n", host->classname, host->owner->client->pers.netname );
		return;
	}

	for ( h = hosts; h->host_class; h++ ) {

		if ( !strcmp( h->host_class, host->classname ) ) {

			// found it, grab the available monsterinfo.currentmove(s)
			host->hmove_list = h->host_moves;
			host->take_host_noise = h->takeNoise;
			host->drop_host_noise = h->dropNoise;
		}
	}

	// only takes hosts with hmoves ( even if empty )
	if ( !host->hmove_list ) { 
		gi.dprintf ( "%s doesn't have any host moves defined\n", host->classname );
		return;
	}

	gi.sound ( host, CHAN_VOICE, gi.soundindex (host->take_host_noise), 1, ATTN_NORM, 0);

	self->client->host		= host;
	host->possessed			= true;
	host->owner				= self;						// to prevent clipping
	self->client->ghostmode = false;
	self->client->hostmode	= true;
//	self->client->ps.pmove.pm_type = PM_SPECTATOR;

	// hide the player model
	self->svflags			= SVF_NOCLIENT;

	host->possesed_think = monster_think_possesed;		// should this be left hanging when host is dropped? shouldn't matter
				
	//develop a proper chasecam
	//SetChaseTarget( self, host );

	switch ( take_style ) {
		case HOST_TOUCH:	{ gi.centerprintf (self, "TOUCH POSSESSION OF: %s\n", host->classname ); break; }
		case HOST_RADIAL:	{ gi.centerprintf( self, "RADIAL POSSESSION OF: %s\n", host->classname ); break; }
		case HOST_TARGETED:	{ gi.centerprintf (self, "TARGETED POSSESSION OF: %s\n", host->classname ); break;}
		case HOST_TRANSFORM:	{ gi.centerprintf (self, "HOST UPGRADE TO: %s\n", host->classname ); break;}
	}
}

void DropHost ( edict_t *self, int drop_style ) 
{
	hmove_t *perform_move;
	edict_t *host;

	host = self->client->host;

	if ( !host )
		return;
	else if ( !host->inuse || strncmp( host->classname, "monster_", 8 ) ) {
		self->client->host = NULL;
		return;
	}

	gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( self->client->host->drop_host_noise ), 1, ATTN_NORM, 0);

	switch ( drop_style ) {

		case HOST_NO_HARM: { gi.centerprintf( self, "HOST LEFT UNHARMED, GHOST MODE ENABLED\n" ); break; }

		case HOST_KILL: { 

			// nail it ( courtesy of KillBox )
			T_Damage ( self->client->host, self, self, vec3_origin, self->s.origin, vec3_origin, 100000, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
			gi.centerprintf( self, "HOST OBLITERATED, GHOST MODE ENABLED\n" );
			break; 
		}

		case HOST_DEATH: { gi.centerprintf( self, "HOST DIED ON ITS OWN, GHOST MODE ENABLED\n" ); break; }
		
		case HOST_HUSK_DEATH: { gi.centerprintf( self, "YOU DIED\n" ); break; }
	}

	if ( self->client->host->host_target )	{ G_FreeEdict( self->client->host->host_target ); }
	if ( self->client->host_follower )		{ DropFollower( self ); }

	//prevent NULL-enemy crash mid-fire in uberhost mode
	if ( self->client->host->deadflag == DEAD_NO ) {
		
		perform_move = find_host_move ( self->client->host, "stand1" ); // or try "stand2"
		if ( perform_move && perform_move->hmove ) { perform_move->hmove( self->client->host ); }
	}

	// retain ownership for the duration of the cooldown (to avoid getting stuck)
	//self->client->host->owner			= NULL;
	self->client->escape_host			= host;

	self->client->host->s.angles[ROLL]	= 0;
	self->client->host->s.angles[PITCH] = 0;
	self->client->host->possessed		= false;
	self->client->host					= NULL;
	self->client->hostmode				= false;
	self->client->ghostmode				= true;
	self->client->nextPossessTime		= level.time + 3.0f;
//	self->client->ps.pmove.pm_type		= PM_NORMAL;

	// show the player model again
	self->svflags						= 0;
}

void TakeFollower( edict_t *self, edict_t *follower ) {

	follower->owner					= self;
	self->client->host_follower		= follower;
	follower->host_target			= self;
	self->client->tempListener		= true;
	self->client->giveOrdersTime	= level.time + 3.0f;
	follower->hostLaugh				= true;

	follower->followerOldMove		= follower->monsterinfo.currentmove;
	follower->followerOldMoveTarget = self->client->host_follower->movetarget;

	if ( follower->monsterinfo.stand ) {

		follower->monsterinfo.aiflags = 0;
		follower->monsterinfo.aiflags |= (AI_STAND_GROUND | AI_TEMP_STAND_GROUND);

		follower->goalentity = 
		follower->movetarget = 
		follower->target_ent = 
		follower->oldenemy =
		follower->enemy = NULL;

		follower->monsterinfo.stand( follower ); 
	}
}

// precondition: self has a host and a follower
void DropFollower( edict_t *self ) {

	edict_t *follower;

	follower = self->client->host_follower;

	if ( !follower )
		return;
	else if ( !follower->inuse || strncmp( follower->classname, "monster_", 8 ) ) {
		self->client->host_follower = NULL;
		return;
	}

	if ( follower->deadflag == DEAD_NO ) {

		// though keep the current enemy 
		follower->goalentity = follower->followerOldMoveTarget;
		follower->monsterinfo.currentmove = follower->followerOldMove;
	}

	// get rid of any leftover benign target edict
	if ( follower->host_target && !Q_strcasecmp( follower->host_target->classname, "host_target" ) )  
		G_FreeEdict( follower->host_target ); 

	gi.dprintf( "FOLLOWER DROPPED\n" );

	gi.sound ( self->client->host, CHAN_VOICE, gi.soundindex( "slighost/bleh.wav" ), 1, ATTN_NORM, 0 );
	self->client->tempListener = false;
	follower->owner = NULL;
	follower->host_target = NULL;
	self->client->host_follower = NULL;
}

void host_target_touch( edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf ) {

	// any possessed or follower monsters can touch this point
	// no matter who they belong to
	// provides potential to derail opponent's monsters
	if ( other->owner && other->owner->client ) {

		//Com_Printf( "STOP MOVING!\n" );

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

		gi.sound (other, CHAN_VOICE, gi.soundindex ("slighost/freeze.wav"), 1, ATTN_NORM, 0);
		G_FreeEdict ( self );
	}

	return;
}

// USE CASES:
// 1) spawn/move a ball of light to chase
// 2) set a monster to a direct-enemy
// 3) set a monster to harmlessly chase its owner
void set_host_target( edict_t *host, edict_t *other, vec3_t origin, qboolean show, int control_type ) {

	host->monsterinfo.aiflags = 0;

	switch ( control_type ) {

		case RODEO_BENIGN: {

			// move or spawn somthing for the host to chase
			if ( host->host_target && !Q_strcasecmp( host->host_target->classname, "host_target" ) ) {

				VectorCopy ( origin, host->host_target->s.origin );
				gi.linkentity ( host->host_target );

			} 
			else { 
				SP_Host_Target( host, origin, show ); 
			}

			host->goalentity = host->movetarget =  host->host_target;
			host->target_ent = host->enemy = host->oldenemy = NULL;	
			host->monsterinfo.aiflags = AI_COMBAT_POINT;
			gi.sound( host, CHAN_VOICE, gi.soundindex( "slighost/hereboy.wav" ), 1, ATTN_NORM, 0 );
			break;
		}

		case RODEO_ENEMY: {

			host->goalentity = 	host->movetarget =	host->oldenemy = host->enemy = other;
			host->target_ent = NULL;
			if ( !show )
				gi.sound (host, CHAN_VOICE, gi.soundindex ("slighost/getem.wav"), 1, ATTN_NORM, 0);

			// get rid of any leftover benign target edict
			if ( host->host_target && !Q_strcasecmp( host->host_target->classname, "host_target" ) )
				G_FreeEdict( host->host_target );
			break;
		}

		case RODEO_FOLLOW: {
			host->goalentity = host->movetarget = other;
			host->target_ent = host->enemy = host->oldenemy = NULL;	
			host->monsterinfo.aiflags = AI_COMBAT_POINT;
			gi.sound( host, CHAN_VOICE, gi.soundindex( "slighost/hereboy.wav" ), 1, ATTN_NORM, 0 );

			// get rid of any leftover benign target edict
			if ( host->host_target && !Q_strcasecmp( host->host_target->classname, "host_target" ) )
				G_FreeEdict( host->host_target );
			break;
		}
	}

	if ( host->monsterinfo.run )
		host->monsterinfo.run( host ); 
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
	targ->s.effects		= (EF_BFG|EF_ANIM_ALLFAST);

	targ->flags		   |= FL_NO_KNOCKBACK;
	targ->takedamage	= DAMAGE_NO;
	targ->movetype		= MOVETYPE_NONE;
	targ->touch			= host_target_touch;
	//targ->owner		= host->owner;					// not necessary
	targ->classname		= "host_target";
	
	gi.linkentity( targ );
}


// jump towards the pulling player ( owner )
void monster_soul_pull ( edict_t *self, edict_t *soul ) {

	int num;
	vec3_t dir;
	char *sound;

	num = rand()%2;

	sound = va( "soul/pullsoul%i.wav", num );
	gi.sound (soul, CHAN_VOICE, gi.soundindex( sound ), 1, ATTN_NORM, 0);

	VectorSubtract ( self->s.origin, soul->s.origin, dir );
	VectorNormalize( dir );

	soul->s.origin[2] += 1;
	VectorScale ( dir, 600, soul->velocity );
	soul->velocity[2] = 250;
	soul->groundentity = NULL;

	gi.linkentity( soul );
}

void monster_soul_touch ( edict_t *soul, edict_t *other, cplane_t *plane, csurface_t *surf ) {

	int old_soul_collector_level;
	int new_soul_collector_level;
	int index;
	
	// only living clients in ghostmode can gather souls
	if ( !other->client || other->deadflag != DEAD_NO || !other->client->ghostmode ) 
		{ return; }

	//pickup verification
	other->client->bonus_alpha = 0.35;
	gi.sound ( other, CHAN_ITEM, gi.soundindex("soul/pickupsoul.wav") , 1, ATTN_NORM, 0);

// BEGIN ( ghud )
	other->client->soulChange = true;

	// the "string" is a hardcoded value via SetMonsterNames(void) initialized at worldspawn
	other->client->ps.stats[STAT_SOULS_STRING] = CS_ITEMS+gi.configindex( soul->classname ); 
	other->client->pickup_soul_msg_time = level.time + 3.0;

	other->client->newSoulLevel = false;
// END ( ghud )

	old_soul_collector_level = other->client->soul_collector_level;
	other->client->pool_of_souls += soul->mass;

	index = GetCollectionIndex(soul->classname);
	other->client->soulCollection[ index ]++;
	
	// balance these level thresholds better
													  new_soul_collector_level = 1;
	if (	other->client->pool_of_souls >= 5	)	{ new_soul_collector_level = 2; }
	if (	other->client->pool_of_souls >= 30	)	{ new_soul_collector_level = 3; }
	if (	other->client->pool_of_souls >= 100	)	{ new_soul_collector_level = 4; }
	if (	other->client->pool_of_souls >= 500	)	{ new_soul_collector_level = 5; }

	if ( new_soul_collector_level > old_soul_collector_level ) {

		other->client->soul_collector_level = new_soul_collector_level;
		LevelUpSoulCollector( other );
	}

	G_FreeEdict( soul );
}

void monster_soul_think ( edict_t *soul ) {

	int num;
	char *sound;
	vec3_t point;
	edict_t *ent;

	if ( level.time >= soul->soulSpawnTime ) {	
		soul->touch = monster_soul_touch;
		
		num = rand()%5;

		sound = va( "soul/wailingsoul%i.wav", num );
		gi.sound (soul, CHAN_VOICE, gi.soundindex( sound ), 1, ATTN_NORM, 0);

		soul->soulSpawnTime = level.time + 10.0;
	}

	if ( soul->owner && soul->groundentity ) {
		monster_soul_pull( soul->owner, soul );
	}

	//check if any one client is within touching distance
	if ( soul->touch ) {
		for ( ent = &g_edicts[1]; ent <= &g_edicts[game.maxclients]; ent++ )
			if ( ent && ent->inuse && ent->client->ghostmode ) {

			VectorSubtract( ent->s.origin, soul->s.origin, point );

			if ( VectorLength(point) < GHOST_RANGE ) { 	
				soul->touch( soul, ent, NULL, NULL ); 
				return;
			}
		}
	}

	//jump back to idle animation if outside it
	if ( soul->s.frame > soul->mSoulLastFrame || soul->s.frame < soul->mSoulFirstFrame) { soul->s.frame = soul->mSoulFirstFrame; }

	//continue idle animation
	if ( soul->s.frame == soul->mSoulLastFrame ) { soul->s.frame = soul->mSoulFirstFrame; }
	else if ( soul->s.frame < soul->mSoulLastFrame && soul->s.frame >= soul->mSoulFirstFrame ) { soul->s.frame++; }

	soul->nextthink = level.time + FRAMETIME;
}

void SP_LostMonsterSoul ( edict_t *self ) {

	int i, savedEntNumber;
	mmove_t	*move;
	edict_t	*soul;

	i = GetIndexByMonster( self->classname );

	if ( i < 0 ) {
		gi.dprintf( "%s has no soul\n", self->classname );
		return;
	}

	soul = G_Spawn();
	savedEntNumber		= soul->s.number;

	move = self->monsterinfo.currentmove;

	//prevent memory overload, and ensure there isn't stuff there already
	memset (soul, 0, sizeof(*soul));

	soul->inuse			= true;
	soul->gravity		= self->gravity;
	soul->s.number		= savedEntNumber;

	soul->groundentity = self->groundentity;
	VectorCopy ( self->s.origin, soul->s.origin );
	VectorCopy ( soul->s.origin, soul->s.old_origin );
	
	soul->takedamage	= DAMAGE_NO;
	soul->mass			= ( self->mass / 100 );				// strength of soul

	soul->classname = hosts[i].soul_name;

	VectorCopy ( self->mins, soul->mins );
	VectorCopy ( self->maxs, soul->maxs );
	soul->s.angles[YAW] = self->s.angles[YAW];
	VectorClear (soul->velocity);

	soul->s.modelindex	= self->s.modelindex;				// will use the skin specified model
	soul->s.skinnum		= self->s.skinnum;

	soul->soulSpawnTime = level.time + 5.0;
	soul->s.renderfx	= RF_TRANSLUCENT;		

	soul->s.frame			= self->s.frame;				// idle animation = repeating the last action of the body
	soul->mSoulFirstFrame	= move->firstframe;
	soul->mSoulLastFrame	= move->lastframe;

	soul->solid			= SOLID_NOT;
	soul->clipmask		= self->clipmask;
	soul->flags		   |= FL_NO_KNOCKBACK;					// set additional flags in monster_soul_think
	soul->svflags		= SVF_SOUL;							// allow server-side per-client visiblity check
	soul->movetype		= MOVETYPE_TOSS;

	soul->possessed		= false;

	//set in monster_soul_think
	soul->touch = NULL;
	// never dies, only gets freed

	soul->think = monster_soul_think;
	soul->nextthink = level.time + FRAMETIME;

//	gi.dprintf( "SOUL SPAWNED\n" );

	gi.linkentity ( soul );
}

//*****************
// BEGIN GHUD INFO
//*****************




/*
===============
SetMonsterNames

Called by worldspawn
===============
*/
void SetMonsterNames (void)
{
	int		i;
	unsigned long int uniqueNames;
	host_t	*monster;

	// dont add the same "Soldier Soul" string three times
	uniqueNames = (1<<SOLDIER_SS); 
	uniqueNames |= (1<<SOLDIER);

	// adding configstrings in the buffer area: (CS_ITEMS+MAX_ITEMS) - (game.num_items)
	// at the end of the items list but before the CS_PLAYERSKINS indexes
	for ( i = 0, monster = hosts; monster->host_class; monster++ ) {

		if ( CS_ITEMS+game.num_items+i >= CS_PLAYERSKINS )
			break;

		if ( uniqueNames & (1<<(monster-hosts)) )
			continue;
		
		uniqueNames |= (1<<(monster-hosts));
		gi.configstring( CS_ITEMS+game.num_items+i, monster->soul_name );
		i++;
	}
}

//***************
// END GHUD INFO
//***************

trace_t GhostMuzzleTrace ( edict_t *ent ) {

	trace_t tr;
	vec3_t forward;	
	vec3_t fromPos, toPos, targ;
	edict_t *ignore;

	//copy the player's muzzle yaw, pitch, roll angles into the forward looking direction
	AngleVectors( ent->client->v_angle, forward, NULL, NULL );
	VectorNormalize( forward );

	//a really long trace along the view direction from the player origin
	VectorCopy (ent->s.origin, fromPos);
	fromPos[2] += ent->viewheight;

	VectorMA( fromPos, 4096.0f, forward, toPos );

	tr = gi.trace( fromPos, vec3_origin, vec3_origin, toPos, ent, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);
			
	// continue the trace if this hits ent's host, a host_target, or ent's player_husk
	while ( tr.fraction < 1.0f && tr.ent  
		&& ( !Q_strcasecmp( tr.ent->classname, "host_target" )
		||   ( ent->client->host			&& tr.ent == ent->client->host )
		||   ( ent->client->player_husk		&& tr.ent == ent->client->player_husk ) ) ) {

		VectorCopy( tr.endpos, fromPos );
		ignore = tr.ent;

		tr = gi.trace( fromPos, vec3_origin, vec3_origin, toPos, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_SLIME|CONTENTS_LAVA|CONTENTS_WINDOW);
	}

	return tr;
}

void UpdateSoulShield ( edict_t *self ) {

	int i, lasttofree;
	float raise;
	vec3_t	origin;
	edict_t	*orbitSoul;
	gclient_t *client;

	client = self->client;

	if ( self->client->player_husk && !Q_strcasecmp( self->client->player_husk->classname, "player_husk" ) ) {
		VectorCopy ( self->client->player_husk->s.origin, origin );
		raise = (float)self->client->player_husk->viewheight/3.0f;
	}
	else {
		VectorCopy ( self->s.origin, origin );
		raise = (float)self->viewheight/3.0f;
	}

	// update all orbiting souls
	lasttofree = -1;
	for ( i = 0; i < client->numOrbitingSouls; i++ ) {
			
		if ( client->soul_shield[i] >= globals.num_edicts )
			continue;

		orbitSoul = &g_edicts[ client->soul_shield[i] ];

		if ( !orbitSoul || !orbitSoul->inuse || Q_strcasecmp( orbitSoul->classname, "orbit_soul" ) )
			continue;

		lasttofree = client->soul_shield[i];

		orbitSoul->angle = anglemod( orbitSoul->angle + (float)ORBIT_SPEED/(float)ORBIT_RADIUS );
		orbitSoul->s.origin[0] = origin[0] + ORBIT_RADIUS*cos(orbitSoul->angle); 
		orbitSoul->s.origin[1] = origin[1] + ORBIT_RADIUS*sin(orbitSoul->angle); 
		orbitSoul->s.origin[2] = origin[2] + raise;

		gi.linkentity( orbitSoul );
	}

	if ( lasttofree > 0 && client->damageAbsorbed && level.time >= client->orbitTime ) {

		G_FreeEdict( &g_edicts[ lasttofree ] );
		client->numOrbitingSouls--; 
		client->damageAbsorbed = 0;
		client->orbitTime = level.time + ORBIT_INTERVAL;
		gi.sound( self, CHAN_VOICE, gi.soundindex("husk/soulshieldhurt.wav"), 1, ATTN_STATIC, 0 );
	}
}

void LevelUpSoulCollector ( edict_t *ent ) {

	gclient_t *client = ent->client;
/*
	Additional:
	soul walk duration
	possession duration
	husk transfer damage ( to player )

	// Ghost abilities			// level available
	DRAIN_LIFE					1						// toggle
	TARGETED_POSSESSION			3						// passive
	RADIAL_POSSESSION			4						// passive
	TOUCH_POSSESSION			2						// passive
	DETECT_LIFE					3						// toggle ( too many may get annoying )
	GHOST_FLY					4						// toggle
	PULL_SOULS					3						// passive
	PUSH_BEASTS					4						// passive ( cmd )

	// Host abilities
	UBERHOST					3						// toggle
	OBLITERATE_HOST				4						// passive
	RECRUIT_FOLLOWERS			4						// passive
	TRANSFORM_HOST				5						// passive

	// Husk abilities
	DAMAGE_HOST					4						// passive
	SOUL_SHIELD					4						// passive
	WARP_HUSK					5						// passive

*/
	client->soul_abilities = 0;

	// separated to allow for level skip cheats
	switch ( client->soul_collector_level ) {
		case 1: {
			client->soul_abilities |= DRAIN_LIFE;
			break;
		}
		case 2: {
			client->soul_abilities |= DRAIN_LIFE;
			client->soul_abilities |= TOUCH_POSSESSION;
			break; 
		}
		case 3: {
			client->soul_abilities |= DRAIN_LIFE;
			client->soul_abilities |= TOUCH_POSSESSION;
			client->soul_abilities |= (TARGETED_POSSESSION|DETECT_LIFE|PULL_SOULS|UBERHOST);
			break; 
		}
		case 4: {
			client->soul_abilities |= DRAIN_LIFE;
			client->soul_abilities |= TOUCH_POSSESSION;
			client->soul_abilities |= (TARGETED_POSSESSION|DETECT_LIFE|PULL_SOULS|UBERHOST);
			client->soul_abilities |= (RADIAL_POSSESSION|GHOST_FLY|PUSH_BEASTS|OBLITERATE_HOST|RECRUIT_FOLLOWERS|DAMAGE_HOST|SOUL_SHIELD);
			break; 
		}
		case 5: {
			client->soul_abilities |= DRAIN_LIFE;
			client->soul_abilities |= TOUCH_POSSESSION;
			client->soul_abilities |= (TARGETED_POSSESSION|DETECT_LIFE|PULL_SOULS|UBERHOST);
			client->soul_abilities |= (RADIAL_POSSESSION|GHOST_FLY|PUSH_BEASTS|OBLITERATE_HOST|RECRUIT_FOLLOWERS|DAMAGE_HOST|SOUL_SHIELD);
			client->soul_abilities |= (TRANSFORM_HOST|WARP_HUSK);
			break; 
		}
	}

	// display the soul collector level in the hud popup instead of a centerprint
	//gi.centerprintf( ent, "SOUL COLLECTOR LEVEL = %i\n", ent->client->soul_collector_level );
	gi.sound (ent, CHAN_VOICE, gi.soundindex ("husk/levelup.wav"), 1, ATTN_NORM, 0);

	// force-display hud element of new abilities
	client->newSoulLevel = true;
	Cmd_PutAway_f( ent );
	client->showabilities = true;
	SoulAbilities( ent );
}

void detect_life ( edict_t *self ) {

	vec3_t	life_point;
	edict_t *other;

	//draw a line to  all monsters in range
	other = NULL;
	while ( ( other = findradius( other, self->s.origin, LIFE_RANGE ) ) != NULL ) {

		if ( other == self )
		{ continue; }

		if ( !(other->svflags & SVF_MONSTER) || (other->client) )
		{ continue; }

		if ( other->deadflag != DEAD_NO )
		{ continue; }

		if ( !Q_strncasecmp( other->classname, "monster_", 8 ) ) { 
			
			VectorMA (other->absmin, 0.5, other->size, life_point);
			gi.WriteByte (svc_temp_entity);
			gi.WriteByte (TE_BFG_LASER);
			gi.WritePosition (self->s.origin);
			gi.WritePosition (life_point);
			//gi.multicast (self->s.origin, MULTICAST_PHS);
			gi.unicast( self, true );
		}
	}
}

void player_husk_touch ( edict_t *self, edict_t *other ) {

	int j;
	edict_t *oldEnemy;

	if ( !self || !self->client || !other ) { return; }

	if ( self->client->player_husk && self->client->player_husk == other && other->classname && !Q_strcasecmp( other->classname, "player_husk" ) )  {
			 
		//flash the screen for a less sudden transition
		self->client->bonus_alpha = 0.5;

		gi.centerprintf( self, "BYE BYE HUSK!\n" );
		self->client->host = NULL;
		self->client->hostmode = false;
		self->client->ghostmode = false;

		// allow repossession of old host
		if ( self->client->escape_host ) {
		
			if ( self->client->escape_host->owner && self->client->escape_host->owner == self )
				self->client->escape_host->owner = NULL;

			self->client->escape_host = NULL;
		}

		self->s.renderfx &= ~RF_TRANSLUCENT;
		self->flags &= ~FL_NOTARGET;
		self->takedamage = DAMAGE_AIM;

		// jump back to the husk spawn ( or warp to ghost location )
		if ( (self->client->soul_abilities & WARP_HUSK) && (self->client->buttons & BUTTON_SHIFT) ) {
			gi.sound (self, CHAN_VOICE, gi.soundindex ("husk/warphusk.wav"), 1, ATTN_STATIC, 0);
		} else {
			gi.sound (self, CHAN_VOICE, gi.soundindex ("husk/enterhusk.wav"), 1, ATTN_STATIC, 0);
			VectorCopy( other->s.origin, self->s.origin );
		}

		//transfer all husk enemies back to the player
		for ( j = 1; j <= globals.num_edicts; j++ ) {
			oldEnemy = &g_edicts[ j ]; 
			
			if ( !oldEnemy || !oldEnemy->inuse ) { continue; }

			if ( oldEnemy->enemy == other ) { 

				if ( oldEnemy->svflags & SVF_MONSTER ) {
					oldEnemy->monsterinfo.aiflags = 0;
				}
				oldEnemy->goalentity = 
				oldEnemy->movetarget = 
				oldEnemy->enemy		 = 
				oldEnemy->oldenemy	 = self;
			}
		}

		G_FreeEdict( other );

		// Force the sight_client back to the player 
		// to avoid an empty pointer mid-frame 
		// in the event of player-death ( which crashes the game )
		level.sight_client = self;		

		gi.linkentity( self );

		//self->client->ps.gunindex = gi.modelindex(self->client->pers.weapon->view_model);
	} 
}

//returns soul entities that have origins within a spherical area
edict_t *findSouls (edict_t *from, vec3_t org, float rad)
{
	vec3_t	eorg;
	int		j;

	if (!from)
		from = g_edicts;
	else
		from++;
	for ( ; from < &g_edicts[globals.num_edicts]; from++)
	{
		if (!from->inuse)
			continue;
		for (j=0 ; j<3 ; j++)
			eorg[j] = org[j] - (from->s.origin[j] + (from->mins[j] + from->maxs[j])*0.5);
		if (VectorLength(eorg) > rad)
			continue;
		return from;
	}

	return NULL;
}

void ghostmode_protocols ( edict_t *self ) {

	int			i, num;
	trace_t		tr;
	gclient_t	*client;
	edict_t		*other;
	edict_t		*touch[MAX_EDICTS];

	client = self->client;
	self->svflags |= SVF_SOUL;				// allow server-side per-client monster-soul visiblity check

	// allow repossession of old host
	if ( level.time >= client->nextPossessTime && client->escape_host ) {
		
		if ( client->escape_host->owner && client->escape_host->owner == self )
			client->escape_host->owner = NULL;

		client->escape_host = NULL;
	}

	//radial possession in ClientCommand
	if ( client->soul_abilities & TARGETED_POSSESSION ) {

		if ( client->latched_buttons & BUTTON_ATTACK ) {

			client->latched_buttons &= ~BUTTON_ATTACK;

			if ( level.time >= client->nextPossessTime ) {
			
				tr = GhostMuzzleTrace( self );

				if ( tr.fraction < 1.0f && tr.ent ) {

					if ( tr.ent->classname 
						&& !Q_strncasecmp( tr.ent->classname, "monster_", 8 ) 
						&& tr.ent->deadflag == DEAD_NO ) {

						TakeHost( self, tr.ent, HOST_TARGETED );
					} 
				} 
			} else  { gi.centerprintf (self, "POSSESSION RECHARGHING" ); }
		} 
	}

	// check for touch possession
	if ( !client->host && !(client->soul_abilities & DRAIN_LIFE) && client->soul_abilities & TOUCH_POSSESSION ) {
		
		other = NULL;
		while ( ( other = findradius( other, self->s.origin, GHOST_RANGE ) ) != NULL )	{

			if ( other->classname && !Q_strncasecmp( other->classname, "monster_", 8 ) && other->deadflag == DEAD_NO ) 
			{ 
				if ( level.time >= client->nextPossessTime ) { TakeHost( self, other, HOST_TOUCH ); }
				else { gi.centerprintf (self, "POSSESSION RECHARGHING" ); }
				
				break;
			}
		}
	}

	// damage all monsters in range on a continuous pulse cycle
	if ( client->soul_abilities & DRAIN_LIFE && level.time >= self->client->drainLifeTime ) {

		other = NULL;
		while ( ( other = findradius( other, self->s.origin, GHOST_RANGE ) ) != NULL )	{

			if ( other->classname && !Q_strncasecmp( other->classname, "monster_", 8 ) && other->deadflag == DEAD_NO ) {
				T_Damage( other, other, other, vec3_origin, self->s.origin, vec3_origin, 10, 0, DAMAGE_NO_PROTECTION, MOD_TELEFRAG );
			} 
		}
		self->client->drainLifeTime = level.time + 2.0f;
	}

	// set all souls in range to be pulled
	if ( self->client->soul_abilities & PULL_SOULS ) {

		other = NULL;
		while ( ( other = findSouls( other, self->s.origin, SOUL_RANGE ) ) != NULL )	{

			if ( !(other->svflags & SVF_SOUL) )
				continue;
			if ( !other->touch )
				continue;
			if ( other->owner )		// already being pulled
				continue;

			if ( other->classname && strstr( other->classname, "Soul" ) ) {
				other->owner = self;
				other->s.effects |= EF_IONRIPPER;
			}
		}
	}
}

void husk_think ( edict_t *husk ) {

	vec3_t point;
	int cont;
	gclient_t *client;

	if ( !husk || ( husk->classname && Q_strcasecmp( husk->classname, "player_husk" ) ) ) { return; } 

	if ( husk->owner && husk->owner->client && !husk->owner->deadflag ) { client = husk->owner->client; }
	else { return; }

	//check if the owner is within touching distance
	if ( !client->hostmode && level.time > husk->huskBeginSearchTime ) {

		VectorSubtract( husk->owner->s.origin, husk->s.origin, point );

		if ( VectorLength(point) < GHOST_RANGE ) { 	
			husk->owner->husktouch ( husk->owner, husk ); 
			return;
		}
	}

	//jump back to idle animation if outside it
	if ( husk->s.frame > FRAME_stand40 || husk->s.frame < FRAME_stand01) { husk->s.frame = FRAME_stand01; }

	//continue idle animation
	if ( husk->s.frame == FRAME_stand40 ) { husk->s.frame = FRAME_stand01; }
	else if ( husk->s.frame < FRAME_stand40 && husk->s.frame >= FRAME_stand01) { husk->s.frame++; }

	//match player pain animation (this may cause crashes if for some reason the player dies in ghostmode)
	if (  husk->owner->s.frame >= FRAME_pain101 && husk->owner->s.frame <= FRAME_pain304  ) { 
		husk->s.frame = husk->owner->s.frame;
	} 

	// get waterlevel and type ( drowning/burning player remotely )
	point[0] = husk->s.origin[0];
	point[1] = husk->s.origin[1];
	point[2] = husk->s.origin[2] + husk->mins[2] + 1;	
	cont = gi.pointcontents ( point );

	if ( cont & MASK_WATER )
	{
		husk->watertype = cont;
		husk->waterlevel = 1;
		
		point[2] += 26;
		cont = gi.pointcontents (point);

		if ( cont & MASK_WATER ) {
			husk->waterlevel = 2;
			point[2] += 22;
			cont = gi.pointcontents (point);

			if (cont & MASK_WATER) { husk->waterlevel = 3; }
		}
	} else {
		husk->watertype = 0;
		husk->waterlevel = 0;
	}

	husk->owner->waterlevel = husk->waterlevel;
	husk->owner->watertype = husk->watertype;

	//if the player uses a powerup in ghostmode reflect that visually in the husk
	husk->s.effects		= husk->owner->s.effects;
	husk->s.renderfx	= husk->owner->s.renderfx;

	husk->s.effects    &= ~EF_TRACKER;
	husk->s.effects    &= ~EF_TRACKERTRAIL;
	husk->s.renderfx   &= ~RF_TRANSLUCENT;

	husk->nextthink = level.time + FRAMETIME;

	gi.linkentity ( husk );
	gi.linkentity ( husk->owner );
}

void SP_ClientHusk ( edict_t *self ) {

	int savedEntNumber, j;
	edict_t	*husk, *other;

	if ( !self || !self->client ) { return; }

	self->client->player_husk = G_Spawn();
	husk				= self->client->player_husk;

	savedEntNumber		= husk->s.number;

	//prevent memory overload, and ensure there isn't stuff there already
	memset (husk, 0, sizeof(*husk));

	husk->inuse			= true;
	husk->gravity		= self->gravity;
	husk->s.number		= savedEntNumber;

	husk->groundentity = self->groundentity;
	VectorCopy ( self->s.origin, husk->s.origin );
	VectorCopy ( husk->s.origin, husk->s.old_origin );
	
	husk->takedamage	= DAMAGE_AIM;						//forwards all T_Damage calls to the player
	husk->health		= self->health;
	husk->deadflag		= self->deadflag;

	husk->viewheight	= self->viewheight;
	husk->classname		= "player_husk";
	husk->mass			= self->mass;

	VectorCopy ( self->mins, husk->mins );
	VectorCopy ( self->maxs, husk->maxs );
	husk->s.angles[YAW] = self->s.angles[YAW];
	VectorClear (husk->velocity);

//	gi.setmodel ( husk, "players/male/tris.md2" );
	husk->s.modelindex	= self->s.modelindex;				// will use the skin specified model
	husk->s.modelindex2 = self->s.modelindex2;				// custom gun model (change this to something else?)
	husk->s.modelindex3 = self->s.modelindex3;				// ctf
	husk->s.modelindex4 = self->s.modelindex4;				// derp
	//husk->s.skinnum		= self->s.skinnum;

	husk->s.renderfx	= self->s.renderfx;	
	husk->s.effects		= self->s.effects;	

	//transfer all player enemies to the husk
	for ( j = 1; j <= globals.num_edicts; j++ ) {
		other = &g_edicts[ j ];			

		if ( !other || !other->inuse ) { continue; }

		if ( other->enemy == self ) { 

			if ( other->svflags & SVF_MONSTER ) { other->monsterinfo.aiflags = 0; }

			other->goalentity	= 
			other->movetarget	= 
			other->enemy		= 
			other->oldenemy		= husk;
		}
	}

	husk->s.frame		= FRAME_stand01;					// idle animation
	husk->owner			= self;								// owner will not clip against its belongings ( no touch! )

	husk->solid			= self->solid;
	husk->clipmask		= self->clipmask;
	husk->flags		   |= FL_NO_KNOCKBACK;					// set additional flags in husk_think
	husk->svflags		= self->svflags;
	husk->movetype		= MOVETYPE_TOSS;

	husk->waterlevel	= self->waterlevel;
	husk->watertype		= self->watertype;
	husk->air_finished	= self->air_finished;
	husk->light_level	= self->light_level;
	husk->show_hostile	= level.time + 1;

	husk->possessed		= false;

	husk->huskBeginSearchTime = level.time + 3.0f;			// give a few moments to allow player to leave the husk

	// husk pain animation is handled in husk_think
	// touch is handled in player_husk_touch and husk_think
	// never dies, only gets freed, player dies
	// ghostly screen blend added in <p_view.c> SV_CalcBlend

	husk->think = husk_think;
	husk->nextthink = level.time + FRAMETIME;

	self->s.effects		= EF_TRACKER;
	if ( self->client->soul_collector_level >= 3 )
		self->s.effects	= EF_TRACKERTRAIL;

	self->s.renderfx	|= RF_TRANSLUCENT;					// make the player ghostlike (not tangible yet, need a proper chasecam)

	self->flags			|= FL_NOTARGET;						// This can be a AI_SightClient crash issue ( fixed )
	self->takedamage	= DAMAGE_NO;								

	//self->client->ps.gunindex = 0;							// doesn't prevent firing on its own

	//not a PlayerNoise, to avoid alerting monsters
	gi.sound ( husk, CHAN_VOICE, gi.soundindex ("husk/leavehusk.wav"), 1, ATTN_NORM, 0);

	gi.linkentity ( self );
	gi.linkentity ( husk );
}

// 'f' is bound to ghost mode toggle ( husk spawn ), and host obliteration
void Cmd_Ghost_f( edict_t *ent ) {

	if ( !( ent->client->hostmode ) ) {
		ent->client->ghostmode = !ent->client->ghostmode;

	} else if ( ent->client->hostmode && ent->client->soul_abilities & OBLITERATE_HOST ) { DropHost( ent, HOST_KILL ); }		
		
	// clear out the husk
	if ( !(ent->client->ghostmode || ent->client->hostmode) && ent->client->player_husk 
		&& ent->client->player_husk->classname && !Q_strcasecmp( ent->client->player_husk->classname, "player_husk" ) 
		&& ent->husktouch ) {
		ent->husktouch( ent, ent->client->player_husk );
	}

	if ( ent->client->ghostmode && ( !ent->client->player_husk 
		|| ( ent->client->player_husk->classname && Q_strcasecmp( ent->client->player_husk->classname, "player_husk" ) ) ) ) 
	{ SP_ClientHusk ( ent ); }

	gi.cprintf (ent, PRINT_HIGH, "GHOST = %s\n", ent->client->ghostmode ? "TRUE" : "FALSE" );
}

// 'r' is bound to radial host-take and host drop
void Cmd_Inhabit_f( edict_t *ent ) {

	edict_t *other;

	if ( ent->client->ghostmode ) { 

		if ( ent->client->soul_abilities & RADIAL_POSSESSION ) { 

			if ( level.time >= ent->client->nextPossessTime ) {

				//find the first monster in range
				other = NULL;
				while ( ( other = findradius( other, ent->s.origin, SOUL_RANGE ) ) != NULL ) {

					if ( other == ent )
					{ continue; }

					if ( !(other->svflags & SVF_MONSTER) || (other->client) )
					{ continue; }

					if ( !Q_strncasecmp( other->classname, "monster_", 8 ) && other->deadflag == DEAD_NO ) { break; }
				}

				if ( other ) { TakeHost( ent, other, HOST_RADIAL ); } 
				else { gi.centerprintf ( ent, "NO HOSTS IN RANGE\n" ); }

			} else { gi.centerprintf ( ent, "POSSESSION RECHARGHING" );  }
		}
	} else if ( ent->client->hostmode ) { DropHost( ent, HOST_NO_HARM ); }

	gi.cprintf (ent, PRINT_HIGH, "GHOST = %s\n", ent->client->ghostmode ? "TRUE" : "FALSE" );
}

// 'n' is bound to "detect_life" toggle
void Cmd_Detect_Life_f( edict_t *ent ) {
		
	char	*msg;

	if ( ent->client->soul_collector_level < 3 )
		return;

	ent->client->soul_abilities ^= DETECT_LIFE;
	if (!(ent->client->soul_abilities & DETECT_LIFE) )
		msg = "Detect Life OFF\n";
	else
		msg = "Detect Life ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}

// 'mouse3' is bound to radial monster push, and host-speak
void Cmd_Push_Beasts_f( edict_t *ent ) {

	int num, pushCount;
	vec3_t dir;
	trace_t tr;
	edict_t *other;

	if ( !ent->client->hostmode && ent->client->soul_abilities & PUSH_BEASTS ) {

		if ( level.time >= ent->client->nextPossessTime ) {

			//push all monsters in range
			pushCount = 0;
			other = NULL;
			while ( ( other = findradius( other, ent->s.origin, SOUL_RANGE ) ) != NULL ) {

				if ( other == ent )
				{ continue; }

				if ( !(other->svflags & SVF_MONSTER) || (other->client) )
				{ continue; }

				if ( other->deadflag != DEAD_NO )
				{ continue; }

				if ( !Q_strncasecmp( other->classname, "monster_", 8 ) ) {
					VectorSubtract( other->s.origin, ent->s.origin, dir );
					VectorNormalize( dir );
					T_Damage ( other, ent, ent, dir, ent->s.origin, vec3_origin, 5, 1000, DAMAGE_NO_PROTECTION, MOD_FALLING);				
					pushCount++;
				}
			}

			if ( !pushCount ) { gi.centerprintf ( ent, "NO BEASTS IN RANGE\n" ); }
			else { 
				ent->client->nextPossessTime = level.time + 3.0f;
				gi.sound ( ent, CHAN_VOICE, gi.soundindex( "husk/pushbeasts.wav" ), 1, ATTN_NORM, 0); 
			}

		} else { gi.centerprintf ( ent, "RECHARGHING" );  }

	} else if ( ent->client->hostmode && ent->client->soul_abilities & UBERHOST ) {

		// hostspeak 2of2
		HostSpeak( ent, MOUSE_THREE );
	}
}

void Cmd_Set_Soul_Level_f( edict_t *ent ) {
	
	int		desired_level;

	desired_level = atoi (gi.argv(1));

	switch ( desired_level ) {
		case 1: { ent->client->soul_collector_level = 1; LevelUpSoulCollector( ent ); break; }
		case 2: { ent->client->soul_collector_level = 2; LevelUpSoulCollector( ent ); break; }
		case 3: { ent->client->soul_collector_level = 3; LevelUpSoulCollector( ent ); break; }
		case 4: { ent->client->soul_collector_level = 4; LevelUpSoulCollector( ent ); break; }
		case 5: { ent->client->soul_collector_level = 5; LevelUpSoulCollector( ent ); break; }
		default:{ 
			gi.cprintf( ent, PRINT_HIGH, "Set Your Soul Collector Level. USAGE: soullevel [1-5] EXAMPLE: soullevel 3\n" );
			gi.cprintf( ent, PRINT_HIGH, "CURRENT SOUL COLLECTOR LEVEL = %i\n", ent->client->soul_collector_level ); 
			break; 
		}
	}
}

// 'v' is bound to "uberhost" toggle and "drain_life" toggle
void Cmd_Uber_f (edict_t *ent) {

	char	*msg;
	hmove_t *perform_move;

	if ( ent->client->hostmode ) {

		if ( ent->client->soul_collector_level < 3 )
			return;

		ent->client->soul_abilities ^= UBERHOST;
		if (!(ent->client->soul_abilities & UBERHOST) )
			msg = "uberhost OFF\n";
		else
			msg = "uberhost ON\n";

		if ( ent->client->host ) {

			if ( ent->client->host->host_target ) { G_FreeEdict( ent->client->host->host_target ); }

			// prevent mid-fire crash
			perform_move = find_host_move ( ent->client->host, "stand1" ); // or try "stand2"
			if ( perform_move && perform_move->hmove ) { perform_move->hmove( ent->client->host ); }
		}

		gi.cprintf (ent, PRINT_HIGH, msg);

	} else if ( ent->client->ghostmode ) {

		ent->client->soul_abilities ^= DRAIN_LIFE;
		if (!(ent->client->soul_abilities & DRAIN_LIFE) )
			msg = "Drain Life OFF\n";
		else
			msg = "Drain Life ON\n";

		gi.cprintf (ent, PRINT_HIGH, msg);
	}
}

void SP_OrbitSoul( edict_t *ent, int index ) {

	vec3_t		os_mins		= {-16, -16, -24};
	vec3_t		os_maxs		= {16, 16, 32};
	edict_t	*orbitSoul;
	int savedEntNumber;

	orbitSoul = G_Spawn();
	savedEntNumber = orbitSoul->s.number;
	memset( orbitSoul, 0, sizeof(orbitSoul) );

	orbitSoul->inuse = true;
	orbitSoul->classname = "orbit_soul";
	orbitSoul->s.number = savedEntNumber;

	orbitSoul->groundentity = NULL;
	orbitSoul->gravity = ent->gravity;

	VectorCopy ( os_mins, orbitSoul->mins );
	VectorCopy ( os_maxs, orbitSoul->maxs );
	VectorCopy ( ent->s.origin, orbitSoul->s.origin );
	orbitSoul->s.modelindex = gi.modelindex ( "models/items/ammo/grenades/medium/tris.md2" );
	orbitSoul->s.effects	= (EF_COLOR_SHELL|EF_ROTATE|EF_GRENADE);
	orbitSoul->s.renderfx	= (RF_TRANSLUCENT|RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);

	orbitSoul->solid		= SOLID_NOT;
	orbitSoul->clipmask		= ent->clipmask;
	orbitSoul->flags	   |= FL_NO_KNOCKBACK;
	orbitSoul->svflags		= SVF_SOUL;				// allow server-side per-client visiblity check
	orbitSoul->movetype		= MOVETYPE_TOSS;

	gi.linkentity( orbitSoul );
	ent->client->soul_shield[index] = savedEntNumber;
}

qboolean UniqueOrbitSoul( edict_t *ent, int ignore, int edictNumber ) {

	int i;
	qboolean unique;

	unique = true;

	for( i = 0; i < MAX_ORBITS; i++ ) {
		
		if ( i == ignore )
			continue;

		if ( ent->client->soul_shield[i] == edictNumber ) {
			unique = false;
			break;
		}
	}

	return unique;
}

// 'y' is bound to shield of souls refresh
void Cmd_Soul_Shield_f( edict_t *ent ) {
	
	int i,orbitSoulNumber;
	float yaw;
	edict_t *orbitSoul;

	// passive
	if ( !(ent->client->soul_abilities & SOUL_SHIELD ) )
		return;
	
	if ( ent->client->numOrbitingSouls == MAX_ORBITS ) { 
		gi.centerprintf( ent, "SHIELD OF SOULS AT MAX\n" ); 
		return;
	}

	if ( ent->client->pool_of_souls >= 10 ) {

		// re-initialize the angles and respawn missing orbitSouls
		for ( yaw = 0, i = 0; i < MAX_ORBITS; i++, yaw += (360/MAX_ORBITS) ) {
		
			// get an entity number
			if ( !ent->client->soul_shield[i]  
				|| ent->client->soul_shield[i] >= globals.max_edicts 
				|| !UniqueOrbitSoul(ent, i, ent->client->soul_shield[i]) )
				SP_OrbitSoul( ent, i );

			orbitSoul = &g_edicts[ ent->client->soul_shield[i] ];

			// make sure its still an orbit_soul
			if ( !orbitSoul || !orbitSoul->inuse || Q_strcasecmp( orbitSoul->classname, "orbit_soul" ) )
				SP_OrbitSoul( ent, i );

			orbitSoul = &g_edicts[ ent->client->soul_shield[i] ];
			
			orbitSoul->angle = anglemod( yaw*M_PI*2 / 360);
			gi.linkentity ( orbitSoul );
		}

		ent->client->soulChange = true;
		ent->client->pool_of_souls -= 10;
		ent->client->numOrbitingSouls = MAX_ORBITS;
		gi.sound( ent, CHAN_VOICE, gi.soundindex("husk/soulshield.wav"), 1, ATTN_STATIC, 0 );

	} else { gi.centerprintf( ent, "NOT ENOUGH SOULS" ); }
}

// 'o' is bound to host transformation/upgrade
void Cmd_Transform_Host_f( edict_t *ent ) {

	int i, num, oldIndex, uniqueAttempts;
	unsigned long int attemptFlags;
	int oldRank, desiredRank;

	qboolean transformOk;
	char *newClassname;
	int lift, max_lift;
	vec3_t neworg;

	vec3_t	mins, maxs, start;
	int		x, y;

	host_t *trans;
	trace_t	trace;
	edict_t *host;

	int soundCount = 0;
	
	// passive
	if ( !(ent->client->soul_abilities & TRANSFORM_HOST) )
		return;

	host = ent->client->host;

	if ( !host )
		return;

	for ( trans = hosts; trans->host_class; trans++ ) {

		if ( !strcmp( trans->host_class, host->classname ) ) {
			// found it
			oldRank = trans->host_rank;
			oldIndex = trans-hosts;
			break;
		}
	}

	transformOk = false;
	desiredRank = oldRank;

	// don't transform into the same monster
	attemptFlags = (1<<oldIndex);
	uniqueAttempts = 0;

	// enough to upgrade and desire to upgrade?
	if ( ent->client->buttons & BUTTON_ALT ) 
		desiredRank++; 

	if ( desiredRank == 1 && ent->client->pool_of_souls ) { transformOk = true; }
	else if ( desiredRank == 2 && ent->client->pool_of_souls >= 10 ) { transformOk = true; }
	else if ( desiredRank == 3 && ent->client->pool_of_souls >= 20 ) { transformOk = true; }
	else if ( desiredRank == 4 && ent->client->pool_of_souls >= 30 ) { transformOk = true; }
	else if ( desiredRank == 5 && ent->client->pool_of_souls >= 40 ) { transformOk = true; }

	if ( transformOk ) {

		newClassname = NULL;
		while ( !newClassname ) {

newTransformAttempt:
			if ( uniqueAttempts == 21 ) { // one index is already used by host's current form
				gi.centerprintf( ent, "NO ROOM TO TRANSFORM\n" );
				return;
			}

			num = rand()%22;	// check indexes 0 through 21 ( 22 possible monster types )

			if ( attemptFlags & (1<<num) )
				continue;

			attemptFlags |= (1<<num);
			uniqueAttempts++;

			//gi.dprintf( "ATTEMPT INDEX = %i\n", num );

			trans = &hosts[num];

			if ( trans->host_rank == desiredRank ) {

				max_lift = 2*( trans->host_maxs[2] - trans->host_mins[2] );
				VectorCopy( host->s.origin, neworg );
			//	gi.dprintf( "ATTEMPT MINS = %s\nATTEMPT MAXS = %s\nMAX_LIFT = %i\n\n", vtos(trans->host_mins), vtos(trans->host_maxs), max_lift);

				for( lift = 0; lift <= max_lift; lift++ ) {

					neworg[2] += lift;
					// make sure there's room to transform

					VectorAdd (neworg, trans->host_mins, mins);
					VectorAdd (neworg, trans->host_maxs, maxs);

					// if any of the points on top corners are solid world, try a different monster
					start[2] = maxs[2] + 1;
					for	(x=0 ; x<=1 ; x++) 
					{
						for	(y=0 ; y<=1 ; y++) 
						{
							start[0] = x ? maxs[0] : mins[0];
							start[1] = y ? maxs[1] : mins[1];
							if (gi.pointcontents (start) == CONTENTS_SOLID)
								goto newTransformAttempt;
						}
					}

					trace = gi.trace (neworg, trans->host_mins, trans->host_maxs, neworg, host, MASK_MONSTERSOLID);
			
					if ( !trace.startsolid && trace.fraction == 1 ) {

						newClassname = trans->host_class;
						VectorCopy( neworg, host->s.origin );

						if (lift)
							host->groundentity = NULL;

						gi.linkentity( host );
						break;
					}
				}
			} 
		}

	} else { 
		gi.centerprintf( ent, "NOT ENOUGH SOULS\n" ); 
		return;
	}

	// it costs souls to do
	ent->client->soulChange = true;

	if( desiredRank > 1 )
		ent->client->pool_of_souls -= 10;
	else
		ent->client->pool_of_souls--;

	ED_CallTransformSpawn ( host, newClassname );
	if ( host->think ) {
		host->nextthink = level.time;
		host->think( host );
	}
	gi.sound (ent, CHAN_AUTO, gi.soundindex( "slighost/hosttransform.wav" ), 1, ATTN_NORM, 0);
//	for ( i = 0; i < 5; i++)
//			ThrowGib (host, "models/objects/gibs/sm_meat/tris.md2", 100, GIB_ORGANIC);

	TakeHost( ent, host, HOST_TRANSFORM );
}

/*
collection layout
      souls captured
### strogg    ### strogg
--- ------    --- ------
003 Berserker 003 Gladiator
003 Gunner    003 Infantry
003 Soldier   003 Tank
003 Commander 003 Medic
003 Flipper   003 Maiden
003 Parasite  003 Flyer
003 Brain     003 Floater
003 Hover     003 Mutant
003 Supertank 003 Hornet
003 Makron    003 Jorg
*/

void SoulCollection( edict_t *ent )
{
	char	string[1024];

	char *soul_name, *amt;
	char display_name[30];
	char display_amt[10];
	int index, x, y;
	char xv[10];
	char yv[10];

	memset( string, 0, sizeof(string) );
	strcat( string,  "xv 32 yv 8 picn inventory " );
	strcat( string, "xv 56 yv 32 string2 \"      souls captured\" " );
	strcat( string, "xv 56 yv 40 string2 \"### strogg    ### strogg\" " );
	strcat( string, "xv 56 yv 48 string2 \"--- ------    --- ------\" " );

	// starting soul text position - 8
	y = 48;

	// each loop produces an additional:
	// "xv 50 yv 172 string2 \"%3i %s\" "
	for( index = 0; index < UNIQUE_SOUL_TYPES; index++ ) {

		if ( index%2 == 1 ) {
			x = 168;
		} else {
			x = 56;
			y += 8;
		}		
		
		if ( !(ent->client->soulCollection[ index ]) )
			continue;

		// position
		memset( xv, 0, sizeof(xv) );
		strcat( xv, va( "%i", x ) );
		
		memset( yv, 0, sizeof(yv) );
		strcat( yv, va( "%i", y ) );

		// value
		memset( display_amt, 0, sizeof(display_amt) );
		amt = va( "%.3i", ent->client->soulCollection[ index ] );
		strcat( display_amt, amt );
		
		// name
		memset( display_name, 0, sizeof(display_name) );
		soul_name = GetCollectionHudName( index );	
		strncpy( display_name, soul_name, strlen(soul_name)-5 );			// dont use " Soul"

		strcat( string, "xv " );
		strcat( string, xv );
		strcat( string, " yv " );
		strcat( string, yv );
		strcat( string, " string2 " );
		strcat( string, "\"" );
		strcat( string, display_amt );
		strcat( string, " " );
		strcat( string, display_name );
		strcat( string, "\" " );

		// debug print verify
		//gi.dprintf( "x = %i\ny = %i\nxv = %s\nyv = %s\n", x, y, xv, yv );
	}

	// debug verify
	//gi.dprintf( string );

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}

/* 27 vertical columns available
ability layout
rank-up ( ghost )          header1
soul collector rank: #     header2
usekey ability			   1
------ -------			   2
shoot targeted possession  3 ( rodeo chase target / shoot ) // hostmode only, ( mouse3 push beasts ) // normal mode only
    r radial possession    4 ( release host )
	f warp body to ghost   5 ( obliterate host, spawn ghost )
	v drain life toggle    6 ( uberhost/rodeo toggle )
	n detect life	 	   7 // all modes
	m ghost fly	 		   8 
	y shield of souls	   9 // all modes
    ---touch---			   10
proximity possession	   11
proximity drain life	   12
    ---passive---		   13	
pull nearby souls		   14 
ghost-walk time: ###/###   15 ( damage transfer to host )
						   16 ( o transform host )
						   17 ( recruit/order followers )...
*/

/*
	Additional:
	soul walk duration
	possession duration
	husk transfer damage ( to player )

	// Ghost abilities			// level available
	DRAIN_LIFE					1						// toggle
	TARGETED_POSSESSION			3						// passive
	RADIAL_POSSESSION			4						// passive
	TOUCH_POSSESSION			2						// passive
	DETECT_LIFE					3						// toggle ( too many may get annoying )
	GHOST_FLY					4						// toggle
	PULL_SOULS					3						// passive
	PUSH_BEASTS					4						// passive ( cmd )

	// Host abilities
	UBERHOST					3						// toggle
	OBLITERATE_HOST				4						// passive
	RECRUIT_FOLLOWERS			4						// passive
	TRANSFORM_HOST				5						// passive

	// Husk abilities
	DAMAGE_HOST					4						// passive
	SOUL_SHIELD					4						// passive
	WARP_HUSK					5						// passive

*/

// produce the rank and mode appropriate readout
// extremely ugly, I know...
// this would be better as a svc_inventory
// with a new function in <cl_inven.c>
void SoulAbilities( edict_t *ent )
{
	char	string[1024];

	memset( string, 0, sizeof(string) );
										  strcat( string,  "xv 32 yv 8 picn inventory "	   );

	if ( ent->client->newSoulLevel )	{ strcat( string, "xv 56 yv 32 string2 \"rank-up " );	}
	else								{ strcat( string, "xv 56 yv 32 string2 \"        " );	}

	if ( ent->client->ghostmode )		{ strcat( string,                 "---ghost---\" " );	}
	else if ( ent->client->hostmode )	{ strcat( string,                  "---host---\" " );	}
	else								{ strcat( string,             "---corporeal---\" " );	}

										  strcat( string, "xv 56 yv 40 string2 \"soul collector rank: " );
										  strcat( string, va( "%i", ent->client->soul_collector_level ) );
										  strcat( string, "\" " );
										  strcat( string, "xv 56 yv 48 string2 \"usekey ability\" " );
										  strcat( string, "xv 56 yv 56 string2 \"------ -------\" " );

	if ( ent->client->ghostmode ) {
		// 14 lines
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 64 string2 \"   fire targeted possession\" " ); }
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 72 string2 \"      r radial possession\" "   ); }
		if ( ent->client->soul_collector_level >= 5 ) { strcat( string, "xv 56 yv 80 string2 \"shift+f warp body to ghost\" "  ); } 
														strcat( string, "xv 56 yv 88 string2 \"      f return to body\" "	   );	
														strcat( string, "xv 56 yv 96 string2 \"      v drain life toggle\" "   );
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 104 string2 \"      n detect life toggle\" " ); }
		if ( ent->client->soul_collector_level >= 4 ) { strcat( string, "xv 56 yv 112 string2 \"      m ghost fly\" "		   );
														strcat( string, "xv 56 yv 120 string2 \"      y shield of souls\" "	   ); }
														strcat( string, "xv 56 yv 128 string2 \"    ---touch---\" "			   );
		if ( ent->client->soul_collector_level >= 2 ) { strcat( string, "xv 56 yv 136 string2 \"proximity possession\" "	   ); }
														strcat( string, "xv 56 yv 144 string2 \"proximity drain life\" "	   );
														strcat( string, "xv 56 yv 152 string2 \"    ---passive---\" "		   );
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 160 string2 \"pull nearby souls\" "		   ); }
		//												strcat( string, "xv 56 yv 168 string2 \"ghost-walk time: ###/###\" " ); // CHANGE 

	} else if ( ent->client->hostmode ) {
		// 9 or 13 lines
		if ( ent->client->soul_abilities & UBERHOST ) { 
														strcat( string, "xv 56 yv 64 string2 \"  fire attack\" "			    );
														// appears at bottom
														strcat( string, "xv 56 yv 144 string2 \"    ---host speak---\" "	    );
														strcat( string, "xv 56 yv 152 string2 \"shift+fire follower control\" " );
														strcat( string, "xv 56 yv 160 string2 \"shift+mouse3 free follower\" "  );
														strcat( string, "xv 56 yv 168 string2 \"  alt+mouse3 shout an alert\" " );
														strcat( string, "xv 56 yv 172 string2 \"      mouse3 call for help\" "  );
		} else {										strcat( string, "xv 56 yv 64 string2 \"  fire set chase target\" "      ); }

														strcat( string, "xv 56 yv 72 string2 \"     r release host\" "		); 
		if ( ent->client->soul_collector_level >= 4 ) { strcat( string, "xv 56 yv 80 string2 \"     f obliterate host\" "	); }
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 88 string2 \"     v uberhost toggle\" "	); }
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 96 string2 \"     n detect life toggle\" "); }
		if ( ent->client->soul_collector_level >= 5 ) { strcat( string, "xv 56 yv 104 string2 \"     o transform host\" "	); 
														strcat( string, "xv 56 yv 112 string2 \" alt+o upgrade host\" "	    ); }
		if ( ent->client->soul_collector_level >= 4 ) { strcat( string, "xv 56 yv 120 string2 \"     y shield of souls\" "	); }
														strcat( string, "xv 56 yv 128 string2 \"    ---passive---\" "		);
		if ( ent->client->soul_collector_level >= 4 ) { strcat( string, "xv 56 yv 136 string2 \"damage transfer to host\" "	); }

	} else { // normal mode
		// 4 lines
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 64 string2 \"mouse3 push beasts\" "		 ); }
														strcat( string, "xv 56 yv 72 string2 \"     f ghost-walk\" "		 );
		if ( ent->client->soul_collector_level >= 3 ) { strcat( string, "xv 56 yv 80 string2 \"     n detect life toggle\" " );	}
		if ( ent->client->soul_collector_level >= 4 ) { strcat( string, "xv 56 yv 88 string2 \"     y shield of souls\" "	 );	}
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}

// F7 is bound to "soul_abilities" hud toggle
void Cmd_Soul_Abilities_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showcollection = false;

	if ( ent->client->showabilities ) {
		ent->client->showabilities = false;
		return;
	}

	ent->client->showabilities = true;
	SoulAbilities( ent );
}

// F8 is bound to "soul_abilities" hud toggle
void Cmd_Soul_Collection_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showabilities = false;

	if ( ent->client->showcollection ) {
		ent->client->showcollection = false;
		return;
	}

	ent->client->showcollection = true;
	SoulCollection( ent );
}

//*************************
//	TMF7 END GHOST MODE
//*************************