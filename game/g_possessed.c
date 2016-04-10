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
/*
//////////////////////////////////////////Used for player animation//////////////////////////////////////////////////////
	// client_t->anim_priority		from <g_local.h>	// ent->client->anim_priority = ANIM_ATTACK;
	// find examples of when each of these are set and set them here according the the pmove output
	//if ( ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK) )
	#define	ANIM_BASIC		0		// stand / run
	#define	ANIM_WAVE		1
	#define	ANIM_JUMP		2
	#define	ANIM_PAIN		3
	#define	ANIM_ATTACK		4
	#define	ANIM_DEATH		5
	#define	ANIM_REVERSE	6

	// pmove->pm_flags			from <q_shared.h>		// current_client->ps.pmove.pm_flags & PMF_DUCKED
	// these are all already set by the player's transferred pmove and pmove_state
	#define	PMF_DUCKED			1
	#define	PMF_JUMP_HELD		2
	#define	PMF_ON_GROUND		4
	#define	PMF_TIME_WATERJUMP	8	// pm_time is waterjump
	#define	PMF_TIME_LAND		16	// pm_time is time before rejump
	#define	PMF_TIME_TELEPORT	32	// pm_time is non-moving time
	#define PMF_NO_PREDICTION	64	// temporarily disables prediction (used for grappling hook)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//available amination frames ( temporarily here for easier reference, if kept they will cause weird animation overlap )
#include "m_berserk.h"		//
#include "m_boss2.h"		//	//Hornet			// G:\quake2\baseq2\models/monsters/boss2
#include "m_boss31.h"		//	//makron stage1		// G:\quake2\baseq2\models/monsters/boss3/jorg
#include "m_boss32.h"		//	//makron stage2		// G:\quake2\baseq2\models/monsters/boss3/rider
#include "m_brain.h"		//
#include "m_chick.h"		//
#include "m_flipper.h"		//
#include "m_float.h"		//
#include "m_flyer.h"		//
#include "m_gladiator.h"	//
#include "m_gunner.h"		//
#include "m_hover.h"		//
#include "m_infantry.h"		//
#include "m_medic.h"		//
#include "m_mutant.h"		//
#include "m_parasite.h"		//
#include "m_rider.h"	//	??	//makron?			// G:\quake2\baseq2\models/monsters/boss3/rider
#include "m_soldier.h"		//
#include "m_supertank.h"	//
#include "m_tank.h"			//
*/

// use a structure similar to <g_local.h> for monster's global mmove_t and mframe_t
// OR one similar to how the player's frames are handled....learn that....extra in T_Damage, ChangeWeapon, WeaponGeneric
/*typedef struct
{
	char		*name;
	int			allFrames;	// prototype: all monster-specific frames to be used pulled from the monster's header
						// break down each animation set according to each m_xyzMonster.c mmove_t*********
						// those will also tell which frame to call the fireweapon/jump/etc functions that I really need

// animation vars similar to <g_local.h> in gclient_s
	//these extra members may confuse the hosts[] definition...alot...invalid...crashy-crash
	int			anim_end;
	int			anim_priority;
	qboolean	anim_duck;
	qboolean	anim_run;

} host_t;

// all potential hosts according to <g_spawn.c>
host_t hosts[] = {

	{"monster_berserk",			SP_monster_berserk},		// 243, 1
	{"monster_gladiator",		SP_monster_gladiator},		// 89, 1
	{"monster_gunner",			SP_monster_gunner},			// 208, 1.15
	{"monster_infantry",		SP_monster_infantry},		// 206, 1

	{"monster_soldier_light",	SP_monster_soldier_light},	// 474, 1.2
	{"monster_soldier",			SP_monster_soldier},		// ditto ( verified in m_soldier.c )
	{"monster_soldier_ss",		SP_monster_soldier_ss},		// ditto ( verified in m_soldier.c )

	{"monster_tank",			SP_monster_tank},			// 293, 1
	{"monster_tank_commander",	SP_monster_tank},			// ditto ( verified in m_tank.c )

	{"monster_medic",			SP_monster_medic},			// 236, 1
	{"monster_flipper",			SP_monster_flipper},		// 159, 1
	{"monster_chick",			SP_monster_chick},			// 287, 1
	{"monster_parasite",		SP_monster_parasite},		// 117, 1
	{"monster_flyer",			SP_monster_flyer},			// 150, 1

	{"monster_brain",			SP_monster_brain},			// 221, 1
	{"monster_floater",			SP_monster_floater},		// 247, 1
	{"monster_hover",			SP_monster_hover},			// 204, 1
	{"monster_mutant",			SP_monster_mutant},			// 148, 1
	{"monster_supertank",		SP_monster_supertank},		// 253, 1

	{"monster_boss2",			SP_monster_boss2},			// 180, 1
	{"monster_boss3_stand",		SP_monster_boss3_stand},	// 490,1 ( used boss32 because its in the main source folder )
	{"monster_jorg",			SP_monster_jorg},			// 187, 1

//	{"monster_commander_body", SP_monster_commander_body}, <-----NOT ACTUALLY A MONSTER ( do a case check for this? )...unless it has GOOD animations

	{NULL, NULL}
};
*/

