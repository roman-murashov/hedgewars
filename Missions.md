# Missions #
## Introduction ##
Missions can be played in singleplayer mode and are selected from the single player menu in the mission menu (the hedgehog at the blackboard).

## Mission files ##
Only the Lua script is needed, the other files/data are optional. But it is still recommended to create them.

### The mission's Lua script ###
This is the heart of your mission. Here you will set up the mission and define the entire game logic of the mission.

See [LuaGuide](LuaGuide.md), [LuaAPI](LuaAPI.md) and [LuaLibraries](LuaLibraries.md) for more information about scripting.

This file needs to be saved in `Data/Missions/Training`. The file name must end with “`.lua`”. Remember the file name, you will need it for the other files.

#### Hints ####
In mission scripting, some patterns appear often, so here are some hints for your convenience (Refer to [LuaAPI](LuaAPI.md) for a full documentation):

  * Start with `onGameInit` to set up the most basic environment parameters, teams, hedgehogs, etc.
  * Important set up functions: `AddTeam`, `AddHog`, `AddGear`, `SetGearPosition`.
  * You probably almost always want to call `ShowMission` at the beginning of the game (`onGameStart`) to show the objectives of your mission.
  * Enable the game flag `gfOneClanMode` if you need only one clan for your mission.
  * Normally, the game ends when all hedgehogs are dead or one clan is remaining (unless `gfOneClanMode` is set). You can call `EndGame` to end the game manually.
  * Call `SendStat` to customize the statistics screen.
  * Use the [Locale](LuaLibraries#Locale.md) library to make your mission translatable. Enclose the human-readable texts in `loc`.
  * When making your mission translatable, use Lua's `string.format` to insert numbers and other strings instead of just concatenating everything together.
  * If you want to create complex animations and/or cinematics, include and use the [Animate](LuaLibraries#Animate.md) library.

### Preview image ###
The preview image is displayed in the mission menu when you have selected the mission.
It must be a PNG image of size 314px×260px.

The file must be saved in `Data/Graphics/Missions/Training`. The file name must follow this pattern:

`<mission name>@2x.png`

Where `<mission name>` is name of the mission script without the file name suffix.

For example, the preview image for the mission script `User_Mission_-_Teamwork.lua` must have the name `User_Mission_-_Teamwork@2x.png`.

### Mission title and description ###
The mission title and its description as shown in the mission menu must be edited in the missions files in `Data/Locale`. The file name is `missions_<language code>.txt`. There is one file per language. I.e. `missions_en.txt` for English, `missions_de.txt` for German, etc. Note that these files are supposed to contain the titles and descriptions of **all** missions on your computer. This makes these files unsuitable for sharing or inclusion into [HWP files](HWPFormat.md), for example.

To add the title an description for your mission, just add these 2 lines into the file according to this syntax:

```
<mission name>.name=<mission title>
<mission name>.desc=<mission description>
```

  * `<mission name>` is the file name of the mission script without the file suffix.
  * `<mission title>` is the mission title as shown in the mission menu.
  * `<mission description>` is a short description of the mission, also only shown in the mission menu.

You can optionally enclose `<mission title>` and `<mission description>` in quotation marks (they will be removed by the game).

If the mission title and description are missing, the mission menu will derive the title from the file name of the Lua script instead and shows no description.

For more information about translating Hedgewars, see [Translations](Translations.md).

#### Example ####
Here are the English title and description of the mission “Bamboo Thicket” (file name of script: “`User_Mission_-_Bamboo_Thicket.lua`”):

```
User_Mission_-_Bamboo_Thicket.name=Mission: Bamboo Thicket
User_Mission_-_Bamboo_Thicket.desc="Death comes from above."
```