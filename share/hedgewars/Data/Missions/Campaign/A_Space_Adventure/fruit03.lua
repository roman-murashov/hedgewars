------------------- ABOUT ----------------------
--
-- Hero has get into an Red Strawberies ambush
-- He has to eliminate the enemies by using limited
-- ammo of sniper rifle and watermelon

HedgewarsScriptLoad("/Scripts/Locale.lua")
HedgewarsScriptLoad("/Scripts/Animate.lua")
HedgewarsScriptLoad("/Missions/Campaign/A_Space_Adventure/global_functions.lua")

----------------- VARIABLES --------------------
-- globals
local missionName = loc("Precise shooting")
local timeLeft = 10000
local lastWeaponUsed = amSniperRifle
-- hogs
local hero = {
	name = loc("Hog Solo"),
	x = 1100,
	y = 560
}
local enemiesOdd = {
	{name = "Hog 1", x = 2000 , y = 175},
	{name = "Hog 3", x = 1950 , y = 1110},
	{name = "Hog 5", x = 1270 , y = 1480},
	{name = "Hog 7", x = 240 , y = 790},
	{name = "Hog 9", x = 620 , y = 1950},
	{name = "Hog 11", x = 720 , y = 1950},
	{name = "Hog 13", x = 1620 , y = 1950},
	{name = "Hog 15", x = 1720 , y = 1950},
}
local enemiesEven = {
	{name = "Hog 2", x = 660, y = 140},
	{name = "Hog 4", x = 1120, y = 1250},
	{name = "Hog 6", x = 1290, y = 1250},
	{name = "Hog 8", x = 820, y = 1950},
	{name = "Hog 10", x = 920, y = 1950},
	{name = "Hog 12", x = 1820, y = 1950},
	{name = "Hog 14", x = 1920, y = 1950},
	{name = "Hog 16", x = 1200, y = 560},
}
-- teams
local teamA = {
	name = loc("Hog Solo"),
	color = tonumber("38D61C",16) -- green
}
local teamB = {
	name = loc("RS1"),
	color = tonumber("FF0000",16) -- red
}
local teamC = {
	name = loc("RS2"),
	color = tonumber("FF0000",16) -- red
}

-------------- LuaAPI EVENT HANDLERS ------------------

function onGameInit()
	GameFlags = gfDisableWind + gfInfAttack
	Seed = 1
	TurnTime = 15000
	CaseFreq = 0
	MinesNum = 0
	MinesTime = 1
	Explosives = 0
	Map = "fruit03_map"
	Theme = "Fruit"
	
	-- Hog Solo
	AddTeam(teamA.name, teamA.color, "Bone", "Island", "HillBilly", "cm_birdy")
	hero.gear = AddHog(hero.name, 0, 100, "war_desertgrenadier1")
	AnimSetGearPosition(hero.gear, hero.x, hero.y)
	-- enemies
	AddTeam(teamC.name, teamC.color, "Bone", "Island", "HillBilly", "cm_birdy")
	for i=1,table.getn(enemiesEven) do
		enemiesEven[i].gear = AddHog(enemiesEven[i].name, 1, 100, "war_desertgrenadier1")
		AnimSetGearPosition(enemiesEven[i].gear, enemiesEven[i].x, enemiesEven[i].y)
	end	
	AddTeam(teamB.name, teamB.color, "Bone", "Island", "HillBilly", "cm_birdy")
	for i=1,table.getn(enemiesOdd) do
		enemiesOdd[i].gear = AddHog(enemiesOdd[i].name, 1, 100, "war_desertgrenadier1")
		AnimSetGearPosition(enemiesOdd[i].gear, enemiesOdd[i].x, enemiesOdd[i].y)
	end
	
	initCheckpoint("fruit03")
	
	AnimInit()
	--AnimationSetup()
end

