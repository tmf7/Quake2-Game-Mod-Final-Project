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



/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
	if (deathmatch->value || coop->value)
		ent->client->showscores = true;
	VectorCopy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	// add the layout

	if (deathmatch->value || coop->value)
	{
		DeathmatchScoreboardMessage (ent, NULL);
		gi.unicast (ent, true);
	}

}

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (coop->value)
		{
			for (i=0 ; i<maxclients->value ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inuse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_ITEMS; n++)
				{
					if (itemlist[n].flags & IT_KEY)
						client->client->pers.inventory[n] = 0;
				}
			}
		}
	}
	else
	{
		if (!deathmatch->value)
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission (client);
	}
}


/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k;
	int		sorted[MAX_CLIENTS];
	int		sortedscores[MAX_CLIENTS];
	int		score, total;
	int		picnum;
	int		x, y;
	gclient_t	*cl;
	edict_t		*cl_ent;
	char	*tag;

	// sort the clients by score
	total = 0;
	for (i=0 ; i<game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse || game.clients[i].resp.spectator)
			continue;
		score = game.clients[i].resp.score;
		for (j=0 ; j<total ; j++)
		{
			if (score > sortedscores[j])
				break;
		}
		for (k=total ; k>j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// print level name and exit rules
	string[0] = 0;

	stringlength = strlen(string);

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (i=0 ; i<total ; i++)
	{
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		picnum = gi.imageindex ("i_fixme");
		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == ent)
			tag = "tag1";
		else if (cl_ent == killer)
			tag = "tag2";
		else
			tag = NULL;
		if (tag)
		{
			Com_sprintf (entry, sizeof(entry),
				"xv %i yv %i picn %s ",x+32, y, tag);
			j = strlen(entry);
			if (stringlength + j > 1024)
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
		}

		// send the layout
		Com_sprintf (entry, sizeof(entry),
			"client %i %i %i %i %i %i ",
			x, y, sorted[i], cl->resp.score, cl->ping, (level.framenum - cl->resp.enterframe)/600);
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	DeathmatchScoreboardMessage (ent, ent->enemy);
	gi.unicast (ent, true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;
	ent->client->showabilities = false;			//TMF7 GHOST MODE ( ghud )
	ent->client->showcollection = false;		//TMF7 GHOST MODE ( ghud )

	if (!deathmatch->value && !coop->value)
		return;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboard (ent);
}


/*
==================
HelpComputer

Draw help computer.
==================
*/
void HelpComputer (edict_t *ent)
{
	char	string[1024];
	char	*sk;

	if (skill->value == 0)
		sk = "easy";
	else if (skill->value == 1)
		sk = "medium";
	else if (skill->value == 2)
		sk = "hard";
	else
		sk = "hard+";

	// send the layout
	Com_sprintf (string, sizeof(string),
		"xv 32 yv 8 picn help "			// background
		"xv 202 yv 12 string2 \"%s\" "		// skill
		"xv 0 yv 24 cstring2 \"%s\" "		// level name
		"xv 0 yv 54 cstring2 \"%s\" "		// help 1
		"xv 0 yv 110 cstring2 \"%s\" "		// help 2
		"xv 50 yv 164 string2 \" kills     goals    secrets\" "
		"xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ", 
		sk,
		level.level_name,
		game.helpmessage1,
		game.helpmessage2,
		level.killed_monsters, level.total_monsters, 
		level.found_goals, level.total_goals,
		level.found_secrets, level.total_secrets);

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}


/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
	// this is for backwards compatability
	if (deathmatch->value)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;
	ent->client->showabilities = false;		//TMF7 GHOST MODE ( ghud )
	ent->client->showcollection = false;		//TMF7 GHOST MODE ( ghud )

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputer (ent);
}


//=======================================================================

