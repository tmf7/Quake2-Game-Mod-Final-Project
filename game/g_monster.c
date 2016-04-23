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


void SP_LostMonsterSoul ( edict_t *self );

//
// monster weapons
//

//FIXME mosnters should call these with a totally accurate direction
// and we can mess it up based on skill.  Spread should be for normal
// and we can tighten or loosen based on skill.  We could muck with
// the damages too, but I'm not sure that's such a good idea.
void monster_fire_bullet (edict_t *self, vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype)
{
	fire_bullet (self, start, dir, damage, kick, hspread, vspread, MOD_UNKNOWN);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void monster_fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype)
{
	fire_shotgun (self, start, aimdir, damage, kick, hspread, vspread, count, MOD_UNKNOWN);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void monster_fire_blaster (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect)
{
	fire_blaster (self, start, dir, damage, speed, effect, false);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}	

void monster_fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype)
{
	fire_grenade (self, start, aimdir, damage, speed, 2.5, damage+40);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void monster_fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype)
{
	fire_rocket (self, start, dir, damage, speed, damage+20, damage);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}	

void monster_fire_railgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype)
{
	fire_rail (self, start, aimdir, damage, kick);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}

void monster_fire_bfg (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype)
{
	fire_bfg (self, start, aimdir, damage, speed, damage_radius);

	gi.WriteByte (svc_muzzleflash2);
	gi.WriteShort (self - g_edicts);
	gi.WriteByte (flashtype);
	gi.multicast (start, MULTICAST_PVS);
}



//
// Monster utility functions
//

static void M_FliesOff (edict_t *self)
{
	self->s.effects &= ~EF_FLIES;
	self->s.sound = 0;
}

static void M_FliesOn (edict_t *self)
{
	if (self->waterlevel)
		return;
	self->s.effects |= EF_FLIES;
	self->s.sound = gi.soundindex ("infantry/inflies1.wav");
	self->think = M_FliesOff;
	self->nextthink = level.time + 60;
}

void M_FlyCheck (edict_t *self)
{
	if (self->waterlevel)
		return;

	if (random() > 0.5)
		return;

	self->think = M_FliesOn;
	self->nextthink = level.time + 5 + 10 * random();
}

void AttackFinished (edict_t *self, float time)
{
	self->monsterinfo.attack_finished = level.time + time;
}


void M_CheckGround (edict_t *ent)
{
	vec3_t		point;
	trace_t		trace;

	if (ent->flags & (FL_SWIM|FL_FLY))
		return;

	if (ent->velocity[2] > 100)
	{
		ent->groundentity = NULL;
		return;
	}

// if the hull point one-quarter unit down is solid the entity is on ground
	point[0] = ent->s.origin[0];
	point[1] = ent->s.origin[1];
	point[2] = ent->s.origin[2] - 0.25;

	trace = gi.trace (ent->s.origin, ent->mins, ent->maxs, point, ent, MASK_MONSTERSOLID);

	// check steepness
	if ( trace.plane.normal[2] < 0.7 && !trace.startsolid)
	{
		ent->groundentity = NULL;
		return;
	}

//	ent->groundentity = trace.ent;
//	ent->groundentity_linkcount = trace.ent->linkcount;
//	if (!trace.startsolid && !trace.allsolid)
//		VectorCopy (trace.endpos, ent->s.origin);
	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy (trace.endpos, ent->s.origin);
		ent->groundentity = trace.ent;
		ent->groundentity_linkcount = trace.ent->linkcount;
		ent->velocity[2] = 0;
	}
}


void M_CatagorizePosition (edict_t *ent)
{
	vec3_t		point;
	int			cont;

//
// get waterlevel
//
	point[0] = ent->s.origin[0];
	point[1] = ent->s.origin[1];
	point[2] = ent->s.origin[2] + ent->mins[2] + 1;	
	cont = gi.pointcontents (point);

	if (!(cont & MASK_WATER))
	{
		ent->waterlevel = 0;
		ent->watertype = 0;
		return;
	}

	ent->watertype = cont;
	ent->waterlevel = 1;
	point[2] += 26;
	cont = gi.pointcontents (point);
	if (!(cont & MASK_WATER))
		return;

	ent->waterlevel = 2;
	point[2] += 22;
	cont = gi.pointcontents (point);
	if (cont & MASK_WATER)
		ent->waterlevel = 3;
}


