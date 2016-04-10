echo off



::empty the mod directory for updating

del "C:\Program Files (x86)\Steam\SteamApps\common\Quake 2\soulcollector\*"	/q




::copy over the relevant files to the mod directory

copy "C:\Users\Tom\Desktop\code\Quake2-Project\release\gamex86.dll" "C:\Program Files (x86)\Steam\SteamApps\common\Quake 2\soulcollector\"




::run quake2 with the mod
"C:\Program Files (x86)\Steam\SteamApps\common\Quake 2\quake2.exe" +game soulcollector