/*
===============
G_SetStats
===============
*/
void G_SetStats (edict_t *ent)
{
	gitem_t		*item;
	int			index, cells;
	int			power_armor_type;

	//
	// health
	//
	ent->client->ps.stats[STAT_HEALTH_ICON] = level.pic_health;
	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//
	if (!ent->client->ammo_index /* || !ent->client->pers.inventory[ent->client->ammo_index] */)
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
	}
	else
	{
		item = &itemlist[ent->client->ammo_index];
		ent->client->ps.stats[STAT_AMMO_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_AMMO] = ent->client->pers.inventory[ent->client->ammo_index];
	}
	
	//
	// armor
	//
	power_armor_type = PowerArmorType (ent);
	if (power_armor_type)
	{
		cells = ent->client->pers.inventory[ITEM_INDEX(FindItem ("cells"))];
		if (cells == 0)
		{	// ran out of cells for power armor
			ent->flags &= ~FL_POWER_ARMOR;
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/power2.wav"), 1, ATTN_NORM, 0);
			power_armor_type = 0;;
		}
	}

	index = ArmorIndex (ent);
	if (power_armor_type && (!index || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex ("i_powershield");
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (index)
	{
		item = GetItemByIndex (index);
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.inventory[index];
	}
	else
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}

	//
	// pickup message
	//
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
	}

	//
	// timers
	//
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_quad");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quad_framenum - level.framenum)/10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_invulnerability");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->invincible_framenum - level.framenum)/10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_envirosuit");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->enviro_framenum - level.framenum)/10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_rebreather");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->breather_framenum - level.framenum)/10;
	}
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	//
	// selected item
	//
	if (ent->client->pers.selected_item == -1)
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	else
		ent->client->ps.stats[STAT_SELECTED_ICON] = gi.imageindex (itemlist[ent->client->pers.selected_item].icon);

	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.selected_item;

	//
	// layouts
	//
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (deathmatch->value)
	{
		if (ent->client->pers.health <= 0 || level.intermissiontime
			|| ent->client->showscores)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}
	else															
	{											// TMF7 GHOST MODE ( two new conditions here for soul ability/collection layouts )
		if (ent->client->showscores || ent->client->showhelp || ent->client->showcollection || ent->client->showabilities )	
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}

	//
	// frags
	//
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

	//
	// help icon / current weapon if not shown
	//
	if (ent->client->pers.helpchanged && (level.framenum&8) )
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex ("i_help");
	else if ( (ent->client->pers.hand == CENTER_HANDED || ent->client->ps.fov > 91)
		&& ent->client->pers.weapon)
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex (ent->client->pers.weapon->icon);
	else
		ent->client->ps.stats[STAT_HELPICON] = 0;

	ent->client->ps.stats[STAT_SPECTATOR] = 0;

// TMF7 BEGIN GHOST MODE ( ghud )
	//
	// souls
	//
	if ( ent->client->pool_of_souls ) {
		ent->client->ps.stats[STAT_SOULS_ICON] = gi.imageindex ("turtle");
		ent->client->ps.stats[STAT_SOULS] = ent->client->pool_of_souls;
	} else {
		ent->client->ps.stats[STAT_SOULS_ICON] = 0;
		ent->client->ps.stats[STAT_SOULS] = 0;
	}

	//
	// soul pickup message
	//
	if ( level.time > ent->client->pickup_soul_msg_time ) {
		ent->client->ps.stats[STAT_SOULS_STRING] = CS_ITEMS+game.num_items+MAX_SOUL_TYPES+1;	// because 0 displays the level name
	}
// TMF7 END GHOST MODE ( ghud )
}

/*
===============
G_CheckChaseStats
===============
*/
void G_CheckChaseStats (edict_t *ent)
{
	int i;
	gclient_t *cl;

	for (i = 1; i <= maxclients->value; i++) {
		cl = g_edicts[i].client;
		if (!g_edicts[i].inuse || cl->chase_target != ent)
			continue;
		memcpy(cl->ps.stats, ent->client->ps.stats, sizeof(cl->ps.stats));
		G_SetSpectatorStats(g_edicts + i);
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void G_SetSpectatorStats (edict_t *ent)
{
	gclient_t *cl = ent->client;

	if (!cl->chase_target)
		G_SetStats (ent);

	cl->ps.stats[STAT_SPECTATOR] = 1;

	// layouts are independant in spectator
	cl->ps.stats[STAT_LAYOUTS] = 0;
	if (cl->pers.health <= 0 || level.intermissiontime || cl->showscores)
		cl->ps.stats[STAT_LAYOUTS] |= 1;
	if (cl->showinventory && cl->pers.health > 0)
		cl->ps.stats[STAT_LAYOUTS] |= 2;

	if (cl->chase_target && cl->chase_target->inuse)
		cl->ps.stats[STAT_CHASE] = CS_PLAYERSKINS + 
			(cl->chase_target - g_edicts) - 1;
	else
		cl->ps.stats[STAT_CHASE] = 0;
}

//TMF7 BEGIN GHOST MODE
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
	for( index = 0; index < MAX_SOUL_TYPES; index++ ) {

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
		soul_name = GetMonsterByIndex( index );
		strncpy( display_name, soul_name, strlen(soul_name)-5 );		// dont use " Soul"

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
														strcat( string, "xv 56 yv 152 string2 \"    ---host speak---\" "	    );
														strcat( string, "xv 56 yv 160 string2 \"shift+fire follower control\" " );
														strcat( string, "xv 56 yv 168 string2 \"shift+mouse3 free follower\" "  );
														strcat( string, "xv 56 yv 176 string2 \"  alt+mouse3 shout an alert\" " );
														strcat( string, "xv 56 yv 184 string2 \"      mouse3 call for help\" "  );
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
		//												strcat( string, "xv 56 yv 144 string2 \"possession time: ###/###\" "); // CHANGE 

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

//TMF7 END GHOST MODE