void M_WorldEffects (edict_t *ent)
{
	int		dmg;

	if (ent->health > 0)
	{
		if (!(ent->flags & FL_SWIM))
		{
			if (ent->waterlevel < 3)
			{
				ent->air_finished = level.time + 12;
			}
			else if (ent->air_finished < level.time)
			{	// drown!
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (ent, world, world, vec3_origin, ent->s.origin, vec3_origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
		else
		{
			if (ent->waterlevel > 0)
			{
				ent->air_finished = level.time + 9;
			}
			else if (ent->air_finished < level.time)
			{	// suffocate!
				if (ent->pain_debounce_time < level.time)
				{
					dmg = 2 + 2 * floor(level.time - ent->air_finished);
					if (dmg > 15)
						dmg = 15;
					T_Damage (ent, world, world, vec3_origin, ent->s.origin, vec3_origin, dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
					ent->pain_debounce_time = level.time + 1;
				}
			}
		}
	}
	
	if (ent->waterlevel == 0)
	{
		if (ent->flags & FL_INWATER)
		{	
			gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_out.wav"), 1, ATTN_NORM, 0);
			ent->flags &= ~FL_INWATER;
		}
		return;
	}

	if ((ent->watertype & CONTENTS_LAVA) && !(ent->flags & FL_IMMUNE_LAVA))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 0.2;
			T_Damage (ent, world, world, vec3_origin, ent->s.origin, vec3_origin, 10*ent->waterlevel, 0, 0, MOD_LAVA);
		}
	}
	if ((ent->watertype & CONTENTS_SLIME) && !(ent->flags & FL_IMMUNE_SLIME))
	{
		if (ent->damage_debounce_time < level.time)
		{
			ent->damage_debounce_time = level.time + 1;
			T_Damage (ent, world, world, vec3_origin, ent->s.origin, vec3_origin, 4*ent->waterlevel, 0, 0, MOD_SLIME);
		}
	}
	
	if ( !(ent->flags & FL_INWATER) )
	{	
		if (!(ent->svflags & SVF_DEADMONSTER))
		{
			if (ent->watertype & CONTENTS_LAVA)
				if (random() <= 0.5)
					gi.sound (ent, CHAN_BODY, gi.soundindex("player/lava1.wav"), 1, ATTN_NORM, 0);
				else
					gi.sound (ent, CHAN_BODY, gi.soundindex("player/lava2.wav"), 1, ATTN_NORM, 0);
			else if (ent->watertype & CONTENTS_SLIME)
				gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_in.wav"), 1, ATTN_NORM, 0);
			else if (ent->watertype & CONTENTS_WATER)
				gi.sound (ent, CHAN_BODY, gi.soundindex("player/watr_in.wav"), 1, ATTN_NORM, 0);
		}

		ent->flags |= FL_INWATER;
		ent->damage_debounce_time = 0;
	}
}


void M_droptofloor (edict_t *ent)
{
	vec3_t		end;
	trace_t		trace;

	ent->s.origin[2] += 1;
	VectorCopy (ent->s.origin, end);
	end[2] -= 256;
	
	trace = gi.trace (ent->s.origin, ent->mins, ent->maxs, end, ent, MASK_MONSTERSOLID);

	if (trace.fraction == 1 || trace.allsolid)
		return;

	VectorCopy (trace.endpos, ent->s.origin);

	gi.linkentity (ent);
	M_CheckGround (ent);
	M_CatagorizePosition (ent);
}


void M_SetEffects (edict_t *ent)
{
	ent->s.effects &= ~(EF_COLOR_SHELL|EF_POWERSCREEN);
	ent->s.renderfx &= ~(RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);

	if (ent->monsterinfo.aiflags & AI_RESURRECTING)
	{
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderfx |= RF_SHELL_RED;
	}

	if (ent->health <= 0)
		return;

	if (ent->powerarmor_time > level.time)
	{
		if (ent->monsterinfo.power_armor_type == POWER_ARMOR_SCREEN)
		{
			ent->s.effects |= EF_POWERSCREEN;
		}
		else if (ent->monsterinfo.power_armor_type == POWER_ARMOR_SHIELD)
		{
			ent->s.effects |= EF_COLOR_SHELL;
			ent->s.renderfx |= RF_SHELL_GREEN;
		}
	}
}


void M_MoveFrame (edict_t *self)
{
	mmove_t	*move;
	int		index;
	hmove_t *perform_move;		//TMF7 GHOST MODE

	move = self->monsterinfo.currentmove;
	self->nextthink = level.time + FRAMETIME;

//TMF7 BEGIN GHOST MODE ( overrride )
	if ( self->possessed 
		&& self->owner->client->soul_abilities & UBERHOST 
		&& self->s.frame == move->lastframe ) {

			if ( self->host_anim_priority == ANIM_PAIN ) {

				// prevent constant-cycle pain animations
				perform_move = find_host_move ( self, "stand1" ); // or try "stand2"
				if ( perform_move && perform_move->hmove ) { perform_move->hmove( self ); }
			}

			self->host_anim_priority = ANIM_BASIC;		// everyone has it, only true uberhosts use it
	}
//TMF7 BEGIN GHOST MODE ( overrride )

	if ((self->monsterinfo.nextframe) && (self->monsterinfo.nextframe >= move->firstframe) && (self->monsterinfo.nextframe <= move->lastframe))
	{
		self->s.frame = self->monsterinfo.nextframe;
		self->monsterinfo.nextframe = 0;
	}
	else
	{
		if (self->s.frame == move->lastframe)
		{
			if (move->endfunc)		//TMF7 GHOST MODE ( add host override conditions? )
			{
				move->endfunc (self);

				// regrab move, endfunc is very likely to change it
				move = self->monsterinfo.currentmove;

				// check for death
				if (self->svflags & SVF_DEADMONSTER)
					return;
			}
		}

		if (self->s.frame < move->firstframe || self->s.frame > move->lastframe)
		{
			self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
			self->s.frame = move->firstframe;
		}
		else
		{
			if (!(self->monsterinfo.aiflags & AI_HOLD_FRAME))
			{
				self->s.frame++;
				if (self->s.frame > move->lastframe)
					self->s.frame = move->firstframe;
			}
		}
	}

	index = self->s.frame - move->firstframe;

	
	if (move->frame[index].aifunc) {		//TMF7 GHOST MODE ( add host override conditions? )

		if (!(self->monsterinfo.aiflags & AI_HOLD_FRAME))
			move->frame[index].aifunc (self, move->frame[index].dist * self->monsterinfo.scale);
		else
			move->frame[index].aifunc (self, 0);
	}

	if (move->frame[index].thinkfunc)
		move->frame[index].thinkfunc (self);
}


void monster_think (edict_t *self)
{
//TMF7 BEGIN GHOST MODE
	vec3_t owner_vec;

	// as a follower, always face the owner
	if ( !self->possessed 
		&& self->owner 
		&& self->owner->client ) {

		// a possessed monster just said hi, say hi back
		if ( self->hostLaugh && level.time > (self->owner->client->giveOrdersTime - 2.0f) ) {
			gi.sound ( self, CHAN_VOICE, gi.soundindex( "slighost/hi.wav" ), 1, ATTN_NORM, 0);
			self->hostLaugh = false;
		}

		if ( self->host_target == self->owner ) {
			VectorSubtract (self->owner->s.origin, self->s.origin, owner_vec);
			self->ideal_yaw = vectoyaw(owner_vec);
			M_ChangeYaw( self );
		}
	}
//TMF7 END GHOST MODE

	M_MoveFrame (self);			//TMF7 this resolves the monsterinfo.currentmove aifunc and thinkfunc
								//which are set according to  g_ai.c's monsterinfo.run (etc) calls
								//NOTE: a monter's currentmove could easily swap out before completing its mframe set

	if (self->linkcount != self->monsterinfo.linkcount)
	{
		self->monsterinfo.linkcount = self->linkcount;
		M_CheckGround (self);
	}
	M_CatagorizePosition (self);
	M_WorldEffects (self);
	M_SetEffects (self);
}

/*
================
monster_use

Using a monster makes it angry at the current activator
================
*/
void monster_use (edict_t *self, edict_t *other, edict_t *activator)
{
	if (self->enemy)
		return;
	if (self->health <= 0)
		return;
	if (activator->flags & FL_NOTARGET)
		return;
	if (!(activator->client) && !(activator->monsterinfo.aiflags & AI_GOOD_GUY))
		return;
	
// delay reaction so if the monster is teleported, its sound is still heard
	self->enemy = activator;
	FoundTarget (self);
}


void monster_start_go (edict_t *self);


void monster_triggered_spawn (edict_t *self)
{
	self->s.origin[2] += 1;
	KillBox (self);

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_STEP;
	self->svflags &= ~SVF_NOCLIENT;
	self->air_finished = level.time + 12;
	gi.linkentity (self);

	monster_start_go (self);

	if (self->enemy && !(self->spawnflags & 1) && !(self->enemy->flags & FL_NOTARGET))
	{
		FoundTarget (self);
	}
	else
	{
		self->enemy = NULL;
	}
}

void monster_triggered_spawn_use (edict_t *self, edict_t *other, edict_t *activator)
{
	// we have a one frame delay here so we don't telefrag the guy who activated us
	self->think = monster_triggered_spawn;
	self->nextthink = level.time + FRAMETIME;
	if (activator->client)
		self->enemy = activator;
	self->use = monster_use;
}

void monster_triggered_start (edict_t *self)
{
	self->solid = SOLID_NOT;
	self->movetype = MOVETYPE_NONE;
	self->svflags |= SVF_NOCLIENT;
	self->nextthink = 0;
	self->use = monster_triggered_spawn_use;
}


/*
================
monster_death_use

When a monster dies, it fires all of its targets with the current
enemy as activator.
================
*/
void monster_death_use (edict_t *self)
{
	self->flags &= ~(FL_FLY|FL_SWIM);
	self->monsterinfo.aiflags &= AI_GOOD_GUY;

// TMF7 BEGIN GHOST MODE
	SP_LostMonsterSoul( self );
	
	if ( self->enemy && self->enemy->possessed ) 
		self->enemy->hostLaugh = true;
// TMF7 END GHOST MODE
	
	if (self->item)
	{
		Drop_Item (self, self->item);
		self->item = NULL;
	}

	if (self->deathtarget)
		self->target = self->deathtarget;

	if (!self->target)
		return;

	G_UseTargets (self, self->enemy);
}


//============================================================================

qboolean monster_start (edict_t *self)
{
	if (deathmatch->value)
	{
		G_FreeEdict (self);
		return false;
	}

	if ((self->spawnflags & 4) && !(self->monsterinfo.aiflags & AI_GOOD_GUY))
	{
		self->spawnflags &= ~4;
		self->spawnflags |= 1;
//		gi.dprintf("fixed spawnflags on %s at %s\n", self->classname, vtos(self->s.origin));
	}

	if (!(self->monsterinfo.aiflags & AI_GOOD_GUY))
		level.total_monsters++;

	self->nextthink = level.time + FRAMETIME;
	self->svflags |= SVF_MONSTER;
	self->s.renderfx |= RF_FRAMELERP;
	self->takedamage = DAMAGE_AIM;
	self->air_finished = level.time + 12;
	self->use = monster_use;
	self->max_health = self->health;
	self->clipmask = MASK_MONSTERSOLID;

	self->s.skinnum = 0;
	self->deadflag = DEAD_NO;
	self->svflags &= ~SVF_DEADMONSTER;

	if (!self->monsterinfo.checkattack)
		self->monsterinfo.checkattack = M_CheckAttack;
	VectorCopy (self->s.origin, self->s.old_origin);

	if (st.item)
	{
		self->item = FindItemByClassname (st.item);
		if (!self->item)
			gi.dprintf("%s at %s has bad item: %s\n", self->classname, vtos(self->s.origin), st.item);
	}

	// randomize what frame they start on
	if (self->monsterinfo.currentmove)
		self->s.frame = self->monsterinfo.currentmove->firstframe + (rand() % (self->monsterinfo.currentmove->lastframe - self->monsterinfo.currentmove->firstframe + 1));

	self->possessed = false;	//TMF7 GHOST MODE

	return true;
}

void monster_start_go (edict_t *self)
{
	vec3_t	v;

	if (self->health <= 0)
		return;

	// check for target to combat_point and change to combattarget
	if (self->target)
	{
		qboolean	notcombat;
		qboolean	fixup;
		edict_t		*target;

		target = NULL;
		notcombat = false;
		fixup = false;
		while ((target = G_Find (target, FOFS(targetname), self->target)) != NULL)
		{
			if (strcmp(target->classname, "point_combat") == 0)
			{
				self->combattarget = self->target;
				fixup = true;
			}
			else
			{
				notcombat = true;
			}
		}
		if (notcombat && self->combattarget)
			gi.dprintf("%s at %s has target with mixed types\n", self->classname, vtos(self->s.origin));
		if (fixup)
			self->target = NULL;
	}

	// validate combattarget
	if (self->combattarget)
	{
		edict_t		*target;

		target = NULL;
		while ((target = G_Find (target, FOFS(targetname), self->combattarget)) != NULL)
		{
			if (strcmp(target->classname, "point_combat") != 0)
			{
				gi.dprintf("%s at (%i %i %i) has a bad combattarget %s : %s at (%i %i %i)\n",
					self->classname, (int)self->s.origin[0], (int)self->s.origin[1], (int)self->s.origin[2],
					self->combattarget, target->classname, (int)target->s.origin[0], (int)target->s.origin[1],
					(int)target->s.origin[2]);
			}
		}
	}

	if (self->target)
	{
		self->goalentity = self->movetarget = G_PickTarget(self->target);
		if (!self->movetarget)
		{
			gi.dprintf ("%s can't find target %s at %s\n", self->classname, self->target, vtos(self->s.origin));
			self->target = NULL;
			self->monsterinfo.pausetime = 100000000;
			self->monsterinfo.stand (self);
		}
		else if (strcmp (self->movetarget->classname, "path_corner") == 0)
		{
			VectorSubtract (self->goalentity->s.origin, self->s.origin, v);
			self->ideal_yaw = self->s.angles[YAW] = vectoyaw(v);
			self->monsterinfo.walk (self);
			self->target = NULL;
		}
		else
		{
			self->goalentity = self->movetarget = NULL;
			self->monsterinfo.pausetime = 100000000;
			self->monsterinfo.stand (self);
		}
	}
	else
	{
		self->monsterinfo.pausetime = 100000000;
		self->monsterinfo.stand (self);
	}

	self->think = monster_think;
	self->nextthink = level.time + FRAMETIME;
}


void walkmonster_start_go (edict_t *self)
{
	if (!(self->spawnflags & 2) && level.time < 1)
	{
		M_droptofloor (self);

		if (self->groundentity)
			if (!M_walkmove (self, 0, 0))
				gi.dprintf ("%s in solid at %s\n", self->classname, vtos(self->s.origin));
	}
	
	if (!self->yaw_speed)
		self->yaw_speed = 20;
	self->viewheight = 25;

	monster_start_go (self);

	if (self->spawnflags & 2)
		monster_triggered_start (self);
}

void walkmonster_start (edict_t *self)
{
	self->think = walkmonster_start_go;
	monster_start (self);
}


void flymonster_start_go (edict_t *self)
{
	if (!M_walkmove (self, 0, 0))
		gi.dprintf ("%s in solid at %s\n", self->classname, vtos(self->s.origin));

	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->viewheight = 25;

	monster_start_go (self);

	if (self->spawnflags & 2)
		monster_triggered_start (self);
}


void flymonster_start (edict_t *self)
{
	self->flags |= FL_FLY;
	self->think = flymonster_start_go;
	monster_start (self);
}


void swimmonster_start_go (edict_t *self)
{
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->viewheight = 10;

	monster_start_go (self);

	if (self->spawnflags & 2)
		monster_triggered_start (self);
}

void swimmonster_start (edict_t *self)
{
	self->flags |= FL_SWIM;
	self->think = swimmonster_start_go;
	monster_start (self);
}


//***********************
// TMF7 BEGIN GHOST MODE
//***********************

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
	
	// only living clients in ghostmode can gather souls
	if ( !other->client || other->deadflag != DEAD_NO || !other->client->ghostmode ) 
		{ return; }

	new_soul_collector_level = 1;

	//pickup verification
	other->client->bonus_alpha = 0.35;
	gi.sound ( other, CHAN_ITEM, gi.soundindex("soul/pickupsoul.wav") , 1, ATTN_NORM, 0);

// BEGIN ( ghud )
	// flash the pool_of_souls hud number ( also flash it when losing souls ) ***********************************
	other->client->soulChange = true;

	// the "string" is a hardcoded value via SetMonsterNames(void) initialized at worldspawn
	other->client->ps.stats[STAT_SOULS_STRING] = CS_ITEMS + game.num_items + (soul->monster_soul_index); 
	other->client->pickup_soul_msg_time = level.time + 3.0;

	other->client->newSoulLevel = false;
// END ( ghud )

	old_soul_collector_level = other->client->soul_collector_level;
	
	other->client->pool_of_souls += soul->mass;
	other->client->soulCollection[ soul->monster_soul_index ]++;

	// balance these level thresholds better
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

	//jump back to idle animation if outside it
	if ( soul->s.frame > soul->mSoulLastFrame || soul->s.frame < soul->mSoulFirstFrame) { soul->s.frame = soul->mSoulFirstFrame; }

	//continue idle animation
	if ( soul->s.frame == soul->mSoulLastFrame ) { soul->s.frame = soul->mSoulFirstFrame; }
	else if ( soul->s.frame < soul->mSoulLastFrame && soul->s.frame >= soul->mSoulFirstFrame ) { soul->s.frame++; }

	soul->nextthink = level.time + FRAMETIME;
}

void SP_LostMonsterSoul ( edict_t *self ) {

	int savedEntNumber;
	mmove_t	*move;
	edict_t	*soul;

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

	soul->classname		= "soul";

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

	soul->solid			= SOLID_TRIGGER;
	soul->clipmask		= self->clipmask;
	soul->flags		   |= FL_NO_KNOCKBACK;					// set additional flags in monster_soul_think
	soul->svflags		= SVF_SOUL;							// allow server-side per-client visiblity check
	soul->movetype		= MOVETYPE_TOSS;

	soul->possessed		= false;
	soul->monster_soul_index = self->monster_soul_index;
	soul->take_host_noise = self->drop_host_noise;

	//set in monster_soul_think
	soul->touch = NULL;
	// never dies, only gets freed

	soul->think = monster_soul_think;
	soul->nextthink = level.time + FRAMETIME;

//	gi.sound (soul, CHAN_AUTO, gi.soundindex ("soul/soulspawn.wav"), 1, ATTN_NORM, 0 );		// BUG: why does this constantly repeat?
//	gi.dprintf( "SOUL SPAWNED\n" );

	gi.linkentity ( soul );
}

//*****************
// BEGIN GHUD INFO
//*****************

/* 
// FROM <g_local.h>

enum monster_souls {
	BERSERK,
	GLADIATOR,
	GUNNER,
	INFANTRY,
	SOLDIER_LIGHT,
	SOLDIER,
	SOLDIER_SS,
	TANK,
	TANK_COMMANDER,
	MEDIC,
	FLIPPER,
	CHICK,
	PARASITE,
	FLYER,
	BRAIN,
	FLOATER,
	HOVER,
	MUTANT,
	SUPERTANK,
	BOSS2,
	BOSS3_STAND,
	JORG
};
*/

// In each xyzmonster.c ( eg: soldier.c ) when a specific monster type spawns
// it grabs a name out of this list according to its monster_soul_index
char * monster_souls[] =
{
	"Beserker Soul",
	"Gladiator Soul",
	"Gunner Soul",
	"Infantry Soul",
	"Soldier Soul",
	"Tank Soul",
	"Commander Soul",
	"Medic Soul",
	"Flipper Soul",
	"Maiden Soul",
	"Parasite Soul",
	"Flyer Soul",
	"Brain Soul",
	"Floater Soul",
	"Hover Soul",
	"Mutant Soul",
	"Supertank Soul",
	"Hornet Soul",		// BOSS2
	"Makron Soul",		// BOSS3/BOSS3_STAND
	"Jorg Soul"			// JORG
};


/*
===============
SetMonsterNames

Called by worldspawn
===============
*/
void SetMonsterNames (void)
{
	int		i;
	char	*monser_name;

	// adding configstrings in the buffer area: (CS_ITEMS+MAX_ITEMS) - (game.num_items)
	// at the end of the items list but before the CS_PLAYERSKINS indexes
	for ( i = 0; i < MAX_SOUL_TYPES; i++ ) {

		if ( CS_ITEMS+game.num_items+i >= CS_PLAYERSKINS )
			break;

		monser_name = monster_souls[i];
		gi.configstring( CS_ITEMS+game.num_items+i, monser_name );
	}
}

char * GetMonsterByIndex (int index)
{
	if ( index < 0 || index >= MAX_SOUL_TYPES )
		return NULL;

	return monster_souls[index];
}

//***************
// END GHUD INFO
//***************

//*************************
//	TMF7 END GHOST MODE
//*************************