//TODO: write in EACH monster a set of functions that simply set the currentmove to all the ones I want control of
//		then forward declare them here and add them to the list

void berserk_stand (edict_t *self);

hmove_t berserk[] =
{
	{	"attack1",				berserk_stand	},
	{	"attack2",				NULL			}
};

hmove_t gladiator[] =
{
	{	"attack1",				NULL		},
	{	"attack2",				NULL		}
};



host_t hosts[] = 
{
	{ "monster_berserk",		berserk		},
	{ "monster_gladiator",		gladiator	},
/*
	{ "monster_gunner",			gunner		},
	{ "monster_infantry",		infantry	},

	{ "monster_soldier_light",	soldier_li	},
	{ "monster_soldier",		soldier		},
	{ "monster_soldier_ss",		soldier_ss	},

	{"monster_tank",			tank		},
	{ "monster_tank_commander",	tank		},

	{ "monster_medic",			medic		},
	{ "monster_flipper",		flipper		},
	{ "monster_chick",			chick		},
	{ "monster_parasite",		parasite	},
	{ "monster_flyer",			flyer		},

	{ "monster_brain",			brain		},
	{ "monster_floater",		floater		},
	{ "monster_hover",			hover		},
	{ "monster_mutant",			mutant		},	
	{ "monster_supertank",		supertank	},

	{ "monster_boss2",			boss2		},	
	{ "monster_boss3_stand",	boss3_stand	},
	{ "monster_jorg",			jorg		},

//	{"monster_commander_body", SP_monster_commander_body}, <-----NOT ACTUALLY A MONSTER ( do a case check for this? )...unless it has GOOD animations
*/
	{ NULL,						NULL		}
};


void monster_think_possesed (edict_t *host, usercmd_t *cmd, const int * const buttons)
{ 

///////////////how a player uses the pm info///////////////////////
	/*
	edict_t	*other;				//may include lasers, water?, buttons, other monsters, triggers, ***level swaps***
	int		i, j;
	pmove_t	mpm;

	level.current_entity = host;

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
	///////////////////

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

void InitHost ( edict_t *self, edict_t *host ) { //set the host owner to self before this call => no need for self ???

	host_t	*h;

	for ( h = hosts; h->host_name; h++ )
	{
		if ( !strcmp( h->host_name, host->classname ) )
		{	
			// found it, set the available monsterinfo.currentmove(s)
			host->hmoves = h->host_moves;
			return;
		}
	}
	gi.dprintf ( "%s doesn't have host moves defined\n", host->classname );

	// SET the monster-specific take/release noise here
	// MAKE them in the usual spots in ClientThink/g_cmds

}
//*******************
// TMF7 END GHOST MODE
//*******************