function onGameStart()
	AnimWait(hero.gear, 3000)
	FollowGear(hero.gear)
	
	AddEvent(onHeroDeath, {hero.gear}, heroDeath, {hero.gear}, 0)
	
	--hero ammo
	AddAmmo(hero.gear, amTeleport, 2)
	AddAmmo(hero.gear, amSniperRifle, 2)
	AddAmmo(hero.gear, amWatermelon, 2)
	--enemies ammo
	AddAmmo(enemiesOdd[1].gear, amDEagle, 100)
	AddAmmo(enemiesOdd[1].gear, amSniperRifle, 100)
	AddAmmo(enemiesOdd[1].gear, amWatermelon, 1)
	AddAmmo(enemiesOdd[1].gear, amGrenade, 5)
	AddAmmo(enemiesEven[1].gear, amDEagle, 100)
	AddAmmo(enemiesEven[1].gear, amSniperRifle, 100)
	AddAmmo(enemiesEven[1].gear, amWatermelon, 1)
	AddAmmo(enemiesEven[1].gear, amGrenade, 5)
	
	SendHealthStatsOff()
end

function onNewTurn()
	if CurrentHedgehog == hero.gear then
		if GetAmmoCount(hero.gear, amSkip) == 0 then
			TurnTimeLeft = TurnTime + timeLeft
			AddAmmo(hero.gear, amSkip, 1)
		end
		timeLeft = 0
	end
	turnHogs()
	WriteLnToConsole("NEW TURN")
end

function onGameTick20()
	if CurrentHedgehog == hero.gear and TurnTimeLeft ~= 0 then
		timeLeft = TurnTimeLeft
	end
end

function onGearDamage(gear, damage)
	FollowGear(gear)
	WriteLnToConsole("GEAR DAMAGED")
end

function onGearDelete(gear)
	WriteLnToConsole("HERO : "..hero.gear)
	WriteLnToConsole("GEAR : "..gear)
	WriteLnToConsole("UPPER BOUND : "..enemiesOdd[table.getn(enemiesOdd)].gear)
	WriteLnToConsole("UPPER BOUND : "..enemiesEven[table.getn(enemiesEven)].gear)
	
	if (gear > hero.gear and gear <= enemiesOdd[table.getn(enemiesOdd)].gear) or 
			(gear > hero.gear and gear <= enemiesEven[table.getn(enemiesEven)].gear) then
		WriteLnToConsole("NOT HERO GEAR")
		local availableTeleports = GetAmmoCount(hero.gear,amTeleport)
		local availableSniper = GetAmmoCount(hero.gear,amSniperRifle)
		if availableTeleports < 2 then
			AddAmmo(hero.gear, amTeleport, availableTeleports + 1 )
		end
		if availableSniper < 3 then
			AddAmmo(hero.gear, amSniperRifle, availableSniper + 1 )
		end
	end
	WriteLnToConsole("GEAR KILLED")
end

-------------- EVENTS ------------------

function onHeroDeath(gear)
	if not GetHealth(hero.gear) then
		return true
	end
	return false
end

-------------- ACTIONS ------------------

-- game ends anyway but I want to sent custom stats probably...
function heroDeath(gear)
	EndGame()
end

------------------ Other Functions -------------------

function turnHogs()
	if GetHealth(hero.gear) then
		for i=1,table.getn(enemiesEven) do
			if GetHealth(enemiesEven[i].gear) then
				if GetX(enemiesEven[i].gear) < GetX(hero.gear) then
					HogTurnLeft(enemiesEven[i].gear, false)
				elseif GetX(enemiesEven[i].gear) > GetX(hero.gear) then
					HogTurnLeft(enemiesEven[i].gear, true)
				end
			end
		end
		for i=1,table.getn(enemiesOdd) do
			if GetHealth(enemiesOdd[i].gear) then
				if GetX(enemiesOdd[i].gear) < GetX(hero.gear) then
					HogTurnLeft(enemiesOdd[i].gear, false)
				elseif GetX(enemiesOdd[i].gear) > GetX(hero.gear) then
					HogTurnLeft(enemiesOdd[i].gear, true)
				end
			end
		end
	end
end
