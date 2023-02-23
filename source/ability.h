#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	SaveVsParalization, SaveVsPoison, SaveVsTraps, SaveVsMagic,
	ClimbWalls, HearNoise, MoveSilently, OpenLocks, RemoveTraps, ReadLanguages,
	LearnSpell,
	ResistCharm, ResistCold, ResistFire, ResistMagic,
	CriticalDeflect, DetectSecrets,
	AC,
	AttackMelee, AttackRange, AttackAll,
	DamageMelee, DamageRange, DamageAll,
	Speed,
	BonusExperience, BonusSave, ReactionBonus,
	ExeptionalStrenght,
	Hits
};
