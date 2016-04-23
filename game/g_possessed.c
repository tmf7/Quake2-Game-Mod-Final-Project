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

// all potential hosts according to <g_spawn.c>
host_t hosts[] = 
{
	{ "monster_berserk",		berserk,	NULL,					NULL					},
	{ "monster_gladiator",		gladiator,	NULL,					NULL					},

	{ "monster_gunner",			gunner,		NULL,					NULL					},
	{ "monster_infantry",		infantry,	NULL,					NULL					},

	{ "monster_soldier_light",	soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier",		soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier_ss",		soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},

	{ "monster_tank",			tank,		NULL,					NULL					},
	{ "monster_tank_commander",	tank,		NULL,					NULL					},

	{ "monster_medic",			medic,		NULL,					NULL					},
	{ "monster_flipper",		flipper,	NULL,					NULL					},
	{ "monster_chick",			chick,		NULL,					NULL					},
	{ "monster_parasite",		parasite,	NULL,					NULL					},
	{ "monster_flyer",			flyer,		NULL,					NULL					},

	{ "monster_brain",			brain,		NULL,					NULL					},
	{ "monster_floater",		floater,	NULL,					NULL					},
	{ "monster_hover",			hover,		NULL,					NULL					},
	{ "monster_mutant",			mutant,		NULL,					NULL					},	
	{ "monster_supertank",		supertank,	NULL,					NULL					},

	{ "monster_boss2",			boss2,		NULL,					NULL					},	
	{ "monster_boss3_stand",	boss3_stand,NULL,					NULL					},
	{ "monster_jorg",			jorg,		NULL,					NULL					},

//	{"monster_commander_body", SP_monster_commander_body}, <-----NOT ACTUALLY A MONSTER ( do a case check for this? )...unless it has GOOD animations

	{ NULL,						NULL		}
};

void set_host_target( edict_t *host, edict_t *other, vec3_t origin, qboolean show, int control_type );
void SP_Host_Target( edict_t *host, vec3_t origin, qboolean show);

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

	// how does the player handle swimming?
	// how do monsters handle fly/swim moves?

	// ai_run_slide performs a strafe ( normally only for FL_FLY monsters )

	// trouble with fly/swim ( or inadvertent fly/swim )?

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

void monster_think_possesed( edict_t *self, edict_t *host, const pmove_t *pm )
{ 
	char		*selected_move;
	float		yaw;
	trace_t		tr;
	
	if ( !self || !host ) { return; }

	// may need to verify the classname if the follwer dies and the address is filled with some other entity
	if ( self->client->host_follower 
		&& ( self->client->host_follower->deadflag != DEAD_NO || Q_strncasecmp( self->client->host_follower->classname, "monster_", 8 ) ) ) {

		DropFollower( self );
	}

	if ( host->deadflag != DEAD_NO ) { 
		
		DropHost( self, HOST_DEATH ); 
		return; 
	}

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
	if ( host->owner && host->owner->client && host->owner != self ) {
		gi.centerprintf ( self, "%s occupied by %s\n", host->classname, host->owner->client->pers.netname );
		return;
	}

	for ( h = hosts; h->host_name; h++ ) {

		if ( !strcmp( h->host_name, host->classname ) ) {

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

	host->possesed_think = monster_think_possesed;		// should this be left hanging when host is dropped? shouldn't matter
				
	//develop a proper chasecam
	SetChaseTarget( self, host );

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
	}

	if ( self->client->host->host_target )	{ G_FreeEdict( self->client->host->host_target ); }
	if ( self->client->host_follower )		{ DropFollower( self ); }

	//prevent NULL-enemy crash mid-fire in uberhost mode
	if ( self->client->host->deadflag == DEAD_NO ) {
		
		// NOTE: ensure the flying/swimming equivalent*****
		perform_move = find_host_move ( self->client->host, "stand1" ); // or try "stand2"
		if ( perform_move && perform_move->hmove ) { perform_move->hmove( self->client->host ); }
	}

	self->client->host->owner			= NULL;
	self->client->host->possessed		= false;
	self->client->host					= NULL;
	self->client->hostmode				= false;
	self->client->ghostmode				= true;
	self->client->nextPossessTime		= level.time + 3.0f;
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

	// check for flags & (FL_FLY|FL_SWIM) too**********************************************
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

		// check for flags & (FL_FLY|FL_SWIM) too**********************************************
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

			host->goalentity = host->movetarget = host->host_target;
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

	//do a check for FLY|SWIM and call a better ai_func********************************
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

//*******************
// TMF7 END GHOST MODE
//*******************