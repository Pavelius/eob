#pragma once

enum morale_s : unsigned char {
	Lawful, Good, Neutral, Chaotic, Evil,
};
enum alignment_s : unsigned char {
	LawfulGood, NeutralGood, ChaoticGood, LawfulNeutral, TrueNeutral, ChaoticNeutral, LawfulEvil, NeutralEvil, ChaoticEvil,
	FirstAlignment = LawfulGood, LastAlignment = ChaoticEvil,
};
struct moralei {
	const char*		name;
};
