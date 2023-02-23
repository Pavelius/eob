#pragma once

enum spell_s : unsigned char {
	Moved,
	// Spells (level 1)
	Bless, BurningHands, CureLightWounds, DetectEvil, DetectMagic, Fear, FeatherFall,
	Identify, MageArmor, MagicMissile, Mending,
	ProtectionFromEvil, PurifyFood,
	ReadLanguagesSpell, ResistColdSpell, ShieldSpell, ShokingGrasp, Sleep,
	// Spells (level 2)
	AcidArrow, Aid, Blindness, Blur, Deafness, FlameBlade, FlamingSphere, Goodberry, HoldPerson,
	Invisibility, Knock, ProduceFlame, ResistFireSpell, Scare, SlowPoison,
	// Spells (level 3)
	CreateFood, CureBlindnessDeafness, Disease, CureDisease, Haste, MageFear, NegativePlanProtection,
	Regeneration, RemoveCurse, RemoveParalizes,
	// Spells (level 4)
	CureSeriousWounds, NeutralizePoison, Poison,
	// Specila ability
	LayOnHands, TurnUndead,
	RaiseStrenght, RaiseDexterity, RaiseConstitution, RaiseIntellect, RaiseWisdow, RaiseCharisma,
	GainExperience,
	FirstSpellAbility = LayOnHands, LastSpellAbility = GainExperience,
};
