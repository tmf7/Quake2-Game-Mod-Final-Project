# About Soul Collector

This is a Quake 2 mod that includes code edits to to gamex86.dll, quake2.exe, and adds several new sound assets. At its core Soul Collector allows the player to step into the "ghost plane" leaving their body behind (and vulnerable). As a ghost, the player is immortal, cannot interact with the environment, and cannot be seen or heard by the Strogg. Killing the Strogg strands their souls in the ghost plane, where the player can collect them and gain new abilities. Collecting enough souls grants the ability to possess and control the Strogg. At first the player's control of the possessed is weak, but further soul collection gives the player direct control of the Strogg they possess, pulling that itchy trigger finger at will. The strongest soul collector can even make the possessed speak to other Strogg, recruit them as followers, and order them to attack! 

This is only a brief taste of what Soul Collector offers, read on, watch the video summary, or play it yourself to get lost in the soul plane!



## Video Summary

[Video Summary of Soul Collector](https://www.youtube.com/watch?v=M8dwFSS6Fao)



## Soul Collection System:

When the Strogg are killed they leave behind their animated souls, constantly replaying their last actions in life, and moaning their lamentations across all planes of existence. As a soul collector the player gains the ability to leave their corporeal form behind, become a ghost, see souls and collect them to gain new abilities, as listed below.

### New Modes of Play

There are four modes of play in Soul Collector, each granting unique and shared abilities. This is a high-level description of each mode.

| Mode         | Description                                                  |
| ------------ | ------------------------------------------------------------ |
| Corporeal    | The usual body-bound mode of play where the player can shoot guns, open doors, alter and attack the Strogg, and so on. |
| Ghost        | The player leaves their body behind, risking it being attacked by nearby enemies, downing, being poisoned, or crushed. However, they become an invulnerable invisible ghost visible only to other ghosts (players and dead Strogg). A ghost cannot operate any gun or machine in the living world, but can collect souls and possess Strogg. If the player's body dies while in ghost mode, the player dies and must load their last save. |
| Host (Rodeo) | The ghost-player possesses a Strogg and can order it to run to specific locations, and attack other Strogg. If the possessed Strogg dies the player returns to their ghost form. |
| Host (Uber)  | The ghost-player possesses a Strogg and can fully control how and where their host runs, and when and where it attacks. In addition, the player has the ability to talk directly and indirectly to other Strogg, gaining a follower and giving orders. If the possessed Strogg dies the player returns to their ghost form. |



### New Abilities

![SoulCollector_SkillTree](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_SkillTree.png)

#### Level 1

| Ability               | Description                                                  |
| --------------------- | ------------------------------------------------------------ |
| Enter Soul Plane      | Actively enter the soul plane, leaving the player's body behind and become a ghost |
| Warp Soul to Body     | The ghost-player will teleport back to the location where they left their body |
| Collect Touched Souls | Touching the soul of  a dead Strogg will automatically collect it, stronger Strogg produce more powerful souls that quickly increase the player's soul collector rank |
| Drain Enemy Life      | If the ghost-player touches a Strogg its life will be drained in increments over time, until the Strogg dies or the ghost-player stops touching the Strogg |

#### Level 2

| Ability             | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| Touch Possession    | If the ghost-player touches any Strogg, they will possess that Strogg and continue play and control from that Strogg's perspective |
| Rodeo-Host Controls | Point-and-click control of possessed Strogg (aka "host"). Clicking on the environment will order the host to run there. Clicking on a Strogg will order the host to chase and attack it. *Note: Rodeo-Host Controls are available for all Strogg types.* |
| Release Host        | The player will leave the possessed Strogg (aka "host") unharmed and re-enter the soul plane |

#### Level 3

| Ability             | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| Pull Nearby Souls   | The ghost-player will pull all souls within a fixed radius toward themselves, Strogg souls will jump towards the player for collection rather than requiring the player to walk over to each soul to collect them |
| Targeted Possession | If the ghost-player (fire)s at a targeted Strogg within line-of-sight, then that Strogg will be possessed and play and control will continue from that Strogg's perspective |
| Detect Life         | In all modes, if the player triggers this, then a green line will draw from the heart of the player to the heart of all nearby Strogg, even if not directly visible through solid objects |
| Uber-Host Controls  | Direct control of possessed Strogg (aka "host"). The player controls the host just as they would control their own body, running and shooting. However, Uberhosts also have the ability to speak and recruit followers (discussed in the "Recruit Followers" ability) |

#### Level 4

| Ability                       | Description                                                  |
| ----------------------------- | ------------------------------------------------------------ |
| Radial Possession             | If the ghost-player triggers this ability then the nearest Strogg will be possessed, regardless if any line-of sight is available, and play will continue from that Strogg's perspective |
| Shield of Souls               | In all modes, if the player **expends souls** to trigger this, then an orbiting wall of souls will appear and make the player immune to physical damage. The summoned souls are only visible in the ghost plane. Once the summoned souls have absorbed a fixed amount of damage, the player will resume taking physical damage. |
| Throw-back All Nearby Enemies | In corporeal mode, the player can trigger a telekinetic force that knocks back all Strogg within a fixed radius of the player, temporarily stunning them |
| Obliterate Host               | If the player has possessed a Strogg (aka "host") in Rodeo or Uber mode, then they can leave that host peacefully, or chose to gibb the host, when returning to the ghost plane |
| All Damage Transfer to Host   | In soul collector levels 1 to 3 if the possessed Strogg (aka "host") was damaged then all damage transferred to the player's body, leaving the host unharmed and potentially killing the player. However, with this passive ability the host now takes all damage directly, leaving the player's body unharmed. If the host dies, then the player returns to the ghost plane. |
| Recruit Followers             | In Host Mode (Uber) the player can force the possessed Strogg (aka "host") to speak. The host can say "hi", order one other Strogg to move to a specific location and/or attack other Strogg. The host can also yell for help if being attacked an all Strogg within shouting distance will chase and kill the host's enemy. Lastly, the host can release a follower by saying "stay", and also call for a general alert that freaks out all nearby Strogg by saying "look out!" The specific controls of this ability are detailed in the **Soul Collector Controls** section below |

#### Level 5

| Ability           | Description                                                  |
| ----------------- | ------------------------------------------------------------ |
| Warp Body to Soul | If the ghost-player triggers this ability, then the player's body will teleport to the location of the player's ghost |
| Transform Host    | If the player has possessed a Strogg (aka "host"), then the player can **expend souls** to transform that host to another Strogg of equal or greater Strogg-Rank, all they way up to boss-level Strogg while retaining host control |

In Addition, there are a few cooldowns to balance play:

* Souls can only be collected after a fixed duration after a Strogg dies (give them time to lament their death before your scoop up their souls!)
* After leaving a possessed host, the ghost-player will be unable to possess another Strogg for a short duration



### New Co-operative Multiplayer Styles

My mod is fully compatible with server-client co-op multiplayer. Multiple players can install and run this Soul Collector mod and play as together co-operatively through the entire game. 

With the new Soul Collection system and abilities co-op multiplayer can even become competitive. As a few co-op mode competition examples:

* Compete to see who can collect the most souls
* Prop-hunt to find the Strogg a player has possessed
* Husk hide-and-go-kill, hide your body someplace safe, possess a Strogg and go kill the other players' husks before they kill yours! Kill possessed Strogg to slow down your opponents!

### New GUI

There are several new context-sensitive control-scheme GUIs, as well as a new GUI that summarizes how many of each type of soul the player has collected. Each of these new GUI's are slowly filled as the player collects souls and ranks up. The "rank-up" GUI appears whenever the player collects enough souls to unlock new abilities.



#### Corporeal Controls GUI

![CorporealGUI_Low](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/CorporealGUI_Low.png) :arrow_right:       ![CorporealGUI_Low](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/CorporealGUI.png)  

*Note how the list of abilities becomes more complex as soul collector rank increases from 1 to 5*



#### Ghost Controls GUI

![GhostGUI_Low](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/GhostGUI_Low.png) :arrow_right:       ![GhostGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/GhostGUI.png)

*Note how the list of abilities becomes more complex as soul collector rank increases from 1 to 5*



#### Host (Rodeo) Controls GUI

![RodeoHostGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/HostRodeoGUI_Low.png) :arrow_right:       ![RodeoHostGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/HostRodeoGUI.png)

*Note how the list of abilities becomes more complex as soul collector rank increases from 2 to 5*



#### Host (Uber) Controls GUI

![UberHostGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/HostUberGUI_Low.png)  :arrow_right:       ![UberHostGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/HostUberGUI.png)

*Note how the list of abilities becomes more complex as soul collector rank increases from 3 to 5*



#### Soul Collection GUI

![CollectionGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollectionGUI_Low.png) :arrow_right:       ![CollectionGUI](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollectionGUI.png)

*Note how this list starts empty and becomes populated as each Strogg soul is collected*



### New Cheat Codes

There are three new cheat codes added to Quakes console command cheats. Open the command console with the **~ key**

| Console Command | Example         | Effect                                                       |
| --------------- | --------------- | ------------------------------------------------------------ |
| soullevel [1-5] | soullevel 3     | Sets the player's Soul Collector level                       |
| give souls [#]  | give souls 120  | Set the player's current soul count                          |
| give collection | give collection | Populates the players soul collection with 3 of every type of soul and adds an equivalent number of souls to their soul count |



# Run Soul Collector

1. Create a backup of vanilla quake. If you wish to remove the Soul Collector mod from your install, this backup will restore your copy of vanilla Quake 2. ***Only the following backups need be made:***
   * Create a new folder called **Quake2_Backup** someplace safe on your computer
   * Navigate to the root directory of your legitimate install of Quake 2 and copy or move the following file over to **Quake2_Backup**: { **quake2.exe** }
   * Inside **Quake2_Backup** create another folder called **baseq2**
   * Within the root directory of your legitimate install of Quake 2, navigate into **baseq2** and copy or move the following files over to **Quake2_Backup/baseq2**: { **gamex86.dll**, **config.cfg** } 
   * **If resorting to vanilla Quake 2,** copy both backups of **baseq2** and **quake2.exe** into your legitimate install of Quake 2 at the root directory, and accept all file overwrite and folder merging messages.

2. Download and unzip [**Freehill_SoulCollector_Quake2_Mod.zip**](www.google.com) using the "Download" button, it contains one folder **baseq2**, and one file **quake2.exe**. Both are uniquely modified and assembled by me and both are necessary for Soul Collector to work as designed.

3. Copy both of Soul Collector's **baseq2** and **quake2.exe** into your legitimate install of Quake 2 at the root directory. Accept all file overwrite and folder merging messages, this is why you made a backup.

4. Run **quake2.exe**

5. Load a new game as normal, you'll know the mod is running when you begin play from a 3rd-person perspective and your soul collector rank popup is active. Press escape, or F7, to close the popup.

   ![SoulCollector_Loaded](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_Loaded.png)



# Soul Collector Controls

The keybindings listed here are in addition to Quake 2's vanilla keybindings. Below are controls for the four (4) separate modes of play: corporeal, ghost, host (rodeo), and host (uber) as explained above in the **Soul Collection System** section. 

***Important Note: A list of all controls is accessible with the F7 key. Controls will automatically display according to the user's current mode.*** As the player's soul collector rank increases, more of these controls and abilities become available, as explained above in the **Soul Collection System** section.



### Corporeal Mode

| Keybinding | Function                                                     |
| ---------- | ------------------------------------------------------------ |
| mouse3     | push beasts, knocks back all Strogg within a fixed radius of the player and temporarily stuns them |
| f          | gost-walk, step into the ghost plane, leaving your body (aka "husk") behind and enabling  ghost abilities and controls (GHOST MODE TOGGLE) |
| n          | detect life toggle, when enabled draws a green line from the player's heart to the heart of all Strogg within a fixed radius of the player, even through solid objects |
| y          | shield of souls, surrounds the player's body with souls only visible in the ghost plane which protect the player's body from a fixed amount of damage |

![SoulCollector_BodyLevel5](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_BodyLevel5.png)



### Ghost Mode

| Keybinding            | Function                                                     |
| --------------------- | ------------------------------------------------------------ |
| mouse1 or ctrl (fire) | targeted possession, possesses the nearest Strogg the player is currently aimed at, if any |
| shift + f             | warp body to ghost, teleports the player's body (aka "husk") to the player's current ghost location |
| f                     | return to body, teleports the player's ghost to the player's body (aka "husk") |
| v                     | drain life toggle, when enabled all Strogg currently in contact with the player's ghost will have their health drained, when disabled no life is drained and soul collector's of rank 2 and above will instead possess touched enemies |
| n                     | detect life toggle, when enabled draws a green line from the player's heart to the heart of all Strogg within a fixed radius of the player, even through solid objects |
| y                     | shield of souls, surrounds the player's body with souls only visible in the ghost plane which protect the player's body from a fixed amount of damage |

![SoulCollector_SoulLevel5](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_GhostLevel5.png)



### Host Mode (Rodeo)

| Keybinding            | Function                                                     |
| --------------------- | ------------------------------------------------------------ |
| mouse1 or ctrl (fire) | set chase target <br/>**if aimed at the environment** the possessed enemy (aka "host") will move to the selected location (highlighted with a green sphere)<br/> **if aimed at an enemy (or player)** the host will chase and attack the selected target |
| r                     | release host, the possessed Strogg (aka "host") will be released from possession (without harm) and the player will return to the ghost plane (GHOST MODE) |
| f                     | obliterate host, the possessed Strogg (aka "host") will be released from possession and gibbed (killed) as the player returns to the ghost plane (GHOST MODE) |
| v                     | uberhost toggle, if enabled the player will take complete control of all the possessed Strogg's (aka "host's") actions (walking, running, shooting, and speaking to other enemies). *Note: this mode is currently only available for the Soldier, however it can be added for all Strogg types.* |
| n                     | detect life toggle, when enabled draws a green line from the player's heart to the heart of all Strogg within a fixed radius of the player, even through solid objects |
| o                     | transform host, changes the possessed Strogg (aka "host") into another Strogg of the same rank. *Notes: this action expends collected souls, and will not revert control to HOST MODE (RODEO) if HOST MODE (UBER) is not available, so remember to manually **toggle uberhost off with the v key*** |
| alt + o               | upgrade host, changes the possessed Strogg (aka "host") into another Strogg one rank above its own. *Note: this action expends collected souls* |
| y                     | shield of souls, surrounds the player's body with souls only visible in the ghost plane which protect the player's body from a fixed amount of damage |

![SoulCollector_RodeoHostLevel5](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_RodeoHostLevel5.png)



### Host Mode (Uber)

| Keybinding            | Function                                                     |
| --------------------- | ------------------------------------------------------------ |
| mouse1 or ctrl (fire) | attack                                                       |
| r                     | release host, the possessed Strogg (aka "host") will be released from possession (without harm) and the player will return to the ghost plane (GHOST MODE) |
| f                     | obliterate host, the possessed Strogg(aka "host") will be released from possession and gibbed (killed) as the player returns to the ghost plane (GHOST MODE) |
| v                     | uberhost toggle, if enabled the player will take complete control of all the possessed enemy's (aka "host's") actions (walking, running, shooting, and speaking to other enemies). *Note: this mode is currently only available for the Soldier, however it can be added for all Strogg types.* |
| n                     | detect life toggle, when enabled draws a green line from the player's heart to the heart of all enemies within a fixed radius of the player, even through solid objects |
| o                     | transform host, changes the possessed Strogg (aka "host") into another Strogg of the same rank. *Notes: this action expends collected souls, and will not revert control to HOST MODE (RODEO) if HOST MODE (UBER) is not available, so remember to manually **toggle uberhost off with the v key*** |
| alt + o               | upgrade host, changes the possessed Strogg (aka "host") into another Strogg one rank above its own. *Note: this action expends collected souls* |
| y                     | shield of souls, surrounds the player's body with souls only visible in the ghost plane which protect the player's body from a fixed amount of damage |
| **HOST SPEAK**        | **SEE THE HOST SPEAK TABLE BELOW**                           |

![SoulCollector_UberHostLevel5](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_UberHostLevel5.png)



#### Host Speak

| Keybinding                                            | Function                                                     |
| ----------------------------------------------------- | ------------------------------------------------------------ |
| shift + mouse1<br/>or shift + ctrl<br/>shift + (fire) | follower control, collect a follower and give it orders, see **Host speak shift + (fire) Actions** table below for specifics |
| shift + mouse3                                        | free follower, possessed Strogg (aka "host) will say **"Stay."** and release the current follower from any further orders from the player, the follower will resume its normal duties |
| alt + mouse3                                          | shout an alert, the possessed Strogg (aka "host") will shout **"Look out!"** to all nearby Strogg and send them into an active seek-and-destroy state with nothing to chase or attack (just to mess with them) |
| mouse3                                                | call for help, **if the possessed Strogg (aka "host") is being attacked** then the host will shout **"Help!"** and all nearby Strogg not attacking the host will target the host's enemy, chase and kill it, then resume their normal duties |

![SoulCollector_Follower](https://github.com/tmf7/Quake2-Game-Mod-Final-Project/blob/soul_final/readme_images/SoulCollector_Follower.png)



##### Host Speak shift + (fire) Actions

| Shift + (fire) State | Conditional Result                                           |
| -------------------- | ------------------------------------------------------------ |
| No follower          | **if pointed at a Strogg** say **"Hi."** and hear "Hi." back to begin cooldown to gain a follower, if "Hi." cooldown expires the Strogg will verbalize their distain and will not follow until you say "Hi." again<br/>**if pointed at the environment** say **"What?"** and various other confused verbalizations with no other effect<br/> **after saying "Hi."** point anywhere and (fire) to lock the Strogg you've said **"Hi."** to as your follower |
| Follower             | **if pointed at a Strogg** the host will say **"Get 'em!"** and the follower will chase and attack that Strogg until they or the enemy Strogg dies<br/>**if pointed at the environment** the host will say "Here boy!" and the follower will run to the point you've (fire)d at and stop when they reach it<br/>**rapid double-(fire)** to say **"Here boy!"** and order your follower to follow you at all times. *Note: you must still explicitly order a direct attack with **(fire)** or shout for help with **mouse3** to have a follower in this state attack* |



# Moding my mod in Visual Studio

1. *(optional)* Make your code edits, if you like, just know that building the "soul_final" branch of my mod will not output vanilla Quake 2. It will output my dll-side and exe-side code modifications as used by my Soul Collector mod. 
   - *(optional)* You can also add various new resource files, or modify the configuration files (for eaxmaple, config.cfg)
     - For example, search for **.wav** in visual studio to see how sound files are hardcoded for registration and use. Some can be found in cl_fx.c, but they're peppered all over. 
     - I will make no remark on id's decision to hardcode resource filenames, they were busy revolutionizing 3D rendering on PC with things like the [fast inverse square root function](https://www.youtube.com/watch?v=p8u_k2LIZyo) (Quake 3).
2. Set the Configuration to **Release** and the Platform to **Win32**
3. Build the entire solution, paying special attention to the output of the **game** and **quake2** projects, they generate the **gamex86.dll** and **quake2.exe** files, respectively.
   * If you have trouble building the visual studio solution make sure you have the MFC that matches what the solution is targeting. Quake 2 requires MFC (Microsoft Foundation Classes) to build under Windows, which you may be missing. You can get it using the Visual Studio installer.
4. Either copy-paste my **sounds** folder into the **baseq2** directory of your legitimate install of Quake 2, or make your own resource files, similarly placed in the baseq2 directory.
5. **Run your mod**, as described from Step 1 onward under the above section named "Run Soul Collector".
6. **Document** your mod and **share** it! **Have fun!**



# List of Quake 2 DLL and EXE Changes

* All code changes and additions are bound by **//TMF7 BEGIN** and **//TMF7 END**  (or if a single line edit just **//TMF7**) to clearly demarcate what I've added/changed from the base game code.

* Use **ctrl+shift+f**  and search for **TMF7** in Visual Studio to search for all my changes in a navigable list.

## DLL-Side ("game" visual studio project that builds to gamex86.dll)

### game directory

| Filename      |
| ------------- |
| g_local.h     |
| game.h        |
| q_shared.h    |
| g_ai.c        |
| g_chase.c     |
| g_cmds.c      |
| g_combat.c    |
| g_main.c      |
| g_monster.c   |
| g_possessed.c |
| g_possessed.h |
| g_save.c      |
| g_spawn.c     |
| g_weapon.c    |
| g_boss32.c    |
| m_move.c      |
| m_soldier.c   |
| p_client.c    |
| p_hud.c       |
| p_view.c      |
| p_weapon.c    |
| q_shared.c    |



## EXE-Side ("quake2" project in visual studio that builds to quake2.exe)

### client directory 

| Filename   |
| ---------- |
| cl_input.c |
| cl_scrn.c  |
| keys.c     |

### qcommon directory

| Filename |
| -------- |
| pmove.c  |

### server directory

| Filename   |
| ---------- |
| sv_ents.c  |
| sv_game.c  |
| sv_init.c  |
| sv_world.c |

### engine configuration file

| Filename   |
| ---------- |
| Config.cfg |

*Note: this config has been modified to add and change keyboard bindings*



# id Software License

This is the complete source code for Quake 2, version 3.21, buildable with
visual C++ 6.0.  The linux version should be buildable, but we haven't
tested it for the release.

The code is all licensed under the terms of the GPL (gnu public license).  
You should read the entire license, but the gist of it is that you can do 
anything you want with the code, including sell your new version.  The catch 
is that if you distribute new binary versions, you are required to make the 
entire source code available for free to everyone.

The primary intent of this release is for entertainment and educational 
purposes, but the GPL does allow commercial exploitation if you obey the 
full license.  If you want to do something commercial and you just can't bear 
to have your source changes released, we could still negotiate a separate 
license agreement (for $$$), but I would encourage you to just live with the 
GPL.

All of the Q2 data files remain copyrighted and licensed under the 
original terms, so you cannot redistribute data from the original game, but if 
you do a true total conversion, you can create a standalone game based on 
this code.

Thanks to Robert Duffy for doing the grunt work of building this release.

John Carmack
Id Software
