#pragma once

enum wear_s : unsigned char {
	Backpack, LastBackpack = Backpack + 13,
	Head, Neck, Body, RightHand, LeftHand, RightRing, LeftRing, Elbow, Legs, Quiver,
	FirstBelt, SecondBelt, LastBelt,
	FirstInvertory = Backpack, LastInvertory = LastBelt
};
struct weari {
	const char* name;
	const char*	choose_name;
};