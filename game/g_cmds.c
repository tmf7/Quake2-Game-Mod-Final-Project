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
#include "m_player.h"

char *ClientTeam (edict_t *ent)
{
	char		*p;
	static char	value[512];

	value[0] = 0;

	if (!ent->client)
		return value;

	strcpy(value, Info_ValueForKey (ent->client->pers.userinfo, "skin"));
	p = strchr(value, '/');
	if (!p)
		return value;

	if ((int)(dmflags->value) & DF_MODELTEAMS)
	{
		*p = 0;
		return value;
	}

	// if ((int)(dmflags->value) & DF_SKINTEAMS)
	return ++p;
}

qboolean OnSameTeam (edict_t *ent1, edict_t *ent2)
{
	char	ent1Team [512];
	char	ent2Team [512];

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
		return false;

	strcpy (ent1Team, ClientTeam (ent1));
	strcpy (ent2Team, ClientTeam (ent2));

	if (strcmp(ent1Team, ent2Team) == 0)
		return true;
	return false;
}


void SelectNextItem (edict_t *ent, int itflags)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;

	cl = ent->client;

	if (cl->chase_target) {
		ChaseNext(ent);
		return;
	}

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (cl->pers.selected_item + i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (!(it->flags & itflags))
			continue;

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void SelectPrevItem (edict_t *ent, int itflags)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;

	cl = ent->client;

	if (cl->chase_target) {
		ChasePrev(ent);
		return;
	}

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (cl->pers.selected_item + MAX_ITEMS - i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (!(it->flags & itflags))
			continue;

		cl->pers.selected_item = index;
		return;
	}

	cl->pers.selected_item = -1;
}

void ValidateSelectedItem (edict_t *ent)
{
	gclient_t	*cl;

	cl = ent->client;

	if (cl->pers.inventory[cl->pers.selected_item])
		return;		// valid

	SelectNextItem (ent, -1);
}


//=================================================================================

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (edict_t *ent)
{
	char		*name;
	gitem_t		*it;
	int			index;
	int			i;
	qboolean	give_all;
	edict_t		*it_ent;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	name = gi.args();
	
	
// TMF7 BEGIN GHOST MODE
	if ( Q_stricmp(gi.argv(1), "souls" ) == 0 )
	{
		if ( gi.argc() == 3 ) {
			gi.sound( ent, CHAN_ITEM, gi.soundindex( "soul/givesouls.wav" ), 1, ATTN_STATIC, 0 );
			ent->client->pool_of_souls = atoi(gi.argv(2));
		}
		else
			gi.cprintf( ent, PRINT_HIGH, "specify number of souls. example: give souls 120\n" );
		return;
	}
//TMF7 END GHOST MDE

	if (Q_stricmp(name, "all") == 0)
		give_all = true;
	else
		give_all = false;

	if (give_all || Q_stricmp(gi.argv(1), "health") == 0)
	{
		if (gi.argc() == 3)
			ent->health = atoi(gi.argv(2));
		else
			ent->health = ent->max_health;
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "weapons") == 0)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (!(it->flags & IT_WEAPON))
				continue;
			ent->client->pers.inventory[i] += 1;
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "ammo") == 0)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (!(it->flags & IT_AMMO))
				continue;
			Add_Ammo (ent, it, 1000);
		}
		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "armor") == 0)
	{
		gitem_armor_t	*info;

		it = FindItem("Jacket Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Combat Armor");
		ent->client->pers.inventory[ITEM_INDEX(it)] = 0;

		it = FindItem("Body Armor");
		info = (gitem_armor_t *)it->info;
		ent->client->pers.inventory[ITEM_INDEX(it)] = info->max_count;

		if (!give_all)
			return;
	}

	if (give_all || Q_stricmp(name, "Power Shield") == 0)
	{
		it = FindItem("Power Shield");
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem (it_ent, it);
		Touch_Item (it_ent, ent, NULL, NULL);
		if (it_ent->inuse)
			G_FreeEdict(it_ent);

		if (!give_all)
			return;
	}

	if (give_all)
	{
		for (i=0 ; i<game.num_items ; i++)
		{
			it = itemlist + i;
			if (!it->pickup)
				continue;
			if (it->flags & (IT_ARMOR|IT_WEAPON|IT_AMMO))
				continue;
			ent->client->pers.inventory[i] = 1;
		}
		return;
	}

	it = FindItem (name);
	if (!it)
	{
		name = gi.argv(1);
		it = FindItem (name);
		if (!it)
		{
			gi.cprintf (ent, PRINT_HIGH, "unknown item\n");
			return;
		}
	}

	if (!it->pickup)
	{
		gi.cprintf (ent, PRINT_HIGH, "non-pickup item\n");
		return;
	}

	index = ITEM_INDEX(it);

	if (it->flags & IT_AMMO)
	{
		if (gi.argc() == 3)
			ent->client->pers.inventory[index] = atoi(gi.argv(2));
		else
			ent->client->pers.inventory[index] += it->quantity;
	}
	else
	{
		it_ent = G_Spawn();
		it_ent->classname = it->classname;
		SpawnItem (it_ent, it);
		Touch_Item (it_ent, ent, NULL, NULL);
		if (it_ent->inuse)
			G_FreeEdict(it_ent);
	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	ent->flags ^= FL_NOTARGET;
	if (!(ent->flags & FL_NOTARGET) )
		msg = "notarget OFF\n";
	else
		msg = "notarget ON\n";

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f (edict_t *ent)
{
	char	*msg;

	if (deathmatch->value && !sv_cheats->value)
	{
		gi.cprintf (ent, PRINT_HIGH, "You must run the server with '+set cheats 1' to enable this command.\n");
		return;
	}

	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->movetype = MOVETYPE_WALK;
		msg = "noclip OFF\n";
	}
	else
	{
		ent->movetype = MOVETYPE_NOCLIP;
		msg = "noclip ON\n";
	}

	gi.cprintf (ent, PRINT_HIGH, msg);
}


