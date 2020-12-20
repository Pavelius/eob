#include "dice.h"
#include "color.h"
#include "crt.h"
#include "point.h"
#include "rect.h"
#include "stringbuilder.h"
#include "markup.h"

#define assert_enum(e, last) static_assert(sizeof(bsdata<e##i>::elements) / sizeof(bsdata<e##i>::elements[0]) == last + 1, "Invalid count of " #e " elements");
#define MNLNK(T1, T2) BSLNK(T1, T2) DGLNK(T1, T2)

const unsigned short	Blocked = 0xFFFF;
const int				walls_frames = 9;
const int				walls_count = 6;
const int				door_offset = 1 + walls_frames * walls_count;
const int				decor_offset = door_offset + 9;
const int				decor_count = 19;
const int				decor_frames = 10;
const int				scrx = 22 * 8;
const int				scry = 15 * 8;
const int				mpx = 38;
const int				mpy = 23;

enum resource_s : unsigned char {
	NONE,
	BORDER, OUTTAKE,
	CHARGEN, CHARGENB, COMPASS, INVENT, ITEMS, ITEMGS, ITEMGL,
	BLUE, BRICK, CRIMSON, DROW, DUNG, GREEN, FOREST, MEZZ, SILVER, XANATHA,
	MENU, PLAYFLD, PORTM, THROWN, XSPL,
	// Monsters
	ANKHEG, ANT, BLDRAGON, BUGBEAR, CLERIC1, CLERIC2, CLERIC3, DRAGON, DWARF, FLIND,
	GHOUL, GOBLIN, GUARD1, GUARD2, KOBOLD, KUOTOA, LEECH, ORC,
	SHADOW, SKELETON, SKELWAR, SPIDER1, WIGHT, WOLF, ZOMBIE,
	Count
};
enum race_s : unsigned char {
	NoRace,
	Dwarf, Elf, HalfElf, Halfling, Human,
	Humanoid, Goblinoid, Insectoid, Animal,
};
enum alignment_s : unsigned char {
	LawfulGood, NeutralGood, ChaoticGood, LawfulNeutral, TrueNeutral, ChaoticNeutral, LawfulEvil, NeutralEvil, ChaoticEvil,
	FirstAlignment = LawfulGood, LastAlignment = ChaoticEvil,
};
enum gender_s : unsigned char {
	NoGender,
	Male, Female,
};
enum size_s : unsigned char {
	Tiny, Small, Medium, Tall, Large,
};
enum duration_s : unsigned char {
	Instant,
	Duration1PerLevel, Duration1d4P1PerLevel,
	Duration5PerLevel,
	DurationTurn, DurationTurnPerLevel,
	DurationHour, Duration1HourPerLevel, Duration2Hours, Duration4Hours, Duration8Hours,
};
enum target_s : unsigned char {
	TargetSelf,
	TargetThrow, TargetThrowHitFighter, TargetAllThrow,
	TargetClose, TargetAllClose,
	TargetAlly, TargetAllAlly,
	TargetItems, TargetAllyItems, TargetAllAllyItems,
};
enum message_s : unsigned char {
	MessageMagicWeapons, MessageMagicRings, MessageSecrets, MessageTraps,
	MessageAtifacts, MessageHabbits,
};
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
	CreateFood, CureBlindnessDeafness, Disease, CureDisease, Haste, NegativePlanProtection,
	RemoveCurse, RemoveParalizes,
	// Spells (level 4)
	CureSeriousWounds, Poison,
	// Specila ability
	LayOnHands, TurnUndead,
	FirstSpellAbility = LayOnHands, LastSpellAbility = TurnUndead,
};
enum class_s : unsigned char {
	NoClass,
	Cleric, Fighter, Mage, Paladin, Ranger, Theif,
	FighterCleric, FighterMage, FighterTheif, FighterMageTheif,
	ClericTheif, MageTheif,
};
enum monster_s : unsigned char {
	NoMonster,
	AntGiant, Bugbear, ClericOldMan, DwarfWarrior, Gnoll, Ghoul, Goblin, Kobold, Kuotoa, Leech,
	Orc, Shadow, Skeleton, SkeletonWarrior, Spider, Wight, Wolf, Zombie,
	LastMonster = Zombie,
};
enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	LastAbility = Charisma,
	// Saves
	SaveVsParalization, SaveVsPoison, SaveVsTraps, SaveVsMagic,
	FirstSave = SaveVsParalization, LastSave = SaveVsMagic,
	// Theif skills
	ClimbWalls, HearNoise, MoveSilently, OpenLocks, RemoveTraps, ReadLanguages,
	// Other skills
	LearnSpell,
	ResistCharm, ResistCold, ResistFire, ResistMagic,
	CriticalDeflect, DetectSecrets,
	LastSkill = DetectSecrets,
};
enum wear_s : unsigned char {
	Backpack, LastBackpack = Backpack + 13,
	Head, Neck, Body, RightHand, LeftHand, RightRing, LeftRing, Elbow, Legs, Quiver,
	FirstBelt, SecondBelt, LastBelt,
	FirstInvertory = Backpack, LastInvertory = LastBelt
};
enum enchant_s : unsigned char {
	OfAccuracy, OfAdvise,
	OfEnergyDrain,
	OfFear, OfHolyness, OfLuck,
	OfParalize, OfPoison, OfPoisonStrong, OfProtection, OfRegeneration,
	OfSharpness, OfSmashing, OfSpeed, OfStrenghtDrain,
	OfVampirism, OfWizardy,
};
enum rarity_s : unsigned char {
	Common, Uncommon, Rare, VeryRare, Artifact
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
	Necklage, NecklageRich, Jewelry,
	Arrow, Dart, Stone,
	Bones, DungeonMap,
	HolySymbol, HolyWarriorSymbol, MagicBook, TheifTools, MagicWand, MageScroll, PriestScroll,
	KeyShelf, KeySilver, KeyCooper, KeySkull, KeySpider, KeyMoon, KeyDiamond, KeyGreen,
	RedRing, BlueRing, GreenRing,
	RedPotion, BluePotion, GreenPotion,
	RedGem, BlueGem, GreenGem, PurpleGem,
	Ration, RationIron,
	// Special items
	DustOfAges, Horn, MantistHead,
	Scepeter, SilverSword,
	StoneAmulet, StoneDagger, StoneGem, StoneHolySymbol, StoneOrb,
	// Monster attacks
	Slam, Slam1d4, Claws, Bite, Bite1d41, Bite2d6,
	ShokingHand, FlameBladeHand, FlameHand,
	LastItem = FlameHand,
	// Thrown effect
	FireThrown, LightingThrown, IceThrown, MagicThrown
};
enum damage_s : unsigned char {
	Bludgeon, Slashing, Pierce,
	Cold, Electricity, Fire, Magic,
	Heal, Paralize, Death, Petrification,
};
enum save_s : unsigned char {
	NoSave, SaveHalf, SaveNegate,
};
enum overlay_flag_s : unsigned char {
	Active
};
enum cell_s : unsigned char {
	CellUnknown,
	// Dungeon cells
	CellPassable, CellWall, CellDoor, CellStairsUp, CellStairsDown, CellPortal, // On space
	CellButton, CellPit, CellBlood, CellBanner, CellPitUp, // On floor
	// Decor
	CellPuller, CellSecrectButton, CellCellar, CellMessage,
	CellKeyHole1, CellKeyHole2, CellTrapLauncher,
	CellDecor1, CellDecor2, CellDecor3,
	CellDoorButton
};
enum cell_flag_s : unsigned char {
	CellExplored, CellActive
};
enum direction_s {
	Center,
	Left, Up, Right, Down
};
enum feat_s : unsigned char {
	BonusSaveVsPoison, BonusSaveVsSpells,
	HolyGrace, Ambidextrity, NoExeptionalStrenght,
	Undead,
	ResistBludgeon, ResistSlashing, ResistPierce, ImmuneNormalWeapon, ImmuneDisease,
	BonusVsElfWeapon, BonusToHitVsGoblinoid, BonusDamageVsEnemy, BonusACVsLargeEnemy, BonusHP,
};
enum usability_s : unsigned char {
	UseLeatherArmor, UseMetalArmor, UseShield,
	UseTheifWeapon, UseMartialWeapon, UseLargeWeapon,
	UseScrolls, UseDivine, UseArcane, UseTheif
};
enum item_feat_s : unsigned char {
	TwoHanded, Light, Versatile, Ranged, Deadly, Quick, UseInHand,
	SevereDamageUndead,
	Natural, Charged, Countable,
	Valuable, Famed,
	Unique
};
enum attack_s : unsigned char {
	AutoHit,
	OneAttack, OneAndTwoAttacks, TwoAttacks,
	OnHit, OnAllHit, OnCriticalHit,
};
enum reaction_s : unsigned char {
	Indifferent, Friendly, Flight, Cautious, Threatening, Hostile,
};
enum intellegence_s : unsigned char {
	NoInt, AnimalInt, Semi, Low, Ave, Very, High, Exeptional, Genius, Supra, Godlike,
};
enum action_s : unsigned char {
	NoAction,
	HealParty, RessurectBones,
	StartCombat, LeaveAway, WinCombat, GainExperience, Trade,
	DeathSave, TrapDamage,
	PartyHas, PartyNotHas, PlayerHas, PlayerNotHas,
	AddItem, RemoveItem,
};
enum speech_s : unsigned char {
	Say, Ask,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Action, Alignment, Class, Creature, Damage,
	Enchant, Feat, Gender, Item, Number, Race, Reaction, Spell,
};
enum pack_s : unsigned char {
	PackDungeon, PackMonster, PackOuttake,
	PackInterface, PackBackground, PackCenter
};
class creature;
class item;
typedef short unsigned indext;
typedef flagable<LastSpellAbility> spellf;
typedef cflags<feat_s> feata;
typedef adatc<ability_s, char, LastSkill+1> skilla;
typedef cflags<usability_s> usabilitya;
typedef cflags<variant_s> variantf;
typedef const char*	(*fngetname)(void* object, stringbuilder& sb);
struct variant {
	variant_s			type;
	unsigned char		value;
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(const ability_s v) : type(Ability), value(v) {}
	constexpr variant(const action_s v) : type(Action), value(v) {}
	constexpr variant(const alignment_s v) : type(Alignment), value(v) {}
	constexpr variant(const class_s v) : type(Class), value(v) {}
	constexpr variant(const damage_s v) : type(Damage), value(v) {}
	constexpr variant(const enchant_s v) : type(Enchant), value(v) {}
	constexpr variant(const feat_s v) : type(Feat), value(v) {}
	constexpr variant(const gender_s v) : type(Gender), value(v) {}
	constexpr variant(const item_s v) : type(Item), value(v) {}
	constexpr variant(const race_s v) : type(Race), value(v) {}
	constexpr variant(const reaction_s v) : type(Reaction), value(v) {}
	constexpr variant(const spell_s v) : type(Spell), value(v) {}
	variant(const creature* v);
	constexpr explicit operator bool() const { return type != NoVariant; }
	constexpr bool operator==(const variant& e) const { return type == e.type && value == e.value; }
	void				clear() { type = NoVariant; value = 0; }
	creature*			getcreature() const;
	const char*			getname() const;
};
typedef variant conditiona[6];
struct variantc : adat<variant> {
	void				cspells(const creature* p, bool expand);
	int					chooselv(class_s type) const;
	void				matchsl(class_s type, int level);
	void				sort();
};
struct spellprogi {
	char				elements[21][10];
};
struct actioni {
	const char*			name;
	int					params;
};
struct action {
	action_s			type;
	variant				param;
	int					number;
};
struct abilityi {
	const char*			name;
	const char*			present;
	cflags<class_s>		allow;
};
struct abilitya {
	char				data[Charisma + 1];
};
struct alignmenti {
	const char*			name;
	adat<class_s, 8>	restricted;
};
struct attacki {
	const char*			name;
	char				attacks_p2r;
	const char*			prefix;
};
struct classi {
	const char*			name;
	char				playable;
	char				hd;
	ability_s			ability;
	adat<class_s, 4>	classes;
	usabilitya			usability;
	feata				feats;
	abilitya			minimum;
	adat<race_s, 12>	races;
	const spellprogi*	spells[2];
};
struct commandi {
	const char*			name;
};
struct damagei {
	const char*			name;
	feat_s				half;
	ability_s			reduce;
	feat_s				immunity;
};
struct directioni {
	const char*			name;
};
struct durationi {
	const char*			name;
	unsigned			multiplier, divider, addiction;
	dice				base;
	int					get(int v) const;
};
struct genderi {
	const char*			name;
};
struct sizei {
	const char*			name;
};
struct intellegencei {
	const char*			name;
	char				v1, v2;
};
struct usabilityi {
	const char*			name;
};
struct itemfeati {
	const char*			name;
};
struct varianti {
	const char*			name;
	array*				source;
	fntext				pgetname;
	unsigned			uname;
	const markup*		form;
};
struct combati {
	attack_s			attack;
	damage_s			type;
	char				speed;
	dice				damage;
	char				bonus, critical_multiplier, critical_range;
	item*				weapon;
};
struct enchantmenti {
	rarity_s			rarity;
	const char*			name;
	variant				power;
	char				magic;
};
struct itemi {
	struct weaponi : combati {
		dice			damage_large;
		const weaponi*	next;
		constexpr weaponi() : damage_large(), combati(), next(0) {}
		constexpr weaponi(attack_s attack, damage_s type, char speed, dice damage, dice damage_large, char bonus = 0, const weaponi* next = 0) :
			combati{attack, type, speed, damage, bonus}, damage_large(damage_large), next(next) {}
	};
	struct armori {
		char			ac;
		char			critical_deflect;
		char			reduction;
	};
	struct portraiti {
		unsigned char	avatar;
		unsigned char	ground;
		unsigned char	size;;
		item_s			shoot;
	};
	const char*			name;
	portraiti			image;
	char				cost;
	wear_s				equipment;
	usabilitya			usability;
	cflags<item_feat_s>	feats;
	weaponi				weapon;
	armori				armor;
	aref<enchantmenti>	enchantments;
	item_s				ammo;
	size_s				getsize() const;
};
struct feati {
	const char*			name;
};
struct enchanti {
	const char*			name;
};
struct monsteri {
	const char*			name;
	resource_s			rfile;
	short				overlays[4];
	race_s				race;
	gender_s			gender;
	size_s				size;
	alignment_s			alignment;
	intellegence_s		ins;
	feata				feats;
	char				hd[2];
	char				ac;
	item_s				attacks[4];
	variant				enchantments[2];
	skilla				skills;
	//
	int					getexperience() const;
	char				getpallette() const;
	bool				is(variant id) const;
	bool				is(feat_s id) const;
};
struct racei {
	const char*			name;
	abilitya			minimum;
	abilitya			maximum;
	abilitya			adjustment;
	feata				feats;
	usabilitya			usability;
	skilla				skills;
};
struct packi {
	char				name[16];
};
struct resourcei {
	char				name[16];
	char				path[16];
	pack_s				pack;
	void*				data;
	bool				isdungeon() const;
	bool				ismonster() const;
	static int			preview(int x, int y, int width, const void* object);
};
struct effecti {
	variant				type;
	duration_s			duration;
	save_s				save;
	char				save_bonus;
	dice				damage;
	dice				damage_per;
	char				damage_increment, damage_maximum;
	void				apply(creature* target, int level) const;
};
struct spelli {
	const char*			name;
	char				levels[4]; // mage, cleric, paladin, ranger
	target_s			range;
	effecti				effect;
	item_s				throw_effect;
	const char*			talk[3];
};
struct sitei {
	struct headi {
		resource_s		type;
		monster_s		habbits[2]; // Who dwelve here
		item_s			keys[2]; // Two keys open all doors
		item_s			special[2]; // Two special items find on this level
		race_s			language; // All messages in this language
	};
	struct crypti {
		monster_s		boss;
		explicit operator bool() const { return boss != NoMonster; }
	};
	struct chancei {
		char			curse;
	};
	headi				head;
	char				levels;
	chancei				chance;
	crypti				crypt;
	constexpr explicit operator bool() const { return head.type != NONE; }
	unsigned			getleveltotal() const;
};
struct weari {
	const char*			name;
	const char*			choose_name;
};
class item {
	item_s				type;
	union {
		unsigned char	flags;
		struct {
			unsigned char identified : 1;
			unsigned char cursed : 1; // -1 to quality and not remove
			unsigned char broken : 1; // Next breaking destroy item
		};
	};
	unsigned char		subtype; // spell scroll or spell of wand
	unsigned char		charges; // uses of item
	friend dginf<item>;
public:
	constexpr item(item_s type = NoItem) : type(type), flags(0), subtype(0), charges(0) {}
	item(item_s type, rarity_s rarity);
	item(item_s type, variant power);
	constexpr explicit operator bool() const { return type != NoItem; }
	constexpr bool operator==(const item& i) const { return i.type == type && i.subtype == subtype && i.flags == flags && i.charges==charges; }
	bool				cast(spell_s id, int level, bool run);
	void				clear();
	static bool			choose_enchantment(const void* object, const array& source, void* pointer);
	bool				damage(const char* text_damage, const char* text_brokes);
	void				finish();
	int					get(variant value) const;
	void				get(combati& result, const creature* enemy) const;
	int					getac() const;
	item_s				getammo() const { return gete().ammo; }
	int					getarmorpenalty(ability_s skill) const;
	int					getcost() const;
	int					getcount() const;
	int					getcharges() const { return charges; }
	int					getdeflect() const;
	constexpr const itemi& gete() const { return bsdata<itemi>::elements[type]; }
	const enchantmenti* getenchantment() const;
	static void*		getenchantptr(const void* object, int index);
	int					getmagic() const;
	void				getname(stringbuilder& sb) const;
	creature*			getowner() const;
	int					getportrait() const;
	variant				getpower() const;
	static rarity_s		getrandomrarity(int level);
	int					getspeed() const;
	item_s				gettype() const { return type; }
	wear_s				getwear() const { return gete().equipment; }
	constexpr bool		is(usability_s v) const { return gete().usability.is(v); }
	constexpr bool		is(item_feat_s v) const { return gete().feats.is(v); }
	constexpr bool		is(item_s v) const { return type == v; }
	bool				isartifact() const;
	constexpr bool		isbroken() const { return broken != 0; }
	constexpr bool		ischarged() const { return is(Charged); }
	constexpr bool		iscursed() const { return cursed != 0; }
	constexpr bool		isidentified() const { return identified != 0; }
	bool				ismagical() const;
	bool				ismelee() const;
	constexpr bool		isnatural() const { return is(Natural); }
	bool				ispower(variant v) const;
	constexpr bool		isranged() const { return is(Ranged); }
	bool				istwohanded() const { return is(TwoHanded); }
	void				setcharges(int v);
	void				setcount(int v);
	void				setbroken(int value) { broken = value; }
	void				setcursed(int value) { cursed = value; }
	void				setidentified(int value) { identified = value; }
	item&				setpower(rarity_s rarity);
	void				setpower(variant power);
	bool				stack(item& v);
	void				use() { setcount(getcount() - 1); }
};
struct boosti {
	variant				owner, id;
	unsigned			round;
	char				value;
	constexpr explicit operator bool() const { return id.type != NoVariant; }
	void				clear();
};
struct selli {
	item_s				object;
	rarity_s			rarity;
	constexpr selli(item_s object) : object(object), rarity(Common) {}
	constexpr selli(item_s object, rarity_s rarity) : object(object), rarity(rarity) {}
};
struct speechi {
	const char*			name;
};
struct messagei {
	struct imagei {
		char			custom[16];
		unsigned		flags;
		constexpr explicit operator bool() const { return custom[0] != 0; }
		static int		preview(int x, int y, int width, const void* object);
	};
	speech_s			type;
	int					id;
	conditiona			variants;
	const char*			text;
	short				next[2];
	imagei				overlay[4];
	selli*				trade;
	constexpr explicit operator bool() const { return id != 0; }
	void				apply() const;
	bool				isallow() const;
	void				choose(bool border, int next_id = 1, reaction_s reaction = Indifferent) const;
	const messagei*		find(int id, bool test_allow) const;
};
class itema : public adat<item*, 48> {
public:
	item*				choose(const char* format, bool cancel_button);
	void				forsale(bool remove);
	void				select();
};
struct looti {
	int					gold;
	int					experience;
	int					fame;
	int					progress;
	int					luck;
};
class creature {
	alignment_s			alignment;
	race_s				race;
	gender_s			gender;
	class_s				type;
	monster_s			kind;
	short unsigned		index;
	unsigned char		side;
	direction_s			direction;
	feata				feats;
	usabilitya			usability;
	short				hits, hits_aid, hits_rolled;
	char				initiative;
	char				levels[3];
	char				ability[LastAbility + 1];
	item				wears[LastInvertory + 1];
	char				spells[LastSpellAbility + 1];
	char				prepared[LastSpellAbility + 1];
	spellf				known_spells;
	spellf				active_spells;
	char				avatar;
	unsigned			experience;
	unsigned char		name[2];
	char				str_exeptional;
	char				drain_energy, drain_strenght, disease_progress;
	char				pallette;
	short				food;
	reaction_s			reaction;
	//
	void				addboost(variant id, unsigned duration, char value = 0) const;
	void				attack_drain(creature* defender, char& value, int& hits);
	void				campcast(item& it);
	int					get_base_save_throw(ability_s st) const;
	class_s				getbestclass() const { return getclass(getclass(), 0); }
	void				prepare_random_spells(class_s type, int level);
	void				resting(int healed);
	char				racial_bonus(char* data) const;
	void				raise_level(class_s type);
	void				random_spells(class_s type, int level, int count);
	void				update_levelup(bool interactive);
	void				update_poison(bool interactive);
	friend dginf<creature>;
public:
	explicit operator bool() const { return race != NoRace; }
	typedef void		(creature::*apply_proc)(bool);
	void				activate(spell_s v) { active_spells.set(v); }
	bool				add(item i);
	bool				add(spell_s type, unsigned duration = 0, save_s id = NoSave, char save_bonus = 0, ability_s save_type = SaveVsMagic);
	void				addaid(int v) { hits_aid += v; }
	void				addexp(int value);
	static void			addexp(int value, int killing_hit_dice);
	static void			addparty(item i, bool interactive = false);
	static void			apply(apply_proc proc, bool interactive = true);
	void				apply(spell_s id, int level, unsigned duration);
	void				attack(short unsigned index, direction_s d, int bonus, bool ranged);
	void				attack(creature* defender, wear_s slot, int bonus);
	static void			camp(item& it);
	bool				cast(spell_s id, class_s type, int wand_magic, creature* target = 0);
	void				create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive = false);
	void				clear();
	bool				canspeak(race_s language) const;
	static creature*	choosehero();
	spell_s				choosespell(class_s type) const;
	void				damage(damage_s type, int hits, int magic_bonus = 0);
	void				enchant(spell_s id, int level);
	void				encounter(reaction_s id);
	void				equip(item it);
	item*				find(item_s v) const;
	void				finish();
	int					get(ability_s id) const;
	int					get(class_s id) const;
	int					get(spell_s spell) const { return spells[spell]; }
	void				get(combati& e, wear_s slot = RightHand, creature* enemy = 0) const;
	item				get(wear_s id) const;
	const spellprogi*	getprogress(class_s v) const;
	alignment_s			getalignment() const { return alignment; }
	int					getac() const;
	int					getavatar() const { return avatar; }
	int					getawards() const;
	int					getbonus(variant id) const;
	int					getbonus(variant id, wear_s slot) const;
	class_s				getcaster() const;
	int					getcasterlevel(class_s id) const;
	class_s				getclass() const { return type; }
	static class_s		getclass(class_s id, int index);
	int					getclasscount() const;
	static reaction_s	getcomreaction();
	direction_s			getdirection() const;
	int					getenchant(variant id, int bonus) const;
	int					getexperience() const { return experience; }
	int					getfood() const { return food; }
	int					getfoodmax() const;
	int					gethd() const;
	dice				gethitdice() const;
	int					gethitpenalty(int bonus) const;
	short				gethits() const { return hits + hits_aid; }
	short				gethitsmaximum() const;
	gender_s			getgender() const { return gender; }
	short unsigned		getindex() const;
	int					getinitiative() const { return initiative; }
	item*				getitem(wear_s id) { return &wears[id - FirstInvertory]; }
	static int			getlevel(spell_s id, class_s type);
	void				getname(stringbuilder& sb) const;
	const char*			getname(char* result, const char* result_maximum) const;
	int					getpallette() const { return pallette; }
	int					getpartyindex() const;
	int					getprepare(spell_s v) const { return prepared[v]; }
	race_s				getrace() const { return race; }
	reaction_s			getreaction() const { return reaction; }
	static int			getparty(ability_s v);
	resource_s			getres() const;
	int					getside() const;
	size_s				getsize() const;
	int					getspecialist(item_s weapon) const;
	int					getspeed() const;
	int					getspellsperlevel(class_s type, int spell_level) const;
	int					getstrex() const;
	int					getthac0(class_s cls, int level) const;
	bool				have(aref<class_s> source) const;
	bool				have(item_s v) const;
	void				heal(bool interactive) { damage(Heal, gethits()); }
	bool				haveforsale() const;
	void				interract();
	bool				is(spell_s v) const;
	bool				is(feat_s v) const { return feats.is(v); }
	bool				is(usability_s v) const { return usability.is(v); }
	bool				isaffect(variant v) const;
	static bool			isallow(class_s id, race_s r);
	static bool			isallow(alignment_s id, class_s c);
	bool				isallow(const item it, wear_s slot) const;
	bool				isallowremove(const item i, wear_s slot, bool interactive);
	bool				iscast(class_s v) const { return getprogress(v) != 0; }
	bool				isenemy(creature* target) const;
	bool				isinvisible() const;
	bool				isknown(spell_s v) const { return known_spells.is(v); }
	bool				ishero() const;
	bool				ismatch(const variant id) const;
	bool				ismatch(const messagei& v) const;
	bool				ismoved() const { return is(Moved); }
	bool				isready() const;
	bool				isuse(const item v) const;
	void				kill();
	void				poison(save_s save, char save_bonus = 0);
	void				preparespells();
	static void			preparespells(class_s type);
	void				random_equipment();
	void				random_name();
	void				remove(spell_s v);
	bool				remove(wear_s slot, bool interactive);
	void				removeboost(variant v);
	void				removeloot(looti& result);
	int					render_ability(int x, int y, int width, bool use_bold) const;
	int					render_combat(int x, int y, int width, bool use_bold) const;
	bool				roll(ability_s id, int bonus = 0) const;
	void				roll_ability();
	reaction_s			rollreaction(int bonus) const;
	void				say(spell_s id) const;
	void				say(const char* format, ...);
	void				say(const item& it, const char* format, ...);
	void				sayv(const char* format, const char* vl);
	bool				save(int& value, ability_s skill, save_s type, int bonus);
	void				scribe(item& it);
	static void			scriblescrolls();
	static unsigned		select(spell_s* result, const spell_s* result_maximum, class_s type, int level);
	void				select(itema& result);
	void				set(ability_s id, int v) { ability[id] = v; }
	void				set(alignment_s value) { alignment = value; }
	void				set(class_s value) { type = value; }
	void				set(gender_s value) { gender = value; }
	void				set(monster_s type);
	void				set(race_s value) { race = value; }
	void				set(reaction_s v) { reaction = v; }
	bool				set(ability_s skill, short unsigned index);
	void				set(spell_s spell, char v) { spells[spell] = v; }
	void				set(direction_s value);
	void				set(const item it, wear_s v) { wears[v] = it; }
	void				setavatar(int value) { avatar = value; }
	void				setframe(short* frames, short index) const;
	void				sethits(short v) { hits = v; };
	void				sethitsroll(short v) { hits_rolled = v; }
	void				setindex(short unsigned value) { index = value; }
	void				setinitiative(char value) { initiative = value; }
	void				setknown(spell_s id, bool v = true) { known_spells.set(id, v); }
	void				setmoved(bool value);
	void				setprepare(spell_s id, char v) { prepared[id] = v; }
	void				setside(int value);
	static void			setcom(reaction_s v);
	bool				setweapon(item_s v, int charges);
	void				subenergy();
	static bool			swap(item* itm1, item* itm2);
	void				uncurse(bool interactive);
	void				update(const boosti& e);
	void				update(bool interactive);
	void				update_turn(bool interactive);
	void				update_hour(bool interactive);
	static void			update_boost();
	bool				use(ability_s id, indext index, int bonus, bool* firsttime, int exp, bool interactive);
	static bool			use(item* pi);
	bool				useammo(item_s ammo, wear_s slot, bool use_item);
	bool				usequick();
	void				view_ability();
	static void			view_party();
	void				view_portrait(int x, int y) const;
};
class creaturea : public adat<creature*, 12> {
public:
	creature*			getbest(ability_s v);
	void				match(variant v, bool remove);
	void				match(const messagei& id, bool remove);
	void				rollinitiative();
	void				select(indext index);
};
struct dungeon {
	struct overlayi {
		cell_s			type; // type of overlay
		direction_s		dir; // overlay direction
		indext			index; // index
		indext			index_link; // linked to this location
		short unsigned	subtype; // depends on value type
		short unsigned	flags;
		constexpr explicit operator bool() const { return type != CellUnknown; }
		void			clear();
		bool			is(overlay_flag_s v) const { return (flags&(1 << v)) != 0; }
		void			remove(overlay_flag_s v) { flags &= ~(1 << v); }
		void			set(overlay_flag_s v) { flags |= 1 << v; }
	};
	struct groundi : item {
		indext			index;
		unsigned char	side;
		unsigned char	flags;
	};
	struct overlayitem : item {
		indext			storage_index;
	};
	struct statei {
		overlayi		up; // where is stairs up
		overlayi		down; // where is stairs down
		overlayi		portal; // where is portal
		overlayi		crypt; // where is crypt located
		overlayi		crypt_button; // where is crypt located
		indext			spawn[2]; // new monster appera here
		unsigned char	messages; // count of messages
		unsigned char	secrets; // count of secret rooms
		unsigned char	artifacts; // count of powerful items (+4 or hight)
		unsigned char	rings; // count of magical rings
		unsigned char	weapons; // count of magical weapons
		unsigned char	elements; // count of corridors
		unsigned char	traps; // count of traps
		unsigned char	special; // count of special items generated
		short unsigned	items; // total count of items
		short unsigned	overlays; // total count of overlays
		short unsigned	monsters; // total count of monsters
	};
	struct eventi {
		variant			owner;
		ability_s		skill;
		indext			index;
		constexpr operator bool() const { return owner.operator bool(); }
	};
	point				overland_index;
	unsigned char		level;
	sitei::headi		head;
	statei				stat;
	sitei::chancei		chance;
	unsigned char		data[mpx*mpy];
	groundi				items[512];
	overlayi			overlays[256];
	overlayitem			cellar_items[256];
	creature			monsters[200];
	eventi				events[256];
	dungeon() { clear(); }
	operator bool() const { return head.type != NONE; }
	overlayi*			add(indext index, cell_s type, direction_s dir);
	void				add(overlayi* p, item it);
	creature*			addmonster(monster_s type, indext index, char side, direction_s dir);
	int					addmonster(monster_s type, indext index, direction_s dir = Up);
	void				appear(indext index, int radius = 1);
	bool				allaround(indext index, cell_s t1 = CellWall, cell_s t2 = CellUnknown);
	void				attack(const combati& ci, creature* enemy) const;
	void				automap(bool fow);
	void				clear();
	void				clearboost();
	bool				create(rect& rc, int w, int h) const;
	static void			create(indext index, const sitei* site, bool interactive = false);
	void				dropitem(indext index, item rec, int side);
	void				dropitem(item* pi, int side = -1);
	void				explore(indext index, int r = 1);
	void				fill(indext index, int sx, int sy, cell_s value);
	void				finish(cell_s t);
	void				formation(indext index, direction_s dr);
	cell_s				get(indext index) const;
	cell_s				get(int x, int y) const;
	short unsigned		gettarget(indext start, direction_s dir);
	void				getblocked(indext* pathmap, bool treat_door_as_passable);
	int					getfreeside(creature** sides);
	int					getfreeside(indext index);
	unsigned			getitems(item** result, item** result_maximum, indext index, int side = -1);
	unsigned			getitems(item** result, item** result_maximum, overlayi* povr);
	int					getitemside(item* pi);
	indext				getindex(int x, int y) const;
	race_s				getlanguage() const;
	unsigned			getmonstercount() const;
	void				getmonsters(creature** result, indext index, direction_s dr);
	item_s				getkeytype(cell_s keyhole) const;
	short unsigned		getnearest(indext index, int radius, cell_s t1);
	short unsigned*		getnearestfree(indext* indicies, indext index);
	direction_s			getpassable(indext index, direction_s* dirs);
	short unsigned		getsecret() const;
	static size_s		getsize(creature** sides);
	overlayi*			getoverlay(indext index, direction_s dir);
	overlayi*			getoverlay(const overlayitem& e) { return &overlays[e.storage_index]; }
	cell_s				gettype(cell_s id);
	cell_s				gettype(overlayi* po);
	void				hearnoises();
	bool				is(indext index, cell_flag_s value) const;
	bool				is(indext index, int width, int height, cell_s v) const;
	bool				is(const rect& rc, cell_s id) const;
	bool				isactive(const overlayi* po);
	bool				isblocked(indext index) const;
	bool				isblocked(indext index, int side) const;
	bool				islineh(indext index, direction_s dir, int count, cell_s t1, cell_s t2 = CellUnknown) const;
	bool				ismatch(indext index, cell_s t1, cell_s t2);
	bool				ismonster(indext index) const;
	bool				ismonsternearby(indext i, int r = 3) const;
	bool				isroom(indext index, direction_s dir, int side, int height) const;
	static bool			isvisible(indext index);
	dungeon::overlayi*	getlinked(indext index);
	void				makedoor(const rect& rc, overlayi& door, direction_s dir, bool has_button, bool has_button_on_other_side);
	void				makeroom(const rect& rc, overlayi& door);
	void				makewave(indext start, indext* pathmap);
	void				move(indext index, direction_s dr);
	void				move(direction_s direction);
	void				passhour();
	void				passround();
	void				pickitem(item* itm, int side = -1);
	short unsigned		random(indext* indicies);
	bool				read(point overland_index, indext level);
	void				remove(indext index, cell_flag_s value);
	void				remove(overlayi* po, item it);
	void				remove(overlayi* po);
	void				rotate(direction_s direction);
	void				set(indext index, cell_s value);
	void				set(indext index, cell_flag_s value);
	void				set(indext index, direction_s dir, cell_s type);
	void				set(indext index, reaction_s v);
	void				setactive(overlayi* po, bool active);
	void				setactive(indext index, bool value);
	void				setactive(indext index, bool value, int radius);
	void				stop(indext index);
	void				traplaunch(indext index, direction_s dir, item_s show, const combati& e);
	void				turnto(indext index, direction_s dr);
	void				write();
};
struct adventurei {
	char				name[32];
	point				position;
	sitei				levels[8];
	void				create(bool interactive) const;
	void				enter();
};
struct fractioni : looti {
	char				name[32];
	explicit constexpr operator bool() const { return name[0] != 0; }
};
struct companyi {
	looti				resource;
	fractioni			fractions[16];
};
class worldmapi {
	char				name[32];
	point				camera;
	point				party;
	void				paint(point camera) const;
public:
	void				edit();
	point				getparty() const { return party; }
	void				read(const char* url);
	void				scroll(point from, point to) const;
	void				setparty(point v) { party = v; }
};
class gamei : companyi {
	indext				camera_index;
	direction_s			camera_direction;
	point				location_position;
	indext				location_level;
	unsigned			rounds;
	unsigned			rounds_turn;
	unsigned			rounds_hour;
	unsigned			killed[LastMonster + 1];
	unsigned			found_secrets;
	unsigned			gold;
	static void			render_worldmap(void* object);
public:
	void				add(monster_s id) { killed[id]++; }
	void				attack(indext index, bool ranged);
	void				endround();
	void				enter(point index, short unsigned level);
	void				equiping();
	const adventurei*	getadventure() const;
	void				findsecrets();
	int					getavatar(race_s race, gender_s gender, class_s cls);
	int					getavatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls);
	indext				getcamera() const { return camera_index; }
	creature*			getdefender(short unsigned index, direction_s dr, creature* attacker);
	void				getheroes(creature** result, direction_s dir);
	int					getrandom(int type, race_s race, gender_s gender, int prev_name);
	unsigned			getrounds() const { return rounds; }
	int					getside(int side, direction_s dr);
	int					getsideb(int side, direction_s dr);
	direction_s			getdirection() const { return camera_direction; }
	int					getindex(const creature* p) const;
	creature*			getvalid(creature* pc, class_s type) const;
	wear_s				getwear(const item* itm) const;
	static bool			isalive();
	void				leavedungeon();
	bool				manipulate(item* itm, direction_s direction);
	void				passround();
	void				passtime(int minutes);
	bool				question(item* current_item);
	void				rideto(point overland_position);
	void				setcamera(short unsigned index, direction_s direction = Center);
	void				thrown(item* itm);
	bool				read();
	void				worldmap();
	void				write();
};
struct answers {
	struct element {
		int				id;
		const char*		text;
	};
	answers();
	adat<element, 32>	elements;
	void				add(int id, const char* name);
	int					choose(const char* title) const;
	int					choose(const char* title, bool interactive) const;
	int					choosebg(const char* title, const char* footer, const messagei::imagei* pi = 0, bool herizontal_buttons = true) const;
	int					choosesm(const char* title, bool allow_cancel = true) const;
	int					random() const;
	void				sort();
private:
	char				buffer[512];
	stringbuilder		sc;
};
namespace draw {
typedef void(*pevent)();
typedef void(*pobject)(void* object);
struct menu {
	pevent				proc;
	const char*			text;
	operator bool() const { return proc != 0; }
};
namespace animation {
void					attack(creature* attacker, wear_s slot, int hits);
void					clear();
void					damage(creature* target, int hits);
void					render(int pause = 300, bool show_screen = true, item* current_item = 0);
int						thrown(indext index, direction_s dr, item_s rec, direction_s sdr = Center, int wait = 100, bool block_monsters = false);
int						thrownstep(indext index, direction_s dr, item_s itype, direction_s sdr = Center, int wait = 100);
void					worldmap(int pause = 300, item* current_item = 0);
void					update();
}
typedef void(*infoproc)(item*);
void					abilities(int x, int y, creature* pc);
void					adventure();
void					appear(pobject proc, void* object, unsigned duration = 1000);
void					avatar(int x, int y, int party_index, unsigned flags, item* current_item);
void					avatar(int x, int y, creature* pc, unsigned flags, item* current_item);
void					background(int rid);
void*					choose(const array& source, const char* title, const void* object, const void* current, fntext pgetname, fnallow pallow, fndraw preview, int view_width);
bool					choose(const array& source, const char* title, const void* object, void* field, unsigned field_size, const fnlist& list);
void					chooseopt(const menu* source);
void					chooseopt(const menu* source, unsigned count, const char* title);
point					choosepoint(point camera);
bool					dlgask(const char* text);
bool					edit(const char* title, void* object, const markup* form);
void					editor();
void					fullimage(point camera, point* origin);
void					fullimage(point from, point to, point* origin);
void					mainmenu();
void					options();
void					pause();
void					redmarker(int x, int y);
void					setimage(const char* id);
void					setnext(void(*p)());
bool					settiles(resource_s id);
void					textbc(int x, int y, const char* header);
}
extern gamei			game;
extern dungeon			location_above;
extern dungeon			location;
extern variant			party[6];
inline int				gx(indext index) { return index % mpx; }
inline int				gy(indext index) { return index / mpx; }
indext					to(indext index, direction_s d);
void					mslog(const char* format, ...);
void					mslogv(const char* format, const char* vl);
direction_s				pointto(indext from, indext to);
int						rangeto(indext i1, indext i2);
direction_s				to(direction_s d, direction_s d1);
inline int				d100() { return rand() % 100; }
// Function get comon name
template<class T> const char* getnm(const void* object, stringbuilder& sb);
NOBSDATA(action)
NOBSDATA(abilitya)
NOBSDATA(dice)
NOBSDATA(itemi::weaponi)
NOBSDATA(itemi::armori)
NOBSDATA(messagei::imagei)
NOBSDATA(point)
NOBSDATA(sitei)
NOBSDATA(sitei::chancei)
NOBSDATA(sitei::crypti)
NOBSDATA(sitei::headi)
NOBSDATA(variant)
MNLNK(ability_s, abilityi)
MNLNK(alignment_s, alignmenti)
MNLNK(attack_s, attacki)
MNLNK(class_s, classi)
MNLNK(action_s, actioni)
MNLNK(damage_s, damagei)
MNLNK(enchant_s, enchanti)
MNLNK(feat_s, feati)
MNLNK(gender_s, genderi)
MNLNK(intellegence_s, intellegencei)
MNLNK(item_s, itemi)
MNLNK(item_feat_s, itemfeati)
MNLNK(monster_s, monsteri)
MNLNK(race_s, racei)
MNLNK(resource_s, resourcei)
MNLNK(size_s, sizei)
MNLNK(spell_s, spelli)
MNLNK(speech_s, speechi)
MNLNK(usability_s, usabilityi)
MNLNK(variant_s, varianti)
MNLNK(wear_s, weari)