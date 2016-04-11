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

hmove_t soldier_li[] =
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
	{	"attack5",				soldier_attack6		},
	{	"duck",					soldier_duck		},
	{	NULL,					NULL				}
};

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
	{	"attack5",				soldier_attack6		},
	{	"duck",					soldier_duck		},
	{	NULL,					NULL				}
};

hmove_t soldier_ss[] =
{
	{	"stand1",				soldier_stand1		},
	{	"stand2",				soldier_stand3		},
	{	"walk1",				soldier_walk1		},
	{	"walk2",				soldier_walk2		},
	{	"run_start",			soldier_start_run	},
	{	"run",					soldier_runp		},
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

	{ "monster_soldier_light",	soldier_li,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier",		soldier,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},
	{ "monster_soldier_ss",		soldier_ss,	"soldier/solpain2.wav", "soldier/solpain2.wav"	},

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

void set_host_move ( edict_t *host, char *selected_move ) {

	hmove_t		*m;

	for ( m = host->hmove_list; m->move_name; m++ )
	{
		if ( !strcmp( m->move_name, selected_move ) )
		{	
			// found it, set the monsterinfo.currentmove
			m->hmove( host );	//don't resolve the ai_func or endfunc
			return;
		}
	}
	gi.dprintf ( "%s doesn't have that specific host move defined\n", host->classname );
}

void monster_think_possesed( edict_t *self, edict_t *host, const usercmd_t *cmd, const int *buttons )
{ 
	trace_t		tr;
	edict_t		*target;

	if ( !self || !host ) { return; }

	if ( host->host_target ) { target = host->host_target; }
	else { target = NULL; }	

	if ( host->deadflag != DEAD_NO ) { 
		
		DropHost( self, HOST_DEATH ); 
		return; 
	}

	if ( self->client->soul_abilities & UBERHOST && host->hmove_list != NULL ) { 

		// test-run
		// need to set the enemy to give it a point to fire at => trusty host_target
		// host fidgets in place if aifunc or endfunc cant resolve ( double check this )
		if ( *buttons & BUTTON_ATTACK ) { // works, host defaults to last move, host stops if target is too close ( as expected )

			tr = GhostMuzzleTrace( self );	
			set_host_target( host, &tr, false, UBER_ATTACK );
			set_host_move ( host, "attack3" ); 
		} 
		//else { set_host_move ( host, "stand1" ); }	//set every ClientThink => doesn't give time to resolve all frames
	}
	else {

		//RODEO HOST CONTROLS
		if ( *buttons & BUTTON_ATTACK ) {

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

///////////////how a player uses the pm info///////////////////////
	/*
	edict_t	*other;				//may include lasers, water?, buttons, other monsters, triggers, ***level swaps***
	int		i, j;
	pmove_t	mpm;

	hmove_t *m;
	char	*selected_move;

	level.current_entity = host;	//necessary???

	pm_passent = host;			//global? originally declared in <p_client.c>

	// set up for pmove (prevent overload crash)
	memset (&mpm, 0, sizeof(mpm));

	client->ps.pmove.pm_type = PM_NORMAL;			//set the host

	client->ps.pmove.gravity = sv_gravity->value;	//set the host
	mpm.s = client->ps.pmove;						//set the host

	for (i=0 ; i<3 ; i++)	//hosts originally always have zero velocity (only their origin changes)
	{
		mpm.s.origin[i] = host->s.origin[i]*8;
		mpm.s.velocity[i] = host->velocity[i]*8;
	}

	if (memcmp(&client->old_pmove, &mpm.s, sizeof(mpm.s)))		//set the host
	{
		mpm.snapinitial = true;
//		gi.dprintf ("pmove changed!\n");
	}

	mpm.cmd = *cmd;

	mpm.trace = PM_trace;	// adds default parms			//declare this in <g_monster.c>???
	mpm.pointcontents = gi.pointcontents;

	// perform a pmove
	gi.Pmove (&mpm);	

	// save results of pmove					//put this in edict_t????
	client->ps.pmove = pm.s;
	client->old_pmove = pm.s;

	for (i=0 ; i<3 ; i++)
	{
		host->s.origin[i] = mpm.s.origin[i]*0.125;
		host->velocity[i] = mpm.s.velocity[i]*0.125;
	}

	VectorCopy (mpm.mins, host->mins);
	VectorCopy (mpm.maxs, host->maxs);

	client->resp.cmd_angles[0] = SHORT2ANGLE(ucmd->angles[0]);		//set the host???
	client->resp.cmd_angles[1] = SHORT2ANGLE(ucmd->angles[1]);		//set the host???
	client->resp.cmd_angles[2] = SHORT2ANGLE(ucmd->angles[2]);		//set the host???

	if (host->groundentity && !mpm.groundentity && (mpm.cmd.upmove >= 10) && (mpm.waterlevel == 0))
	{
		gi.sound(host, CHAN_VOICE, gi.soundindex("*jump1.wav"), 1, ATTN_NORM, 0);	//make a different noise?
		PlayerNoise(host, host->s.origin, PNOISE_SELF);				//declare this in <g_monster.c>???
	}

	host->viewheight = mpm.viewheight;
	host->waterlevel = mpm.waterlevel;
	host->watertype = mpm.watertype;
	host->groundentity = mpm.groundentity;
	if (mpm.groundentity)
		host->groundentity_linkcount = mpm.groundentity->linkcount;

	VectorCopy (mpm.viewangles, client->v_angle);				//set the host
	VectorCopy (mpm.viewangles, client->ps.viewangles);			//set the host

	gi.linkentity (host);

	if (host->movetype != MOVETYPE_NOCLIP)		//trigger events, finding secrets, jump scares, etc
		G_TouchTriggers (host);

	// touch other objects => allows activation of other monsters and buttons (ending of levels)
	for (i=0 ; i<mpm.numtouch ; i++)
	{
		other = mpm.touchents[i];
		for (j=0 ; j<i ; j++)
			if (mpm.touchents[j] == other)
				break;
		if (j != i)
			continue;	// duplicated
		if (!other->touch)
			continue;
		other->touch (other, host, NULL, NULL);
	}

	// save light level the player is standing on for
	// monster sighting AI
	host->light_level = mpm->cmd.lightlevel;

	// fire weapon from final position if needed
	if (buttons & BUTTON_ATTACK )		//ugh???
	{
		if (!client->weapon_thunk) {
			client->weapon_thunk = true;
			ent->client->pers.weapon->weaponthink (ent);
		}
	}
////////////////////////////////////////////////////

	// set the selected_move NAME according the the pmove AND execute via:

	for ( m = host->hmove_list; m->move_name; m++ )
	{
		if ( !strcmp( m->move_name, selected_move ) )	//!Q_strncasecmp( tr.ent->classname, "monster_", 8 )
		{	
			// found it, set the monsterinfo.currentmove
			m->hmove( host );
			return;
		}
	}
	gi.dprintf ( "%s doesn't have that specific host move defined\n", host->classname );
	
//////////////////////////////////////////////

////////////////Vanilla monster_think/////////////////

	M_MoveFrame (self);

	if (host->linkcount != host->monsterinfo.linkcount)
	{
		host->monsterinfo.linkcount = host->linkcount;
		M_CheckGround (host);
	}
	M_CatagorizePosition (host);
	M_WorldEffects (host);
	M_SetEffects (host);*/
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
	host->old_owner			= host->owner;				// potentially null
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