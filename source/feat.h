#pragma once

enum feat_s : unsigned char {
	BonusSaveVsPoison, BonusSaveVsSpells,
	HolyGrace, Ambidextrity, NoExeptionalStrenght,
	Undead, Slowest,
	ResistBludgeon, ResistSlashing, ResistPierce, ImmuneNormalWeapon, ImmuneDisease,
	BonusVsElfWeapon, BonusToHitVsGoblinoid, BonusDamageVsEnemy, BonusACVsLargeEnemy, BonusHP,
};
struct feati {
	const char*			name;
};