/*
==================
Cmd_Use_f

Use an inventory item
==================
*/
void Cmd_Use_f (edict_t *ent)
{
	int			index;
	gitem_t		*it;
	char		*s;

	s = gi.args();
	it = FindItem (s);
	if (!it)
	{
		gi.cprintf (ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}
	if (!it->use)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	index = ITEM_INDEX(it);
	if (!ent->client->pers.inventory[index])
	{
		gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	it->use (ent, it);
}


/*
==================
Cmd_Drop_f

Drop an inventory item
==================
*/
void Cmd_Drop_f (edict_t *ent)
{
	int			index;
	gitem_t		*it;
	char		*s;

	s = gi.args();
	it = FindItem (s);
	if (!it)
	{
		gi.cprintf (ent, PRINT_HIGH, "unknown item: %s\n", s);
		return;
	}
	if (!it->drop)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	index = ITEM_INDEX(it);
	if (!ent->client->pers.inventory[index])
	{
		gi.cprintf (ent, PRINT_HIGH, "Out of item: %s\n", s);
		return;
	}

	it->drop (ent, it);
}


/*
=================
Cmd_Inven_f
=================
*/
void Cmd_Inven_f (edict_t *ent)
{
	int			i;
	gclient_t	*cl;

	cl = ent->client;

	cl->showscores = false;
	cl->showhelp = false;
	cl->showabilities = false;		//TMF7 GHOST MODE ( ghud )
	cl->showcollection = false;		//TMF7 GHOST MODE ( ghud )

	if (cl->showinventory)
	{
		cl->showinventory = false;
		return;
	}

	cl->showinventory = true;

	gi.WriteByte (svc_inventory);
	for (i=0 ; i<MAX_ITEMS ; i++)
	{
		gi.WriteShort (cl->pers.inventory[i]);
	}
	gi.unicast (ent, true);
}

/*
=================
Cmd_InvUse_f
=================
*/
void Cmd_InvUse_f (edict_t *ent)
{
	gitem_t		*it;

	ValidateSelectedItem (ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf (ent, PRINT_HIGH, "No item to use.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];
	if (!it->use)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not usable.\n");
		return;
	}
	it->use (ent, it);
}

/*
=================
Cmd_WeapPrev_f
=================
*/
void Cmd_WeapPrev_f (edict_t *ent)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;
	int			selected_weapon;

	cl = ent->client;

	if (!cl->pers.weapon)
		return;

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (selected_weapon + i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (! (it->flags & IT_WEAPON) )
			continue;
		it->use (ent, it);
		if (cl->pers.weapon == it)
			return;	// successful
	}
}

/*
=================
Cmd_WeapNext_f
=================
*/
void Cmd_WeapNext_f (edict_t *ent)
{
	gclient_t	*cl;
	int			i, index;
	gitem_t		*it;
	int			selected_weapon;

	cl = ent->client;

	if (!cl->pers.weapon)
		return;

	selected_weapon = ITEM_INDEX(cl->pers.weapon);

	// scan  for the next valid one
	for (i=1 ; i<=MAX_ITEMS ; i++)
	{
		index = (selected_weapon + MAX_ITEMS - i)%MAX_ITEMS;
		if (!cl->pers.inventory[index])
			continue;
		it = &itemlist[index];
		if (!it->use)
			continue;
		if (! (it->flags & IT_WEAPON) )
			continue;
		it->use (ent, it);
		if (cl->pers.weapon == it)
			return;	// successful
	}
}

/*
=================
Cmd_WeapLast_f
=================
*/
void Cmd_WeapLast_f (edict_t *ent)
{
	gclient_t	*cl;
	int			index;
	gitem_t		*it;

	cl = ent->client;

	if (!cl->pers.weapon || !cl->pers.lastweapon)
		return;

	index = ITEM_INDEX(cl->pers.lastweapon);
	if (!cl->pers.inventory[index])
		return;
	it = &itemlist[index];
	if (!it->use)
		return;
	if (! (it->flags & IT_WEAPON) )
		return;
	it->use (ent, it);
}

/*
=================
Cmd_InvDrop_f
=================
*/
void Cmd_InvDrop_f (edict_t *ent)
{
	gitem_t		*it;

	ValidateSelectedItem (ent);

	if (ent->client->pers.selected_item == -1)
	{
		gi.cprintf (ent, PRINT_HIGH, "No item to drop.\n");
		return;
	}

	it = &itemlist[ent->client->pers.selected_item];
	if (!it->drop)
	{
		gi.cprintf (ent, PRINT_HIGH, "Item is not dropable.\n");
		return;
	}
	it->drop (ent, it);
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f (edict_t *ent)
{
	if((level.time - ent->client->respawn_time) < 5)
		return;
	ent->flags &= ~FL_GODMODE;
	ent->health = 0;
	meansOfDeath = MOD_SUICIDE;
	player_die (ent, ent, ent, 100000, vec3_origin);
}

/*
=================
Cmd_PutAway_f
=================
*/
void Cmd_PutAway_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showscores = false;
	ent->client->showhelp = false;
	ent->client->showcollection = false;		//TMF7 GHOST MODE ( ghud )
	ent->client->showabilities = false;			//TMF7 GHOST MODE ( ghud )
}

int PlayerSort (void const *a, void const *b)
{
	int		anum, bnum;

	anum = *(int *)a;
	bnum = *(int *)b;

	anum = game.clients[anum].ps.stats[STAT_FRAGS];
	bnum = game.clients[bnum].ps.stats[STAT_FRAGS];

	if (anum < bnum)
		return -1;
	if (anum > bnum)
		return 1;
	return 0;
}

/*
=================
Cmd_Players_f
=================
*/
void Cmd_Players_f (edict_t *ent)
{
	int		i;
	int		count;
	char	small[64];
	char	large[1280];
	int		index[256];

	count = 0;
	for (i = 0 ; i < maxclients->value ; i++)
		if (game.clients[i].pers.connected)
		{
			index[count] = i;
			count++;
		}

	// sort by frags
	qsort (index, count, sizeof(index[0]), PlayerSort);

	// print information
	large[0] = 0;

	for (i = 0 ; i < count ; i++)
	{
		Com_sprintf (small, sizeof(small), "%3i %s\n",
			game.clients[index[i]].ps.stats[STAT_FRAGS],
			game.clients[index[i]].pers.netname);
		if (strlen (small) + strlen(large) > sizeof(large) - 100 )
		{	// can't print all of them in one packet
			strcat (large, "...\n");
			break;
		}
		strcat (large, small);
	}

	gi.cprintf (ent, PRINT_HIGH, "%s\n%i players\n", large, count);
}

/*
=================
Cmd_Wave_f
=================
*/
void Cmd_Wave_f (edict_t *ent)
{
	int		i;

	i = atoi (gi.argv(1));

	// can't wave when ducked
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
		return;

	if (ent->client->anim_priority > ANIM_WAVE)
		return;

	ent->client->anim_priority = ANIM_WAVE;

	switch (i)
	{
	case 0:
		gi.cprintf (ent, PRINT_HIGH, "flipoff\n");
		ent->s.frame = FRAME_flip01-1;
		ent->client->anim_end = FRAME_flip12;
		break;
	case 1:
		gi.cprintf (ent, PRINT_HIGH, "salute\n");
		ent->s.frame = FRAME_salute01-1;
		ent->client->anim_end = FRAME_salute11;
		break;
	case 2:
		gi.cprintf (ent, PRINT_HIGH, "taunt\n");
		ent->s.frame = FRAME_taunt01-1;
		ent->client->anim_end = FRAME_taunt17;
		break;
	case 3:
		gi.cprintf (ent, PRINT_HIGH, "wave\n");
		ent->s.frame = FRAME_wave01-1;
		ent->client->anim_end = FRAME_wave11;
		break;
	case 4:
	default:
		gi.cprintf (ent, PRINT_HIGH, "point\n");
		ent->s.frame = FRAME_point01-1;
		ent->client->anim_end = FRAME_point12;
		break;
	}
}

/*
==================
Cmd_Say_f
==================
*/
void Cmd_Say_f (edict_t *ent, qboolean team, qboolean arg0)
{
	int		i, j;
	edict_t	*other;
	char	*p;
	char	text[2048];
	gclient_t *cl;

	if (gi.argc () < 2 && !arg0)
		return;

	if (!((int)(dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
		team = false;

	if (team)
		Com_sprintf (text, sizeof(text), "(%s): ", ent->client->pers.netname);
	else
		Com_sprintf (text, sizeof(text), "%s: ", ent->client->pers.netname);

	if (arg0)
	{
		strcat (text, gi.argv(0));
		strcat (text, " ");
		strcat (text, gi.args());
	}
	else
	{
		p = gi.args();

		if (*p == '"')
		{
			p++;
			p[strlen(p)-1] = 0;
		}
		strcat(text, p);
	}

	// don't let text be too long for malicious reasons
	if (strlen(text) > 150)
		text[150] = 0;

	strcat(text, "\n");

	if (flood_msgs->value) {
		cl = ent->client;

        if (level.time < cl->flood_locktill) {
			gi.cprintf(ent, PRINT_HIGH, "You can't talk for %d more seconds\n",
				(int)(cl->flood_locktill - level.time));
            return;
        }
        i = cl->flood_whenhead - flood_msgs->value + 1;
        if (i < 0)
            i = (sizeof(cl->flood_when)/sizeof(cl->flood_when[0])) + i;
		if (cl->flood_when[i] && 
			level.time - cl->flood_when[i] < flood_persecond->value) {
			cl->flood_locktill = level.time + flood_waitdelay->value;
			gi.cprintf(ent, PRINT_CHAT, "Flood protection:  You can't talk for %d seconds.\n",
				(int)flood_waitdelay->value);
            return;
        }
		cl->flood_whenhead = (cl->flood_whenhead + 1) %
			(sizeof(cl->flood_when)/sizeof(cl->flood_when[0]));
		cl->flood_when[cl->flood_whenhead] = level.time;
	}

	if (dedicated->value)
		gi.cprintf(NULL, PRINT_CHAT, "%s", text);

	for (j = 1; j <= game.maxclients; j++)
	{
		other = &g_edicts[j];
		if (!other->inuse)
			continue;
		if (!other->client)
			continue;
		if (team)
		{
			if (!OnSameTeam(ent, other))
				continue;
		}
		gi.cprintf(other, PRINT_CHAT, "%s", text);
	}
}

void Cmd_PlayerList_f(edict_t *ent)
{
	int i;
	char st[80];
	char text[1400];
	edict_t *e2;

	// connect time, ping, score, name
	*text = 0;
	for (i = 0, e2 = g_edicts + 1; i < maxclients->value; i++, e2++) {
		if (!e2->inuse)
			continue;

		Com_sprintf(st, sizeof(st), "%02d:%02d %4d %3d %s%s\n",
			(level.framenum - e2->client->resp.enterframe) / 600,
			((level.framenum - e2->client->resp.enterframe) % 600)/10,
			e2->client->ping,
			e2->client->resp.score,
			e2->client->pers.netname,
			e2->client->resp.spectator ? " (spectator)" : "");
		if (strlen(text) + strlen(st) > sizeof(text) - 50) {
			sprintf(text+strlen(text), "And more...\n");
			gi.cprintf(ent, PRINT_HIGH, "%s", text);
			return;
		}
		strcat(text, st);
	}
	gi.cprintf(ent, PRINT_HIGH, "%s", text);
}

//TMF7 BEGIN GHOST MODE

// 'f' is bound to ghost mode toggle ( husk spawn ), and host obliteration
void Cmd_Ghost_f( edict_t *ent ) {

	if ( !( ent->client->hostmode ) ) {
		ent->client->ghostmode = !ent->client->ghostmode;

	} else if ( ent->client->hostmode && ent->client->soul_abilities & OBLITERATE_HOST ) { DropHost( ent, HOST_KILL ); }		
		
	// clear out the husk
	if ( !(ent->client->ghostmode || ent->client->hostmode) && ent->client->player_husk 
		&& ent->client->player_husk->classname && !Q_strncasecmp( ent->client->player_husk->classname, "husk", 4 ) 
		&& ent->husktouch ) {
		ent->husktouch( ent, ent->client->player_husk );
	}

	if ( ent->client->ghostmode && ( !ent->client->player_husk 
		|| ( ent->client->player_husk->classname && Q_strncasecmp( ent->client->player_husk->classname, "husk", 4 ) ) ) ) 
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

// 'm' is bound to "ghost_fly" toggle
void Cmd_Ghost_Fly_f( edict_t *ent ) {
		
	if ( !(ent->client->soul_abilities & GHOST_FLY) )
		return;
}

// 'mouse3' is bound to radial monster push, and host-speak
void Cmd_Push_Beasts_f( edict_t *ent ) {

	int pushCount;
	vec3_t dir;
	edict_t *other;

	// passive
	if ( !(ent->client->soul_abilities & PUSH_BEASTS ) )
		return;

	if ( !ent->client->hostmode ) {

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
	} else {
/*
		// uberhost
		hi 	= get targeted monster's attention 											( shift+shoot w/o follower )
		chuckle = no targetd monster to say "hi" to
		bleh	= didn't give attentive monster an order ( time limit )

		help	= recruit all monsters to fight current host's enemy ( not follow ) 	( 'mouse3' )
		what	= host doesn't have an enemy when trying "help" sligspeak

		getem	= tell attentive monster who to attack 									( shift+shoot monster )
		hereboy = tell attentive monster to permanently follow 							( shift+shoot world )
		stay	= tell current follower to go away ( break follow ) 					( shift+mouse3 )

		lookout = causes all nearby monsters to start running ( breaking hiding )		( alt+mouse3 )

		freeze	= tell specific follower to stay until ordered otherwise 				( shift+shoot follower )

		highbuzz/lowbuzz = made when host's enemy dies ( a few times on a random loop )**** also in rodeo ******
*/
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

// 'y' is bound to shield of souls refresh
void Cmd_Soul_Shield_f( edict_t *ent ) {

	// passive
	if ( !(ent->client->soul_abilities & SOUL_SHIELD ) )
		return;
	
	// spawn a bunch of svc_tempentity/edict_t to "orbit"/orbit the owner ( player or husk ) ( from the current number to max_orbiting )
	// if there is any health left in the soul_shield then have it take the hit ( dont carry over extra damage )
	// give it a pain and die function ( which reduces the number orbiting and makes separate pain and die noises in the players ear )
	// no active shield transfers damage as normal
}


transform_t transforms[] =
{
	// 1 soul required / 1 cost
	{ "monster_soldier_light",	1 },
	{ "monster_soldier",		1 },
	{ "monster_soldier_ss",		1 },
	{ "monster_infantry",		1 },

	// 10 souls required / 10 cost
	{ "monster_gunner",			2 },
	{ "monster_berserk",		2 },
	{ "monster_parasite",		2 },
	{ "monster_flyer",			2 },
	{ "monster_flipper",		2 },

	// 20 souls required / 10 cost
	{ "monster_chick",			3 },
	{ "monster_floater",		3 },
	{ "monster_hover",			3 },
	{ "monster_mutant",			3 },

	// 30 souls required / 10 cost
	{ "monster_tank_commander",	4 },
	{ "monster_tank",			4 },
	{ "monster_gladiator",		4 },
	{ "monster_medic",			4 },
	{ "monster_brain",			4 },

	// 40 souls required / 10 cost
	{ "monster_supertank",		5 },
	{ "monster_boss2",			5 },
	{ "monster_boss3_stand",	5 },
	{ "monster_jorg",			5 },
	{ NULL,					 NULL }
};

// 'o' is bound to host transformation/upgrade
void Cmd_Transform_Host_f( edict_t *ent ) {

	int i, num;
	int oldRank, desiredRank;
	qboolean transformOk;
	char *newClassname;
	transform_t *trans;
	edict_t *host;
	
	// passive
	if ( !(ent->client->soul_abilities & TRANSFORM_HOST) )
		return;

	host = ent->client->host;

	if ( !host )
		return;

	for ( trans = transforms; trans->host_classname; trans++ ) {

		if ( !strcmp( trans->host_classname, host->classname ) ) {
			// found it
			oldRank = trans->host_rank;
		}
	}

	transformOk = false;
	desiredRank = oldRank;

	// enough to upgrade and desire to upgrade?
	if ( ent->client->pool_of_souls 
		&& ent->client->pool_of_souls%10 
		&& ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ALT) ) 
		{ desiredRank++; }

	if ( desiredRank == 1 && ent->client->pool_of_souls ) { transformOk = true; }
	else if ( desiredRank == 2 && ent->client->pool_of_souls >= 10  ) { transformOk = true; }
	else if ( desiredRank == 3 && ent->client->pool_of_souls >= 20 ) { transformOk = true; }
	else if ( desiredRank == 4 && ent->client->pool_of_souls >= 30 ) { transformOk = true; }
	else if ( desiredRank == 5 && ent->client->pool_of_souls >= 40 ) { transformOk = true; }

	if ( transformOk ) {

		newClassname = NULL;
		while ( !newClassname ) {

			num = rand()%5; // not all ranks have five, hence the while loop
			for ( trans = transforms; trans->host_classname; trans++ ) {

				if ( trans->host_rank == desiredRank && ((trans-transforms)+1)%(num+1) == 0 ) {
					newClassname = trans->host_classname;
					break;
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

	host->spawnflags = 0;
	host->monsterinfo.aiflags = 0;
	host->target = NULL;
	host->targetname = NULL;
	host->combattarget = NULL;
	host->deathtarget = NULL;
	ED_CallTransformSpawn ( host, newClassname );
	if ( host->think ) {
		host->nextthink = level.time;
		host->think( host );
	}
	gi.sound (ent, CHAN_AUTO, gi.soundindex( "slighost/hosttransform.wav" ), 1, ATTN_NORM, 0);
	for ( i = 0; i < 5; i++)
			ThrowGib (host, "models/objects/gibs/sm_meat/tris.md2", 100, GIB_ORGANIC);

	TakeHost( ent, host, HOST_TRANSFORM );
}

//TMF7 END GHOST MODE


/*
=================
ClientCommand
=================
*/
void ClientCommand (edict_t *ent)
{
	char	*cmd;

	if (!ent->client)
		return;		// not fully in game yet

	cmd = gi.argv(0);

	if (Q_stricmp (cmd, "players") == 0)
	{
		Cmd_Players_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "say") == 0)
	{
		Cmd_Say_f (ent, false, false);
		return;
	}
	if (Q_stricmp (cmd, "say_team") == 0)
	{
		Cmd_Say_f (ent, true, false);
		return;
	}
	if (Q_stricmp (cmd, "score") == 0)
	{
		Cmd_Score_f (ent);
		return;
	}
	if (Q_stricmp (cmd, "help") == 0)
	{
		Cmd_Help_f (ent);
		return;
	}
// TMF7 BEGIN GHOST MODE
	if (Q_stricmp (cmd, "soul_abilities") == 0)
	{
		Cmd_Soul_Abilities_f (ent);
		return;
	}	
	if (Q_stricmp (cmd, "soul_collection") == 0)
	{
		Cmd_Soul_Collection_f (ent);
		return;
	}
// TMF7 END GHOST MODE

	if (level.intermissiontime)
		return;

	if (Q_stricmp (cmd, "use") == 0)
		Cmd_Use_f (ent);
	else if (Q_stricmp (cmd, "drop") == 0)
		Cmd_Drop_f (ent);
	else if (Q_stricmp (cmd, "give") == 0)
		Cmd_Give_f (ent);
	else if (Q_stricmp (cmd, "god") == 0)
		Cmd_God_f (ent);
	else if (Q_stricmp (cmd, "notarget") == 0)
		Cmd_Notarget_f (ent);
	else if (Q_stricmp (cmd, "noclip") == 0)
		Cmd_Noclip_f (ent);
	else if (Q_stricmp (cmd, "inven") == 0)
		Cmd_Inven_f (ent);
	else if (Q_stricmp (cmd, "invnext") == 0)
		SelectNextItem (ent, -1);
	else if (Q_stricmp (cmd, "invprev") == 0)
		SelectPrevItem (ent, -1);
	else if (Q_stricmp (cmd, "invnextw") == 0)
		SelectNextItem (ent, IT_WEAPON);
	else if (Q_stricmp (cmd, "invprevw") == 0)
		SelectPrevItem (ent, IT_WEAPON);
	else if (Q_stricmp (cmd, "invnextp") == 0)
		SelectNextItem (ent, IT_POWERUP);
	else if (Q_stricmp (cmd, "invprevp") == 0)
		SelectPrevItem (ent, IT_POWERUP);
	else if (Q_stricmp (cmd, "invuse") == 0)
		Cmd_InvUse_f (ent);
	else if (Q_stricmp (cmd, "invdrop") == 0)
		Cmd_InvDrop_f (ent);
	else if (Q_stricmp (cmd, "weapprev") == 0)
		Cmd_WeapPrev_f (ent);
	else if (Q_stricmp (cmd, "weapnext") == 0)
		Cmd_WeapNext_f (ent);
	else if (Q_stricmp (cmd, "weaplast") == 0)
		Cmd_WeapLast_f (ent);
	else if (Q_stricmp (cmd, "kill") == 0)
		Cmd_Kill_f (ent);
	else if (Q_stricmp (cmd, "putaway") == 0)
		Cmd_PutAway_f (ent);
	else if (Q_stricmp (cmd, "wave") == 0)
		Cmd_Wave_f (ent);
	else if (Q_stricmp(cmd, "playerlist") == 0)
		Cmd_PlayerList_f(ent);
//TMF7 BEGIN GHOST MODE
	else if ( Q_stricmp(cmd, "uber") == 0 )
		Cmd_Uber_f( ent );
	else if ( Q_stricmp(cmd, "ghost") == 0 )
		Cmd_Ghost_f( ent );
	else if ( Q_stricmp(cmd, "inhabit") == 0 )	
		Cmd_Inhabit_f( ent );
	else if ( Q_stricmp(cmd, "detect_life") == 0 )	
		Cmd_Detect_Life_f( ent );
	else if ( Q_stricmp(cmd, "ghost_fly") == 0 )	
		Cmd_Ghost_Fly_f( ent );
	else if ( Q_stricmp(cmd, "push_beasts") == 0 )	
		Cmd_Push_Beasts_f( ent );
	else if ( Q_stricmp(cmd, "soul_shield") == 0 )
		Cmd_Soul_Shield_f( ent );
	else if ( Q_stricmp(cmd, "transform") == 0 )
		Cmd_Transform_Host_f( ent );
	else if ( Q_stricmp(cmd, "soullevel") == 0 )	
		Cmd_Set_Soul_Level_f( ent );
//TMF7 END GHOST MODE
	else	// anything that doesn't match a command will be a chat
		Cmd_Say_f (ent, false, true);
}
