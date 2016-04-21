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

void soldier_attack1	( edict_t *self ); // s & sl
void soldier_attack2	( edict_t *self ); // s & sl
void soldier_attack3	( edict_t *self ); // all, duck-attack
void soldier_attack4	( edict_t *self ); // ss
void soldier_attack6	( edict_t *self ); // all on-sight

// skill level dependent
void soldier_duck		( edict_t *self );


//*************
//   INFANTRY
//*************




/////////////////////////////////////////////
// per-monster currentmove callback arrays //
/////////////////////////////////////////////

//*************
//   BERSERK
//*************

hmove_t berserk[] =
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

//*************
//   GLADIATOR
//*************

hmove_t gladiator[] =
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

//*************
//   GUNNER
//*************

hmove_t gunner[] =
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

//*************
//   SOLDIER
//*************

// give all three soldiers the same physical moves
hmove_t soldier[] =
{
	{	"stand1",				soldier_stand1		},	// regular stand
	{	"stand2",				soldier_stand3		},	// regular stand ( perk up )
	{	"walk1",				soldier_walk1		},	// regualr walk ( pause and look )
	{	"walk2",				soldier_walk2		},	// regualr walk
	{	"run_start",			soldier_start_run	},	// regular run startup
	{	"run",					soldier_runp		},	// regular run
	{	"attack1",				soldier_attack1		},  // stand attack ( arms in )
	{	"attack2",				soldier_attack2		},	// stand attack ( arm outstretched )
	{	"duck_attack",			soldier_attack3		},	// duck attack
	{	"attack4",				soldier_attack4		},	// stand attack ( small step back )
	{	"run_attack",			soldier_attack6		},	// run attack
	{	"duck",					soldier_duck		},	// regular duck
	{	NULL,					NULL				}
};

//*************
//   TANK
//*************

hmove_t tank[] =
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

//*************
//   MEDIC
//*************

hmove_t medic[] =
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

//*************
//   FLIPPER
//*************

hmove_t flipper[] =
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

//*************
//   CHICK
//*************

hmove_t chick[] =
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

//*************
//   PARASITE
//*************

hmove_t parasite[] =
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

//*************
//   FLYER
//*************

hmove_t flyer[] =
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

//*************
//   BRAIN
//*************

hmove_t brain[] =
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

//*************
//   FLOATER
//*************

hmove_t floater[] =
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

//*************
//   HOVER
//*************

hmove_t hover[] =
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

//*************
//   MUTANT
//*************

hmove_t mutant[] =
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

//*************
//   SUPERTANK
//*************

hmove_t supertank[] =
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

//*************
//   BOSS2 ( hornet )
//*************

hmove_t boss2[] =
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

//*************
//   BOSS3_STAND ( makron )
//*************

hmove_t boss3_stand[] =
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

//*************
//   JORG
//*************

hmove_t jorg[] =
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