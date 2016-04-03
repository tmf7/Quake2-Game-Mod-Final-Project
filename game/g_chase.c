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
#include "g_local.h"

void UpdateChaseCam(edict_t *ent)
{
	vec3_t o, ownerv, goal;
	edict_t *targ;
	vec3_t forward, right;
	trace_t trace;
	int i;
	vec3_t oldgoal;
	vec3_t angles;

	// is our chase target gone?
//TMF7 BEGIN GHOST MODE
	if ( ent->hostmode || ent->ghostmode ) {

		if ( !ent->host ) {

			gi.cprintf( ent, PRINT_HIGH, "HOST LOST, STOPPING CHASE CAM\n" );
		
			ent->client->chase_target = NULL;
			ent->movetype = MOVETYPE_WALK;
			ent->viewheight = 22;
			ent->client->ps.pmove.pm_type = PM_NORMAL;	
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			gi.linkentity( ent );
			return;
		}
	}
//TMF7 END GHOST MODE
	else if (!ent->client->chase_target->inuse
		|| ent->client->chase_target->client->resp.spectator) {
		edict_t *old = ent->client->chase_target;
		ChaseNext(ent);
		if (ent->client->chase_target == old) {
			ent->client->chase_target = NULL;
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			return;
		}
	}

	targ = ent->client->chase_target;

	VectorCopy(targ->s.origin, ownerv);
	VectorCopy(ent->s.origin, oldgoal);

	ownerv[2] += targ->viewheight;

	//cannot posses non-monsters
	if ( targ->possesed ) { VectorCopy( targ->s.angles, angles ); }	//TMF7 GHOST MODE where the target HOST is looking
	else { VectorCopy(targ->client->v_angle, angles); }				//TMF7 where the target CLIENT is looking

	if (angles[PITCH] > 56)
		angles[PITCH] = 56;
	AngleVectors (angles, forward, right, NULL);
	VectorNormalize(forward);

//TMF7 BEGIN GHOST MODE
	if ( targ->possesed ) { 
		VectorMA(ownerv, -60, forward, o); 
		o[2] += 16;
	}
//TMF7 END GHOST MODE
	else { VectorMA(ownerv, -30, forward, o); }

	if (o[2] < targ->s.origin[2] + 20)
		o[2] = targ->s.origin[2] + 20;

	// jump animation lifts
	if (!targ->groundentity)
		o[2] += 16;

	trace = gi.trace(ownerv, vec3_origin, vec3_origin, o, targ, MASK_SOLID);

	VectorCopy(trace.endpos, goal);

	VectorMA(goal, 2, forward, goal);

	// pad for floors and ceilings
	VectorCopy(goal, o);
	o[2] += 6;
	trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
	if (trace.fraction < 1) {
		VectorCopy(trace.endpos, goal);
		goal[2] -= 6;
	}

	VectorCopy(goal, o);
	o[2] -= 6;
	trace = gi.trace(goal, vec3_origin, vec3_origin, o, targ, MASK_SOLID);
	if (trace.fraction < 1) {
		VectorCopy(trace.endpos, goal);
		goal[2] += 6;
	}

	if ( targ->possesed ) { ent->client->ps.pmove.pm_type = PM_SPECTATOR; } //TMF7 GHOST MODE
	else if (targ->deadflag)								
		ent->client->ps.pmove.pm_type = PM_DEAD;
	else
		ent->client->ps.pmove.pm_type = PM_FREEZE;	//TMF7 if player chases itself, player cant move

	VectorCopy(goal, ent->s.origin);				//TMF7 this sets where the player is relative to the target

	if ( targ->possesed ) {							//TMF7 GHOST MODE
		//for (i=0 ; i<3 ; i++)						//TMF7 player movement direction, reative to the target HOST
		//	ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(targ->s.angles[i] - ent->client->resp.cmd_angles[i]);
	} else {
		for (i=0 ; i<3 ; i++)						//TMF7 player movement direction, reative to the target CLIENT
			 ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(targ->client->v_angle[i] - ent->client->resp.cmd_angles[i]);
	}

	if (targ->deadflag) {
		ent->client->ps.viewangles[ROLL] = 40;
		ent->client->ps.viewangles[PITCH] = -15;
		ent->client->ps.viewangles[YAW] = targ->client->killer_yaw;
	} else if ( targ->possesed ) {											//TMF7 GHOST MODE
		//	VectorCopy(targ->s.angles, ent->client->ps.viewangles);			//TMF7 where the player looks, relative to the target HOST
		//	VectorCopy(targ->s.angles, ent->client->v_angle);
	} else {
			VectorCopy(targ->client->v_angle, ent->client->ps.viewangles);	//TMF7 where the player looks, relative to the target CLIENT
			VectorCopy(targ->client->v_angle, ent->client->v_angle);
	}

	ent->viewheight = 0;
	ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
	gi.linkentity(ent);
}

void ChaseNext(edict_t *ent)
{
	int i;
	edict_t *e;

	if (!ent->client->chase_target)
		return;

	i = ent->client->chase_target - g_edicts;
	do {
		i++;
		if (i > maxclients->value)
			i = 1;
		e = g_edicts + i;
		if (!e->inuse)
			continue;
		if (!e->client->resp.spectator)
			break;
	} while (e != ent->client->chase_target);

	ent->client->chase_target = e;
	ent->client->update_chase = true;
}

void ChasePrev(edict_t *ent)
{
	int i;
	edict_t *e;

	if (!ent->client->chase_target)
		return;

	i = ent->client->chase_target - g_edicts;
	do {
		i--;
		if (i < 1)
			i = maxclients->value;
		e = g_edicts + i;
		if (!e->inuse)
			continue;
		if (!e->client->resp.spectator)
			break;
	} while (e != ent->client->chase_target);

	ent->client->chase_target = e;
	ent->client->update_chase = true;
}

void GetChaseTarget(edict_t *ent)
{
	int i;
	edict_t *other;

	for (i = 1; i <= maxclients->value; i++) {
		other = g_edicts + i;
		if (other->inuse && !other->client->resp.spectator) {
			ent->client->chase_target = other;
			ent->client->update_chase = true;
			UpdateChaseCam(ent);
			return;
		}
	}
	gi.centerprintf(ent, "No other players to chase.");
}

//TMF7 BEGIN THIRD PERSON
void SetChaseTarget( edict_t *self, edict_t *host )
{
	if ( !host ) { return; }

	self->client->chase_target = host;
	self->client->update_chase = true;		//only checked in ctf project
	UpdateChaseCam( self );
}
//TMF7 END THIRD PERSON
