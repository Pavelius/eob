#include "color.h"
#include "crt.h"
#include "dataset.h"
#include "dice.h"
#include "flagable.h"
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

enum {
	GP = 1
};
enum fcell : unsigned {
	Passable, EmpthyStartIndex,
	LookWall, LookOverlay, LookObject,
	PassableActivated
};
enum fevent_s : unsigned char {
	Wilderness, City, Starting
};
enum resource_s : unsigned char {
	NONE,
	BORDER, OUTTAKE, DECORS,
	CHARGEN, CHARGENB, COMPASS, INVENT, ITEMS, ITEMGS, ITEMGL,
	BLUE, BRICK, CRIMSON, DROW, DUNG, GREEN, FOREST, MEZZ, SILVER, XANATHA,
	MENU, PLAYFLD, INTRO, PORTM, THROWN, XSPL, WORLD,
	NPC, BPLACE, ADVENTURE, BUILDNGS, DUNGEONS, CRYSTAL, SCENES,
	// Monsters
	ANKHEG, ANT, BLDRAGON, BUGBEAR, CLERIC1, CLERIC2, CLERIC3, DRAGON, DWARF, FLIND,
	GHOUL, GOBLIN, GUARD1, GUARD2, KOBOLD, KUOTOA, LEECH, ORC,
	SHADOW, SKELETON, SKELWAR, SPIDER1, WIGHT, WOLF, ZOMBIE,
	Count
};
enum race_s : unsigned char {
	Dwarf, Elf, HalfElf, Halfling, Human,
	Humanoid, Goblinoid, Insectoid, Animal,
};
enum alignment_s : unsigned char {
	LawfulGood, NeutralGood, ChaoticGood, LawfulNeutral, TrueNeutral, ChaoticNeutral, LawfulEvil, NeutralEvil, ChaoticEvil,
	FirstAlignment = LawfulGood, LastAlignment = ChaoticEvil,
};
enum morale_s : unsigned char {
	Lawful, Good, Neutral, Chaotic, Evil,
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
	CreateFood, CureBlindnessDeafness, Disease, CureDisease, Haste, MageFear, NegativePlanProtection,
	Regeneration, RemoveCurse, RemoveParalizes,
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
	StaticObject,
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
	// Additional ability
	AC,
	AttackMelee, AttackRange, AttackAll,
	DamageMelee, DamageRange, DamageAll,
	Speed,
	BonusExperience, BonusSave, ReactionBonus,
	ExeptionalStrenght,
	Hits
};
enum wear_s : unsigned char {
	Backpack, LastBackpack = Backpack + 13,
	Head, Neck, Body, RightHand, LeftHand, RightRing, LeftRing, Elbow, Legs, Quiver,
	FirstBelt, SecondBelt, LastBelt,
	FirstInvertory = Backpack, LastInvertory = LastBelt
};
enum enchant_s : unsigned char {
	OfAccuracy, OfCold,
	OfEnergyDrain,
	OfFear, OfFire, OfHolyness,
	OfParalize, OfPoison, OfPoisonStrong,
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
	Necklage, NecklageRich, NecklageVeryRich,
	Arrow, Dart, Stone,
	Bones, DungeonMap,
	HolySymbol, HolySymbolEvil, MagicBook, TheifTools, MagicWand, MageScroll, PriestScroll,
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
	CellButton, CellPit,
	CellWeb, CellWebTorned,
	CellBarel, CellBarelDestroyed,
	CellEyeColumn,
	CellCocon, CellCoconOpened,
	CellGrave, CellGraveDesecrated,
	CellPitUp, // On floor
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
	NotForSale, Expandable, Unique
};
enum reaction_s : unsigned char {
	Indifferent, Friendly, Hostile,
};
enum intellegence_s : unsigned char {
	NoInt, AnimalInt, Semi, Low, Ave, Very, High, Exeptional, Genius, Supra, Godlike,
};
enum city_ability_s : unsigned char {
	Criminals, Donations, Gold, Prosperty, Reputation,
};
enum action_s : unsigned char {
	Reshufle,
	Attack, Bribe, Buy, Donate, Drink, Fun, HealAction, Gambling,
	Leave, Lie, Quest, Pay, Repair, Rest, Sacrifice, Sell,
	Talk, Take, Trade, Travel, Pet, Work,
	Experience,
};
enum talk_s : unsigned char {
	Greeting,
	FailLie, TalkAbout, TalkArtifact, TalkCursed, TalkMagic, TalkLoot, TalkLootCellar, TalkHistory, TalkRumor,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Action, ActionSet, Adventure, Alignment, Building, Case, Cell, Class,
	Cleveress, Company, Condition, Creature, Damage, Enchant, Event, Feat, Gender,
	Item, Morale, Race, Rarity, Reaction, Resource, Settlement, Spell,
};
enum case_s : unsigned char {
	Case1, Case2, Case3, Case4, Case5, Case6, Case7, Case8, Case9,
};
enum pack_s : unsigned char {
	PackDungeon, PackMonster, PackOuttake,
	PackInterface, PackBackground, PackCenter,
	Pack160x96, Pack320x120, Pack320x200
};
enum ambush_s : unsigned char {
	NoAmbush, MonsterAmbush, PartyAmbush
};
enum shape_s : unsigned char {
	ShapeCorner, ShapeRoom, ShapeRoomLarge, ShapeDeadEnd, ShapeHall,
};
enum building_s : unsigned char {
	Arena, Armory, Bank, Brothel, Inn, Library, Harbor, Prison, Stable, Stock, Tavern, Temple, WizardTower,
};
enum good_s : unsigned char {
	Armors, Books, Clothes, Devices, Food, Jewelry, Papers, Potions, Tools, Weapons
};
enum condition_s : unsigned char {
	Healed, BadlyWounded, Wounded,
};
enum actionf_s : unsigned char {
	CheckCondition, MayLoseReputation, DependOnReputation,
};
enum goal_s : unsigned char {
	KillBoss, KillAlmostAllMonsters, ExploreMostDungeon,
	GrabAlmostAllGems, GrabAllSpecialItems,
};
typedef short unsigned indext;
typedef cflags<action_s> actionf;
typedef cflags<good_s> goodf;
typedef cflags<variant_s> variantf;
typedef cflags<feat_s> feata;
typedef cflags<fevent_s> eventf;
typedef flagable<1 + LastSpellAbility / 8> spellf;
typedef flagable<4> flagf;
typedef adatc<ability_s, char, DetectSecrets + 1> skilla;
typedef cflags<usability_s> usabilitya;
typedef dataset<Reputation, int> citya;
class creature;
class creaturea;
class item;
struct adventurei;
struct settlementi;
struct variant {
	variant_s			type;
	unsigned char		value;
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(const variant_s t, unsigned char v) : type(t), value(v) {}
	constexpr variant(const ability_s v) : type(Ability), value(v) {}
	constexpr variant(const action_s v) : type(Action), value(v) {}
	constexpr variant(const alignment_s v) : type(Alignment), value(v) {}
	constexpr variant(const building_s v) : type(Building), value(v) {}
	constexpr variant(const case_s v) : type(Case), value(v) {}
	constexpr variant(const cell_s v) : type(Cell), value(v) {}
	constexpr variant(const class_s v) : type(Class), value(v) {}
	constexpr variant(const condition_s v) : type(Condition), value(v) {}
	constexpr variant(const damage_s v) : type(Damage), value(v) {}
	constexpr variant(const enchant_s v) : type(Enchant), value(v) {}
	constexpr variant(const feat_s v) : type(Feat), value(v) {}
	constexpr variant(const gender_s v) : type(Gender), value(v) {}
	constexpr variant(const intellegence_s v) : type(Cleveress), value(v) {}
	constexpr variant(const item_s v) : type(Item), value(v) {}
	constexpr variant(const morale_s v) : type(Morale), value(v) {}
	constexpr variant(const race_s v) : type(Race), value(v) {}
	constexpr variant(const rarity_s v) : type(Rarity), value(v) {}
	constexpr variant(const reaction_s v) : type(Reaction), value(v) {}
	constexpr variant(const resource_s v) : type(Resource), value(v) {}
	constexpr variant(const spell_s v) : type(Spell), value(v) {}
	constexpr variant(const int v) : type(variant_s((v >> 8) & 0xFF)), value(v & 0xFF) {}
	variant(variant_s v, const void* p);
	variant(const void* v);
	constexpr explicit operator bool() const { return type != NoVariant; }
	constexpr explicit operator int() const { return (type << 8) | value; }
	constexpr bool operator==(const variant& e) const { return type == e.type && value == e.value; }
	void				clear() { type = NoVariant; value = 0; }
	static variant		find(const char* name);
	auto				getadventure() const { return (adventurei*)getpointer(Adventure); }
	creature*			getcreature() const;
	void*				getpointer(variant_s t) const;
	point				getposition() const;
	auto				getsettlement() const { return (settlementi*)getpointer(Settlement); }
	const char*			getname() const;
};
typedef variant conditiona[6];
struct varianta : adat<variant, 12> {
};
struct textable {
	unsigned			name;
	constexpr textable() : name(0) {}
	constexpr explicit operator bool() const { return name != 0; }
	operator const char*() const { return getname(); }
	static bool			edit(void* object, const array& source, void* pointer);
	static bool			editrich(void* object, const array& source, void* pointer);
	const char*			getname() const;
	void				setname(const char* name);
	static array&		getstrings();
};
struct celli {
	const char*			name;
	resource_s			resource;
	int					frame;
	cflags<fcell>		flags;
	cell_s				destroyed;
};
struct spellprogi {
	char				elements[21][10];
};
struct actioni {
	const char*			name;
	bool				talk;
};
struct talki {
	const char*			name;
	cflags<reaction_s>	flags;
};
struct actionseti {
	const char*			name;
	action_s			action;
	int					count1, count2;
	cflags<actionf_s>	flags;
	int					count3;
	int					roll() const;
	bool				is(actionf_s v) const { return flags.is(v); }
};
struct abilityi {
	const char*			name;
	cflags<class_s>		match;
	bool				allow(class_s v) const;
	int					getmultiplier() const;
};
struct abilitya {
	char				data[Charisma + 1];
};
struct moralei {
	const char*			name;
};
struct alignmenti {
	const char*			name;
	morale_s			law;
	morale_s			morale;
	adat<class_s, 8>	restricted;
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
struct namei {
	race_s				race;
	gender_s			gender;
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
struct casei {
	const char*			name;
};
struct varianti {
	const char*			name;
	const char*			namepl;
	variantf			allowed;
	array*				source;
	fntext				pgetname;
	const markup*		form;
	static variant_s	find(const array* v);
	static varianti*	find(const markup* v);
	variant				find(const char* v) const;
	static void			getmetadata(stringbuilder& sb);
};
struct combati {
	char				number_attacks_p2r;
	damage_s			type;
	char				speed;
	dice				damage;
	char				bonus, critical_multiplier, critical_range;
	item*				weapon;
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
		const weaponi*	next;
		constexpr weaponi() : damage_large(), combati(), next(0) {}
		constexpr weaponi(char number_attack, damage_s type, char speed, dice damage, dice damage_large, char bonus = 0, const weaponi* next = 0) :
			combati{number_attack, type, speed, damage, bonus}, damage_large(damage_large), next(next) {}
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
	char				characters;
	const char*			name;
	abilitya			minimum;
	abilitya			maximum;
	abilitya			adjustment;
	feata				feats;
	usabilitya			usability;
	skilla				skills;
};
struct packi {
	const char*			id;
	const char*			url;
	bool				choose_frame;
};
struct resourcei {
	char				name[16];
	pack_s				pack;
	void*				data;
	const packi&		gete() const { return bsdata<packi>::elements[pack]; }
	const char*			geturl() const;
	bool				isdungeon() const;
	bool				ismonster() const;
	static resourcei*	find(const char* id, unsigned size);
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
struct selli {
	item_s				object;
	rarity_s			rarity;
	constexpr selli(item_s object) : object(object), rarity(Common) {}
	constexpr selli(item_s object, rarity_s rarity) : object(object), rarity(rarity) {}
};
struct imagei {
	resource_s			res;
	unsigned short		frame;
	constexpr explicit operator bool() const { return res != NONE; }
	void				add(stringbuilder& sb) const;
	const resourcei&	gete() const { return bsdata<resourcei>::elements[res]; }
	static bool			choose(void* object, const array& source, void* pointer);
};
struct resultable : public textable {
	conditiona			actions;
	bool				isallow() const;
	settlementi*		getsettlement() const;
	bool				have(variant v) const;
};
struct eventi : textable {
	enum flag_s : unsigned char {
		Start, Wilderness,
	};
	textable			id;
	textable			ask[2];
	resultable			results[4];
	unsigned char		flags;
	void				apply(case_s v, bool interactive) const;
	void				clear();
	void				discard() const;
	bool				is(flag_s v) const { return (flags & (1 << v)) != 0; }
	void				play() const;
	void				set(flag_s v) { flags |= 1 << v; }
	void				shufle() const;
};
class deck : adat<unsigned char, 256> {
	variant_s			type;
public:
	void				addbottom(variant v);
	void				create(variant_s v);
	void				discard(variant v);
	void				shuffle();
	variant				getbottom();
	variant				gettop();
};
struct sitei {
	struct headi {
		resource_s		type;
		monster_s		habbits[2]; // Who dwelve here
		item_s			keys[2]; // Two keys open all doors
		item_s			wands; // Special find somewhere
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
			unsigned char identified : 1; // All properties of item will be known
			unsigned char cursed : 1; // Negative effect
			unsigned char broken : 1; // Next breaking destroy item
			unsigned char started : 1; // Item is generated from start
		};
	};
	unsigned char		subtype; // spell scroll or spell of wand
	unsigned char		charges; // uses of item
	friend dginf<item>;
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
	static bool			choose_enchantment(void* object, const array& source, void* pointer);
	bool				damage(const char* text_damage, const char* text_brokes);
	void				finish();
	void				get(combati& result, size_s enemy_size) const;
	item_s				getammo() const { return gete().ammo; }
	int					getarmorpenalty(ability_s skill) const;
	int					getcost() const;
	int					getcostgp() const { return gete().costgp; }
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
struct buildingi {
	const char*			name;
	const char*			description;
	actionf				actions;
	goodf				goods;
	shape_s				shape;
	bool				is(action_s v) const { return actions.is(v); }
};
struct boosti {
	variant				owner;
	spell_s				id;
	unsigned			round;
	constexpr explicit operator bool() const { return owner.type != NoVariant; }
	void				clear();
};
struct speechi {
	const char*			name;
};
class itema : public adat<item*, 48> {
	typedef bool (item::*pitem)() const;
	void				select(pitem proc, bool keep);
public:
	void				broken(bool keep) { select(&item::isbroken, keep); }
	item*				choose(const char* format, bool cancel_button, fntext panel = 0);
	item*				choose(const char* format, bool* cancel_button, const creature* current, const creaturea* allowed, creature** change, fntext getname = 0) const;
	void				cost(bool keep) { select(&item::iscost, keep); }
	void				costgp(bool keep) { select(&item::iscostgp, keep); }
	void				cursed(bool keep) { select(&item::iscursed, keep); }
	void				identified(bool keep) { select(&item::isidentified, keep); }
	void				is(good_s v, bool keep);
	void				havespell(const creature* pc, bool keep);
	void				magical(bool keep) { select(&item::ismagical, keep); }
	void				match(rarity_s v, bool keep);
	void				match(const goodf& e, bool keep);
	void				match(const item::typea& v, bool keep);
	void				maxcost(int v, bool keep);
	void				forsale(bool keep);
	item*				random();
	void				select();
	void				select(adat<item>& source);
	void				sort();
};
class nameable {
	monster_s			kind = NoMonster;
	race_s				race = Human;
	gender_s			gender = Male;
	unsigned short		name = 0;
	friend dginf<nameable>;
public:
	constexpr gender_s	getgender() const { return gender; }
	const monsteri&		getmonster() const { return bsdata<monsteri>::elements[kind]; }
	const char*			getname() const;
	constexpr race_s	getrace() const { return race; }
	bool				is(monster_s v) const { return kind == v; }
	bool				ishero() const;
	constexpr bool		ismonster() const { return kind != NoMonster; }
	void				kill();
	void				random_name();
	void				say(const char* format, ...) const;
	void				say(const item& it, const char* format) const;
	void				say(spell_s id) const;
	void				sayv(const char* format, const char* vl) const;
	void				set(gender_s v) { gender = v; }
	void				set(monster_s v) { kind = v; }
	void				set(race_s v) { race = v; }
};
// Conceptual ability storages
struct statable {
	char				ability[ExeptionalStrenght + 1] = {};
	spellf				active_spells;
	feata				feats;
	usabilitya			usability = {};
	short				hits_rolled = 0;
	void				add(ability_s id, class_s type, const char* levels);
	void				add(ability_s id, class_s type, int level);
	void				add(ability_s id, class_s type);
	void				apply(const item& it, bool use_spells);
	void				apply(variant v, int magic, bool use_spells);
	int					getstrex() const;
	static int			getthac0(class_s type, int level);
	constexpr bool		is(spell_s v) const { return active_spells.is(v); }
	constexpr bool		is(feat_s v) const { return feats.is(v); }
	constexpr bool		is(usability_s v) const { return usability.is(v); }
	void				random_ability(race_s race, gender_s gender, class_s type);
	void				update_attacks(class_s type, int level);
	void				update_stats();
};
class creature : public statable, public nameable {
	statable			basic;
	alignment_s			alignment = LawfulGood;
	class_s				type = NoClass;
	indext				index = Blocked;
	unsigned char		side = 0;
	direction_s			direction = Up;
	short				hits = 0, hits_aid = 0;
	char				initiative = 0;
	char				levels[3] = {};
	item				wears[LastInvertory + 1] = {};
	char				spells[LastSpellAbility + 1] = {};
	char				prepared[LastSpellAbility + 1] = {};
	spellf				known_spells;
	char				avatar = 0;
	unsigned			experience = 0;
	char				drain_energy = 0, drain_strenght = 0, disease_progress = 0;
	char				pallette = 0;
	short				food = 0;
	reaction_s			reaction = Indifferent;
	//
	void				addboost(spell_s id, unsigned duration);
	void				attack_drain(creature* defender, char& value, int& hits);
	void				campcast(item& it);
	int					get_base_save_throw(ability_s st) const;
	class_s				getbestclass() const { return getclass(getclass(), 0); }
	void				prepare_random_spells(class_s type, int level);
	char				racial_bonus(char* data) const;
	void				raise_level(class_s type);
	void				random_spells(class_s type, int level, int count);
	void				update_poison(bool interactive);
	void				update_wears();
	friend dginf<creature>;
public:
	explicit operator bool() const { return ability[Strenght] != 0; }
	void				activate(spell_s v) { active_spells.set(v); }
	bool				add(item i);
	bool				add(spell_s type, unsigned duration = 0, save_s id = NoSave, char save_bonus = 0, ability_s save_type = SaveVsMagic);
	void				addaid(int v) { hits_aid += v; }
	void				addexp(int value);
	void				apply(spell_s id, int level);
	void				attack(indext index, direction_s d, int bonus, bool ranged, int multiplier);
	void				attack(creature* defender, wear_s slot, int bonus, int multiplier);
	void				autocast(creaturea& friends);
	bool				cast(spell_s id, class_s type, int wand_magic, creature* target = 0);
	void				create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive = false);
	void				clear();
	bool				canspeak(race_s language) const;
	static alignment_s	choosealignment(bool interactive, class_s depend);
	static class_s		chooseclass(bool interactive, race_s depend);
	static race_s		chooserace(bool interactive);
	spell_s				choosespell(class_s type) const;
	void				damage(damage_s type, int hits, int magic_bonus = 0);
	void				enchant(spell_s id, int level);
	void				equip(item it);
	void				exhause();
	item*				find(item_s v) const;
	void				finish();
	void				flee(bool interactive);
	static creature*	get(void* focus);
	int					get(ability_s id) const { return ability[id]; }
	int					get(class_s id) const;
	int					get(spell_s spell) const { return spells[spell]; }
	void				get(combati& e, wear_s slot, size_s enemy_size) const;
	item				get(wear_s id) const;
	const spellprogi*	getprogress(class_s v) const;
	alignment_s			getalignment() const { return alignment; }
	int					getavatar() const { return avatar; }
	int					getawards() const;
	class_s				getcaster() const;
	int					getcasterlevel(class_s id) const;
	class_s				getclass() const { return type; }
	static class_s		getclass(class_s id, int index);
	int					getclasscount() const;
	direction_s			getdirection() const;
	int					getexperience() const { return experience; }
	int					getfood() const { return food; }
	int					getfoodmax() const;
	int					gethd() const;
	dice				gethitdice() const;
	int					gethitpenalty(int bonus) const;
	short				gethits() const { return hits + hits_aid; }
	short				gethitsmaximum() const;
	short unsigned		getindex() const;
	int					getinitiative() const { return initiative; }
	item*				getitem(wear_s id) { return &wears[id - FirstInvertory]; }
	static int			getlevel(spell_s id, class_s type);
	int					getpallette() const { return pallette; }
	int					getpartyindex() const;
	int					getprepare(spell_s v) const { return prepared[v]; }
	reaction_s			getreaction() const { return reaction; }
	resource_s			getres() const;
	int					getside() const;
	size_s				getsize() const;
	wear_s				getslot(const item* p) const;
	int					getspeed() const;
	int					getspellsperlevel(class_s type, int spell_level) const;
	int					getstrex() const;
	bool				have(aref<class_s> source) const;
	bool				have(item_s v) const;
	void				heal(bool interactive) { damage(Heal, gethitsmaximum()); }
	bool				haveforsale() const;
	bool				is(condition_s v) const;
	bool				is(intellegence_s v) const;
	bool				is(monster_s v) const { return nameable::is(v); }
	bool				is(feat_s v) const { return feats.is(v); }
	bool				is(morale_s v) const;
	constexpr bool		is(spell_s v) const { return statable::is(v); }
	bool				is(usability_s v) const { return usability.is(v); }
	static bool			isallow(class_s id, race_s r);
	static bool			isallow(alignment_s id, class_s c);
	bool				isallow(const item it, wear_s slot) const;
	bool				isallowremove(const item i, wear_s slot, bool interactive);
	bool				iscast(class_s v) const { return getprogress(v) != 0; }
	bool				isenemy(creature* target) const;
	bool				isknown(spell_s v) const { return known_spells.is(v); }
	bool				ismatch(const variant v) const;
	bool				ismatch(const conditiona& v) const;
	bool				ismindless() const;
	bool				ismoved() const { return is(Moved); }
	bool				isready() const;
	bool				isspecialize(item_s weapon) const;
	bool				isuse(const item v) const;
	void				kill();
	void				poison(save_s save, char save_bonus = 0);
	void				preparespells();
	static void			preparespells(class_s type);
	void				random_equipment(int level);
	void				remove(spell_s v);
	bool				remove(wear_s slot, bool interactive);
	void				removeboost(spell_s v);
	void				removeloot();
	int					render_ability(int x, int y, int width, unsigned flags) const;
	int					render_combat(int x, int y, int width, unsigned flags) const;
	void				resting(int healed);
	bool				roll(ability_s id, int bonus = 0) const;
	reaction_s			rollreaction(int bonus) const;
	static void			roll_character();
	bool				save(int& value, ability_s skill, save_s type, int bonus);
	void				satisfy();
	void				scribe(item& it);
	static unsigned		select(spell_s* result, const spell_s* result_maximum, class_s type, int level);
	void				select(itema& result);
	void				set(ability_s id, int v) { ability[id] = v; }
	void				set(alignment_s value) { alignment = value; }
	void				set(class_s value) { type = value; }
	void				set(monster_s type);
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
	bool				setweapon(item_s v, int charges);
	void				subenergy();
	static bool			swap(item* itm1, item* itm2);
	void				turnundead(int level);
	void				uncurse(bool interactive);
	static void			update_boost_effects();
	static void			update_class(void* object);
	void				update_finish();
	void				update_hour(bool interactive);
	void				update_levelup(bool interactive);
	static void			update_race(void* object);
	void				update_start();
	void				update_turn(bool interactive);
	bool				use(ability_s id, indext index, int bonus, bool* firsttime, int exp, bool interactive);
	static bool			use(item* pi);
	bool				useammo(item_s ammo, wear_s slot, bool use_item);
	bool				usequick();
	void				view_ability();
	static void			view_party();
	void				view_portrait(int x, int y) const;
};
class creaturea : public adat<creature*, 12> {
	typedef void (creature::*fnapply)();
	void				apply(fnapply p);
public:
	bool				additem(item it, bool intractive = false);
	creature*			choose() const;
	void				exhause() { apply(&creature::exhause); }
	int					getaverage(ability_s v) const;
	creature*			getbest(ability_s v) const;
	creature*			getmostdamaged() const;
	bool				have(variant v) const;
	void				kill();
	void				leave();
	void				match(variant v, bool keep);
	void				match(const conditiona& id, bool keep);
	void				resolve();
	void				rollinitiative();
	void				satisfy() { apply(&creature::satisfy); }
	void				select();
	void				select(indext index);
	void				set(reaction_s v);
};
struct shapei {
	const char*			id;
	point				size;
	const char*			data;
	const char*			getvertical() const;
	const char*			get(direction_s d, point& result_size) const;
};
struct dungeoni {
	struct overlayi {
		cell_s			type; // type of overlay
		direction_s		dir; // overlay direction
		indext			index; // index
		indext			index_link; // linked to this location
		short unsigned	subtype; // depends on value type
		short unsigned	flags;
		constexpr explicit operator bool() const { return index != Blocked; }
		void			clear();
		bool			is(overlay_flag_s v) const { return (flags & (1 << v)) != 0; }
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
		overlayi		up, down; // where is stairs located
		overlayi		portal; // where is portal
		overlayi		lair, lair_button; // where is lair and opening button located
		overlayi		wands; // where is crypt located
		indext			spawn[2]; // new monster appera here
		monster_s		boss; // Type of boss;
		unsigned char	messages; // count of messages
		unsigned char	secrets; // count of secret rooms
		unsigned char	artifacts; // count of artifact items
		unsigned char	rings; // count of magical rings
		unsigned char	weapons; // count of magical weapons
		unsigned char	elements; // count of corridors
		unsigned char	traps; // count of traps
		unsigned char	bones; // count of bones
		unsigned char	gems; // count of gems
		unsigned char	relicts; // count of books and holy symbols
		short unsigned	items; // total count of items
		short unsigned	overlays; // total count of overlays
		short unsigned	monsters; // total count of monsters
		short unsigned	monsters_alive; // total alive monsters
		short unsigned	monsters_killed; // total killed monsters
		flagf			goals;
		bool			boss_alive;
		void			clear();
	};
	struct eventi {
		variant			owner;
		ability_s		skill;
		indext			index;
		constexpr operator bool() const { return owner.operator bool(); }
	};
	unsigned char		overland_index;
	unsigned char		level;
	sitei::headi		head;
	statei				stat;
	sitei::chancei		chance;
	unsigned char		data[mpx * mpy];
	groundi				items[512];
	overlayi			overlays[256];
	overlayitem			cellar_items[256];
	creature			monsters[200];
	eventi				events[256];
	dungeoni() { clear(); }
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
	void				dropitem(indext index, item rec, int side);
	void				dropitem(item* pi, int side = -1);
	void				explore(indext index, int r = 1);
	void				examine(creature* pc, overlayi* po);
	void				fill(indext index, int sx, int sy, cell_s value);
	void				finish(cell_s t);
	void				formation(indext index, direction_s dr);
	cell_s				get(indext index) const;
	cell_s				get(int x, int y) const;
	short unsigned		gettarget(indext start, direction_s dir);
	void				getblocked(indext* pathmap, bool treat_door_as_passable);
	int					getfreeside(creature** sides);
	int					getfreeside(indext index);
	unsigned			getitemcount(item_s type) const;
	unsigned			getitems(item** result, item** result_maximum, indext index, int side = -1);
	unsigned			getitems(item** result, item** result_maximum, overlayi* povr);
	int					getitemside(item* pi);
	static indext		getindex(int x, int y);
	race_s				getlanguage() const;
	unsigned			getmonstercount() const;
	void				getmonsters(creature** result, indext index, direction_s dr);
	item_s				getkeytype(cell_s keyhole) const;
	indext				getnearest(indext index, int radius, cell_s t1);
	short unsigned*		getnearestfree(indext* indicies, indext index);
	const char*			getnavigation(indext index) const;
	direction_s			getpassable(indext index, direction_s* dirs);
	indext				getretreat(indext from, int radius) const;
	indext				getsecret() const;
	static size_s		getsize(creature** sides);
	overlayi*			getoverlay(indext index, direction_s dir);
	overlayi*			getoverlay(const overlayitem& e) { return &overlays[e.storage_index]; }
	cell_s				gettype(cell_s id);
	cell_s				gettype(overlayi* po);
	indext				getvalid(indext index, int width, int height, cell_s v) const;
	void				hearnoises();
	constexpr bool		is(goal_s v) const { return stat.goals.is(v); }
	bool				is(indext index, cell_flag_s value) const;
	bool				is(indext index, int width, int height, cell_s v) const;
	bool				is(const rect& rc, cell_s id) const;
	bool				isactive(const overlayi* po);
	bool				isblocked(indext index) const;
	bool				isblocked(indext index, int side) const;
	bool				islineh(indext index, direction_s dir, int count, cell_s t1, cell_s t2 = CellUnknown) const;
	bool				islying(indext index, item_s type) const;
	bool				ismatch(indext index, cell_s t1, cell_s t2);
	bool				ismonster(indext index) const;
	bool				ismonsternearby(indext i, int r = 3) const;
	bool				isroom(indext index, direction_s dir, int side, int height) const;
	static bool			isvisible(indext index);
	dungeoni::overlayi*	getlinked(indext index);
	void				makedoor(const rect& rc, overlayi& door, direction_s dir, bool has_button, bool has_button_on_other_side);
	void				makeroom(const rect& rc, overlayi& door);
	static void			makewave(indext start, indext* pathmap);
	void				move(indext index, direction_s dr);
	bool				move(direction_s direction);
	void				passhour();
	void				passround();
	void				pickitem(item* itm, int side = -1);
	short unsigned		random(indext* indicies);
	bool				read(short unsigned overland, char level);
	void				remove(indext index, cell_flag_s value);
	void				remove(overlayi* po, item it);
	void				remove(overlayi* po);
	void				rotate(direction_s direction);
	void				set(goal_s v) { stat.goals.set(v); }
	void				set(indext index, cell_s value);
	void				set(indext index, cell_flag_s value);
	void				set(indext index, direction_s dir, cell_s type);
	void				set(indext index, reaction_s v);
	void				set(indext index, direction_s dir, shape_s type, point& size, indext* indecies, bool run = true);
	void				setactive(overlayi* po, bool active);
	void				setactive(indext index, bool value);
	void				setactive(indext index, bool value, int radius);
	void				stop(indext index);
	void				traplaunch(indext index, direction_s dir, item_s show, const combati& e);
	void				turnto(indext index, direction_s dr, bool* surprise = 0);
	void				write();
	void				update_goals();
};
struct indexa : adat<indext, 8 * 8> {
	void				select(const dungeoni& e, indext i, cell_s id, int r);
};
struct historyi {
	static constexpr unsigned history_max = 12;
	textable			history[history_max];
	unsigned char		history_progress;
	unsigned			gethistorymax() const;
};
struct adventurei : textable, historyi {
	unsigned char		stage; // 0 - non active
	textable			message_before;
	textable			message_agree;
	textable			message_entering;
	textable			rumor_activate;
	textable			message_done;
	textable			message_fail;
	point				position;
	sitei				levels[8];
	void				activate() { if(!stage) stage++; }
	void				create(bool interactive) const;
	bool				isactive() const { return stage > 0; }
	bool				isrumor() const { return rumor_activate && (stage == 0); }
	static void			play();
};
struct settlementi : textable {
	imagei				image;
	point				position;
	textable			description;
	cflags<building_s>	buildings;
	spellf				spells;
	unsigned char		prosperty;
	char				mood_tavern, mood_inn, mood_other;
	bool				apply(building_s b, action_s a, bool run);
	variant				enter();
	action_s			enter(building_s id);
	int					getdrinkcost() const { return 1 + getrarity() / 2; }
	int					gethealingcost() const;
	int					getequipmentcost(adventurei& e) const;
	void				getdescriptor(stringbuilder& sb) const;
	rarity_s			getrarity() const;
	int					getrestcost(building_s b) const;
	bool				is(building_s v) const { return buildings.is(v); }
	void				play();
	void				update();
};
struct companyi : textable {
	unsigned char		start; // starting settlement
	textable			intro; // When start new game
	int					start_gold;
	int					pixels_per_day;
	bool				read(const char* name);
	void				write(const char* name);
};
struct chati {
	talk_s				action;
	conditiona			conditions;
	const char*			text;
	constexpr operator bool() const { return text[0] != 0; }
	const char*			find(const aref<variant>& variants) const;
};
struct encounteri : public creaturea {
	reaction_s			reaction;
	talk_s				next;
	encounteri() : creaturea(), reaction(Indifferent), next(Greeting) {}
	bool				apply(action_s id, bool run);
	creature*			getleader() const;
	bool				match(const conditiona& e) const;
	void				dialog();
	void				set(reaction_s v);
};
class gamei : public companyi, citya {
	indext				camera_index;
	direction_s			camera_direction;
	char				location_level;
	unsigned short		adventure_index;
	unsigned			rounds;
	unsigned			rounds_turn;
	unsigned			rounds_hour;
	unsigned			rounds_daypart;
	unsigned			killed[LastMonster + 1];
	unsigned			found_secrets, gold_donated;
	variant				players[6];
public:
	void				add(city_ability_s i, int v) { citya::add(i, v); }
	void				add(creature* v);
	void				add(monster_s id) { killed[id]++; }
	void				addgold(int coins);
	void				addexp(morale_s id, unsigned v);
	void				addexpc(unsigned v, int killing_hit_dice);
	void				apply(variant v);
	void				attack(indext index, bool ranged, ambush_s ambush);
	void				camp(item& it);
	static adventurei*	chooseadventure();
	void				clear();
	void				clearfiles();
	void				endround();
	void				enter(unsigned short index, char level, bool set_camera = true);
	void				equiping();
	adventurei*			getadventure();
	void				findsecrets();
	constexpr int		get(city_ability_s id) const { return citya::get(id); }
	int					get(action_s id) const;
	int					getaverage(ability_s v) const;
	static int			getavatar(race_s race, gender_s gender, class_s cls);
	static int			getavatar(unsigned short* result, race_s race, gender_s gender, class_s cls);
	indext				getcamera() const { return camera_index; }
	static unsigned long long getchecksum();
	creature*			getdefender(short unsigned index, direction_s dr, creature* attacker);
	int					gethour() const { return (rounds % (24 * 60)) / 60; }
	int					getgold() const { return citya::get(Gold); }
	void				getheroes(creature** result, direction_s dir);
	static int			getmapheight();
	static int			getmapwidth();
	static int			getrandom(race_s race, gender_s gender);
	unsigned			getrounds() const { return rounds; }
	int					getside(int side, direction_s dr);
	int					getsideb(int side, direction_s dr);
	direction_s			getdirection() const { return camera_direction; }
	int					getindex(const creature* p) const;
	creature*			getvalid(creature* pc, class_s type) const;
	wear_s				getwear(const item* itm) const;
	void				interract(indext index);
	bool				is(variant v) const;
	static bool			isalive();
	bool				isnight() const;
	bool				manipulate(item* itm, direction_s direction);
	static void			newgame();
	void				passround();
	void				passtime(int minutes);
	void				pay(int coins) { addgold(-coins); }
	static void			play();
	void				preserial(bool writemode);
	bool				question(item* current_item);
	bool				read();
	bool				readtext(const char* url);
	void				returntobase();
	static bool			roll(int value);
	static void			scriblescrolls();
	void				set(city_ability_s i, int v) { citya::set(i, v); }
	void				setcamera(indext index, direction_s direction = Center);
	void				startgame();
	void				thrown(item* itm);
	void				write();
	static bool			writemeta(const char* url);
	static bool			writetext(const char* url, std::initializer_list<variant_s> source);
	static bool			writetext(const char* url, variant_s id) { return writetext(url, {id}); }
};
struct richtexti {
	static constexpr int maximum = 6;
	imagei				images[maximum];
	char				data[maximum][260];
	bool				load(const char* source);
	static const char*	parse(const char* p, imagei& ei, char* ps, const char* pe);
	void				save(textable& result) const;
};
class variantc : public adat<variant> {
	typedef bool(adventurei::*fnadventure)() const;
	void				match(fnadventure p, bool keep);
public:
	void				cspells(const creature* p, bool expand);
	int					chooselv(class_s type) const;
	void				exclude(variant v);
	void				match(variant v, bool keep);
	void				match(point start, int radius, bool keep);
	void				matchrm(bool keep) { match(&adventurei::isrumor, keep); }
	void				matchac(bool keep) { match(&adventurei::isactive, keep); }
	void				matchsl(class_s type, int level);
	void				select(variant_s type);
	void				sort();
};
class answers {
	char				buffer[512];
	stringbuilder		sc;
	struct element {
		int				id;
		const char*		text;
	};
	static imagei		last_image;
public:
	answers();
	adat<element, 32>	elements;
	void				add(int id, const char* name, ...) { addv(id, name, xva_start(name)); }
	void				addv(int id, const char* name, const char* format);
	int					choose(const char* title) const;
	int					choose(const char* title, bool interactive) const;
	int					choose(const char* format, const char* header, int* current_level, creature** current_creature, const creaturea* allowed, fnint getnumber) const;
	int					choosebg(const char* title, const imagei& ei, bool herizontal_buttons) const;
	int					choosebg(const char* title, bool herizontal_buttons = true) const;
	int					choosesm(const char* title, bool allow_cancel = true) const;
	int					choosemn(const char* title, bool allow_cancel = true) const;
	int					choosemn(int x, int y, int width, resource_s id) const;
	static int			compare(const void* v1, const void* v2);
	static void			clearimage();
	static void			message(const char* format);
	static void			message(const char* format, const imagei& im);
	int					random() const;
	static void			set(const imagei& v) { last_image = v; }
	void				sort();
};
namespace draw {
typedef void(*pobject)(void* object);
namespace animation {
void					attack(creature* attacker, wear_s slot, int hits);
void					clear();
void					damage(creature* target, int hits);
void					render(int pause = 300, bool show_screen = true, void* focus = 0, const imagei* pi = 0);
int						thrown(indext index, direction_s dr, item_s rec, direction_s sdr = Center, int wait = 100, bool block_monsters = false);
int						thrownstep(indext index, direction_s dr, item_s itype, direction_s sdr = Center, int wait = 100);
void					update();
}
typedef void(*infoproc)(void*);
extern unsigned			frametick;
int						ciclic(int range, int speed = 1);
void					focusing(const rect& rc, void* v, unsigned param = 0);
rect					form(rect rc, int count = 1, bool focused = false, bool pressed = false);
infoproc				getmode();
void*					getfocus();
void					greenbar(rect rc, int vc, int vm);
int						header(int x, int y, const char* text);
void					initialize();
bool					isfocus(void* ev, unsigned param = 0);
void					itemicn(int x, int y, item itm, unsigned char alpha = 0xFF, int spell = 0);
void					itemicn(int x, int y, item* pitm, bool invlist = false, unsigned flags = 0, void* current_item = 0);
void					logs();
bool					navigate(bool can_cancel = false);
void					redraw();
void					resetres();
void					setbigfont();
void					setmode(infoproc mode);
void					setsmallfont();
void					appear(pobject proc, void* object, unsigned duration = 1000);
void					application();
void					avatar(int x, int y, int party_index, unsigned flags, void* current_item);
void					avatar(int x, int y, creature* pc, unsigned flags, void* current_item);
void					background(int rid);
void*					choose(array& source, const char* title, void* object, const void* current, fntext pgetname, fnallow pallow, fndraw preview, int view_width, const markup* type = 0);
bool					choose(array& source, const char* title, void* object, void* field, unsigned field_size, const fnlist& list);
point					choosepoint(point camera);
void					closeform();
bool					dlgask(const char* text);
void					dlgmsg(const char* text);
bool					edit(const char* title, void* object, const markup* form, bool cancel_button);
void					fullimage(point camera, point* origin);
void					fullimage(point from, point to, point* origin);
void					mainmenu();
void					openform();
void					options(bool camp_mode);
void					pause();
void					redmarker(int x, int y);
void					setnext(void(*p)());
bool					settiles(resource_s id);
void					textbc(int x, int y, const char* header);
}
namespace colors {
extern color			title;
namespace info {
extern color			text;
}
}
extern gamei			game;
extern dungeoni			location_above;
extern dungeoni			location;
extern creaturea		party;
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
NOBSDATA(abilitya)
NOBSDATA(dice)
NOBSDATA(imagei)
NOBSDATA(item)
NOBSDATA(itemi::weaponi)
NOBSDATA(itemi::armori)
NOBSDATA(historyi)
NOBSDATA(point)
NOBSDATA(resultable)
NOBSDATA(sitei)
NOBSDATA(textable)
NOBSDATA(variant)
MNLNK(ability_s, abilityi)
MNLNK(alignment_s, alignmenti)
MNLNK(building_s, buildingi)
MNLNK(class_s, classi)
MNLNK(action_s, actioni)
MNLNK(damage_s, damagei)
MNLNK(enchant_s, enchanti)
MNLNK(feat_s, feati)
MNLNK(gender_s, genderi)
MNLNK(intellegence_s, intellegencei)
MNLNK(item_s, itemi)
MNLNK(item_feat_s, itemfeati)
MNLNK(morale_s, moralei)
MNLNK(monster_s, monsteri)
MNLNK(race_s, racei)
MNLNK(resource_s, resourcei)
MNLNK(size_s, sizei)
MNLNK(spell_s, spelli)
MNLNK(usability_s, usabilityi)
MNLNK(variant_s, varianti)
MNLNK(wear_s, weari)