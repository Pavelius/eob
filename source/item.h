#include "crt.h"
#include "damage.h"
#include "dice.h"
#include "enchant.h"
#include "rarity.h"
#include "size.h"
#include "usability.h"
#include "variant.h"
#include "wear.h"

#pragma once

enum {
	GP = 1
};
enum good_s : unsigned char {
	Armors, Books, Clothes, Devices, Food, Jewelry, Papers, Potions, Tools, Weapons
};
enum item_feat_s : unsigned char {
	TwoHanded, Light, Versatile, Ranged, Deadly, Quick, UseInHand,
	SevereDamageUndead,
	Natural, Charged, Countable,
	NotForSale, Expandable, Unique
};
enum item_s : unsigned char {
	NoItem,
	// Items
	AxeBattle, Axe, Club, Dagger, Flail, Halberd, HammerWar, Mace, Spear, Staff,
	SwordBastard, SwordLong, SwordShort, SwordTwoHanded,
	Bow, Sling,
	Robe, ArmorLeather, ArmorStuddedLeather, ArmorScale, ArmorChain, ArmorBanded, ArmorPlate,
	Helm, Shield, Boots,
	Bracers,
	Necklage, NecklageRich, NecklageVeryRich,
	Arrow, Dart, Stone,
	Bones, Skull, Bone, DungeonMap,
	HolySymbol, HolySymbolEvil, MagicBook, TheifTools, MagicWand, MageScroll, PriestScroll,
	KeyShelf, KeySilver, KeyCooper, KeySkull, KeySpider, KeyMoon, KeyDiamond, KeyGreen,
	RedRing, BlueRing, GreenRing,
	RedPotion, BluePotion, GreenPotion,
	RedGem, BlueGem, GreenGem, PurpleGem,
	Ration, RationIron,
	// Special items
	DustOfAges, Horn, TigerEye, Heft, Tooth, MantistHead,
	Scepeter, SilverSword,
	StoneAmulet, StoneDagger, StoneGem, StoneHolySymbol, StoneOrb,
	// Monster attacks
	Slam, Slam1d4, Claws, Bite, Bite1d41, Bite2d6,
	ShokingHand, FlameBladeHand, FlameHand,
	LastItem = FlameHand,
	// Thrown effect
	FireThrown, LightingThrown, IceThrown, MagicThrown
};
struct combati {
	char				number_attacks_p2r;
	damage_s			type;
	char				speed;
	dice				damage;
	char				bonus, critical_multiplier, critical_range;
	class item*			weapon;
	enchant_s			enchant;
	bool				have_enchant;
	bool				is(enchant_s v) const;
};
struct bonusi : variant {
	char				bonus, random;
	constexpr bonusi() : variant(), bonus(), random(0) {}
	constexpr bonusi(const variant& v, char bonus, char random = 0) : variant(v), bonus(bonus), random(random) {}
};
struct enchantmenti {
	rarity_s			rarity;
	const char*			name;
	bonusi				power;
};
struct itemi {
	struct weaponi : combati {
		dice			damage_large;
		constexpr weaponi() : damage_large(), combati() {}
		constexpr weaponi(char number_attack, damage_s type, char speed, dice damage, dice damage_large, char bonus = 0, const weaponi* next = 0) :
			combati{number_attack, type, speed, damage, bonus}, damage_large(damage_large) {}
	};
	struct armori {
		char			ac;
		char			deflect;
		char			reduction;
	};
	struct portraiti {
		unsigned char	avatar;
		unsigned char	ground;
		unsigned char	size;;
		item_s			shoot;
	};
	const char*			name;
	rarity_s			rarity;
	portraiti			image;
	unsigned			costgp;
	char				cost;
	good_s				goods;
	wear_s				equipment;
	usabilitya			usability;
	cflags<item_feat_s>	feats;
	weaponi				weapon;
	armori				armor;
	aref<enchantmenti>	enchantments;
	item_s				ammo;
	size_s				getsize() const;
};
class item {
	item_s				type;
	union {
		unsigned char	flags;
		struct {
			unsigned char identified : 1; // All properties of item will be known
			unsigned char cursed : 1; // Negative effect
			unsigned char broken : 2; // Next breaking destroy item
			unsigned char started : 1; // Item is generated from start
		};
	};
	unsigned char		subtype; // spell scroll or spell of wand
	unsigned char		charges; // uses of item
public:
	typedef std::initializer_list<item_s> typea;
	constexpr item(item_s type = NoItem) : type(type), flags(0), subtype(0), charges(0) {}
	item(item_s type, rarity_s rarity);
	item(item_s type, variant power);
	item(item_s type, variant power, int magic);
	constexpr explicit operator bool() const { return type != NoItem; }
	constexpr bool operator==(const item& i) const { return i.type == type && i.subtype == subtype && i.flags == flags && i.charges == charges; }
	bool				cast(spell_s id, int level, bool run);
	void				clear();
	bool				damage(const char* text_damage, const char* text_brokes);
	void				finish();
	void				get(combati& result, size_s enemy_size) const;
	item_s				getammo() const { return gete().ammo; }
	int					getarmorpenalty(ability_s skill) const;
	int					getcost() const;
	int					getcostgp() const;
	int					getcount() const;
	int					getcharges() const { return charges; }
	constexpr const itemi& gete() const { return bsdata<itemi>::elements[type]; }
	int					getenchant(enchant_s v) const;
	const enchantmenti* getenchantment() const;
	static void*		getenchantptr(const void* object, int index);
	wear_s				getequiped() const;
	const itemi&		getitem() const { return bsdata<itemi>::elements[type]; }
	int					getmagic() const;
	void				getname(stringbuilder& sb) const;
	creature*			getowner() const;
	int					getportrait() const;
	bonusi				getpower() const;
	constexpr rarity_s	getrarity() const { return gete().rarity; }
	static rarity_s		getrandomrarity(int level);
	item_s				gettype() const { return type; }
	wear_s				getwear() const { return gete().equipment; }
	int					get(enchant_s value) const;
	constexpr bool		is(good_s v) const { return gete().goods == v; }
	constexpr bool		is(usability_s v) const { return gete().usability.is(v); }
	constexpr bool		is(item_feat_s v) const { return gete().feats.is(v); }
	constexpr bool		is(item_s v) const { return type == v; }
	bool				isartifact() const;
	constexpr bool		isbroken() const { return broken != 0; }
	constexpr bool		ischarged() const { return is(Charged); }
	constexpr bool		iscost() const { return gete().cost > 0; }
	constexpr bool		iscostgp() const { return gete().costgp > 0; }
	constexpr bool		iscursed() const { return cursed != 0; }
	constexpr bool		isidentified() const { return identified != 0; }
	bool				ismagical() const;
	bool				ismelee() const;
	constexpr bool		isnatural() const { return is(Natural); }
	bool				ispower(variant v) const;
	constexpr bool		isranged() const { return is(Ranged); }
	bool				issmall() const;
	bool				isstarted() const { return started != 0; }
	bool				istwohanded() const { return is(TwoHanded); }
	bool				match(const typea& v) const { for(auto e : v) if(e == type) return true; return false; }
	static void			select(adat<item>& result, good_s good, rarity_s rarity);
	void				sell();
	void				setbroken(int value) { broken = value; }
	void				setcharges(int v);
	void				setcount(int v);
	void				setcursed(int value) { cursed = value; }
	void				setenchant(int v) { subtype = v; }
	void				setidentified(int value) { identified = value; }
	item&				setpower(rarity_s rarity);
	void				setpower(variant power);
	void				setpower(variant power, int magic);
	void				setstarted(int v) { started = v; }
	bool				stack(item& v);
	void				use() { setcount(getcount() - 1); }
};