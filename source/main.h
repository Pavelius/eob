#include "dice.h"
#include "collection.h"
#include "color.h"
#include "crt.h"
#include "rect.h"
#include "stringcreator.h"

#define assert_enum(e, last) static_assert(sizeof(bsmeta<e##i>::elements) / sizeof(bsmeta<e##i>::elements[0]) == last + 1, "Invalid count of " #e " elements");\
template<> const char* getstr<e##_s>(const e##_s i) { return bsmeta<e##i>::elements[i].name; }

const unsigned short	Blocked = 0xFFFF;
const int				walls_frames = 9;
const int				walls_count = 6;
const int				door_offset = 1 + walls_frames * walls_count;
const int				decor_offset = door_offset + 9;
const int				decor_count = 19;
const int				decor_frames = 10;
const int				scrx = 22 * 8;
const int				scry = 15 * 8;
const int				mpx = 30;
const int				mpy = 22;

enum resource_s : unsigned char {
	NONE,
	SCENES,
	CHARGEN, CHARGENB, COMPASS, INVENT, ITEMS, ITEMGS, ITEMGL,
	AZURE, BLUE, BRICK, CRIMSON, DROW, DUNG, GREEN, FOREST, MEZZ, SILVER, XANATHA,
	MENU, PLAYFLD, PORTM, THROWN, XSPL,
	// Monsters
	ANKHEG, ANT, BLDRAGON, BUGBEAR, CLERIC1, CLERIC2, CLERIC3, DRAGON, FLIND,
	GHOUL, GOBLIN, GUARD1, GUARD2, KOBOLD, KUOTOA, LEECH, ORC,
	SKELETON, SKELWAR, SPIDER1, ZOMBIE,
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
	Duration5PerLevel,
	DurationTurn, DurationTurnPerLevel,
	DurationHour, Duration2Hours, Duration4Hours, Duration8Hours,
};
enum target_s : unsigned char {
	TargetSelf,
	TargetThrow, TargetAllThrow,
	TargetClose, TargetAllClose,
	TargetAlly, TargetAllAlly,
	TargetSpecial,
};
enum message_s : unsigned char {
	MessageHabbits, MessageMagicWeapons, MessageMagicRings, MessageSecrets, MessageTraps,
	MessageAtifacts,
};
enum spell_s : unsigned char {
	NoSpell,
	// Spells (level 1)
	Bless, BurningHands, CureLightWounds, DetectEvil, DetectMagic, FeatherFall,
	MageArmor, MagicMissile,
	ProtectionFromEvil, PurifyFood,
	SpellReadLanguages, SpellShield, Sleep,
	// Specila ability
	LayOnHands, TurnUndead,
	FirstSpellAbility = LayOnHands, LastSpellAbility = TurnUndead,
};
enum command_s : unsigned char {
	NoCommand,
	PrayForSpells, MemorizeSpells, ChooseLevels, ChooseSpells,
	NewGame, LoadGame, SaveGame, QuitGame, Settings,
	Clear, OK, Cancel, Delete, Rename,
	LeaveArea, GoingUp, GoingDown, DropDown,
	Roll, Keep, NextPortrait, PreviousPortrait,
	GameOver, PassSegment
};
enum class_s : unsigned char {
	NoClass,
	Cleric, Fighter, Mage, Paladin, Ranger, Theif,
	FighterCleric, FighterMage, FighterTheif, FighterMageTheif,
	ClericTheif, MageTheif,
};
enum monster_s : unsigned char {
	NoMonster,
	AntGiant, Bugbear, ClericOldMan, Gnoll, Ghoul, Goblin, Kobold, Kuotoa, Leech,
	Orc, Skeleton, Spider, Zombie
};
enum dungeon_s : unsigned char {
	AreaDrow, AreaDungeon, AreaDwarven, AreaElf, AreaForest, AreaMagic, AreaSewers, AreaTemple
};
enum state_s : unsigned char {
	NoState,
	Armored, Blessed, Climbed, DetectedEvil, DetectedMagic,
	FireResisted, Invisibled, Hasted,
	ProtectedFromEvil, Shielded, Sleeped, StateSpeakable, Strenghted,
	Scared, Paralized,
	WeakPoison, Poison, StrongPoison, DeadlyPoison,
	LastState = DeadlyPoison,
};
enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
enum skill_s : unsigned char {
	SaveVsParalization, SaveVsPoison, SaveVsTraps, SaveVsMagic,
	ClimbWalls, HearNoise, MoveSilently, OpenLocks, RemoveTraps, ReadLanguages,
	LearnSpell,
	ResistCharm, ResistCold, ResistFire,
	CriticalDeflect,
	DetectSecrets,
	FirstSave = SaveVsParalization, LastSave = SaveVsMagic,
	FirstResist = ResistFire, LastResist = ResistCharm,
	LastSkill = DetectSecrets
};
enum wear_s : unsigned char {
	Backpack, LastBackpack = Backpack + 13,
	Head, Neck, Body, RightHand, LeftHand, RightRing, LeftRing, Elbow, Legs, Quiver,
	FirstBelt, SecondBelt, LastBelt,
	FirstInvertory = Backpack, LastInvertory = LastBelt
};
enum enchant_s : unsigned char {
	NoEnchant,
	OfAccuracy, OfAdvise, OfCharisma, OfClimbing, OfCold, OfDamage, OfDexterity,
	OfFire, OfFireResistance, OfHealing, OfHolyness,
	OfIntellegence, OfInvisibility, OfLuck, OfMagicResistance, OfNeutralizePoison,
	OfPoison, OfPoisonResistance, OfProtection, OfRegeneration,
	OfSharpness, OfSmashing, OfSpeed, OfStrenght,
	OfVampirism, OfWizardy,
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
	Arrow, Dart, Stone,
	Bones, DungeonMap,
	HolySymbol, MagicBook, TheifTools, MagicWand, MageScroll, PriestScroll,
	KeyShelf, KeySilver, KeyCooper, KeySkull, KeySpider, KeyMoon, KeyDiamond, KeyGreen,
	RedRing, BlueRing, GreenRing,
	PotionRed, PotionBlue, PotionGreen,
	RedGem, BlueGem, GreenGem, PurpleGem,
	Ration, RationIron,
	// Monster attacks
	Slam, Claws, Bite, Bite2d6,
	LastItem = Bite2d6,
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
	ResistBludgeon, ResistSlashing, ResistPierce,
	BonusVsElfWeapon, BonusToHitVsGoblinoid, BonusDamageVsEnemy, BonusACVsLargeEnemy, BonusHP,
};
enum usability_s : unsigned char {
	UseLeatherArmor, UseMetalArmor, UseShield,
	UseTheifWeapon, UseMartialWeapon, UseLargeWeapon,
	UseScrolls, UseDivine, UseArcane, UseTheif
};
enum item_feat_s : unsigned char {
	TwoHanded, Light, Versatile, Ranged, Deadly, Quick,
	Wonderful, Magical,
};
enum attack_s : unsigned char {
	AutoHit,
	OneAttack, OneAndTwoAttacks, TwoAttacks,
	OnHit, OnAllHit, OnCriticalHit,
};
class creature;
template<typename T> struct bsmeta {
	typedef T			data_type;
	static T			elements[];
};
struct abilityi {
	const char*			name;
	enchant_s			enchant;
	state_s				boost;
};
struct alignmenti {
	const char*			name;
	adat<class_s, 8>	restricted;
};
struct attacki {
	const char*			name;
	char				attacks_p2r;
};
struct classi {
	const char*			name;
	char				playable;
	char				hd;
	ability_s			ability;
	adat<class_s, 4>	classes;
	cflags<usability_s>	usability;
	cflags<feat_s>		feats;
	char				minimum[Charisma + 1];
	adat<race_s, 12>	races;
};
struct commandi {
	const char*			name;
};
struct directioni {
	const char*			name;
};
struct enchanti {
	const char*			name;
	state_s				effect;
	const char**		names;
};
struct genderi {
	const char*			name;
};
struct combati {
	attack_s			attack;
	damage_s			type;
	char				speed;
	dice				damage;
	char				bonus, critical_multiplier, critical_range;
};
struct itemi {
	struct weaponi : combati {
		dice			damage_large;
		constexpr weaponi() : damage_large(), combati() {}
		constexpr weaponi(attack_s attack, damage_s type, char speed, dice damage, dice damage_large, char bonus = 0) :
			combati{attack, type, speed, damage, bonus}, damage_large(damage_large) {}
	};
	struct armori {
		char			ac;
		char			critical_deflect;
		char			reduction;
	};
	const char*			name;
	unsigned char		portrait;
	wear_s				equipment;
	cflags<usability_s>	usability;
	cflags<item_feat_s>	feats;
	weaponi				weapon;
	armori				armor;
	aref<enchant_s>		enchantments;
};
struct feati {
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
	cflags<feat_s>		feats;
	char				hd[2];
	char				ac;
	item_s				attacks[4];
	state_s				special[3];
	enchant_s			enchantments[2];
	int					getexperience() const;
	bool				is(enchant_s id) const;
	bool				is(state_s id) const;
};
struct racei {
	const char*			name;
	char				minimum[Charisma + 1];
	char				maximum[Charisma + 1];
	char				adjustment[Charisma + 1];
	cflags<feat_s>		feats;
	cflags<usability_s>	usability;
	adatc<skill_s, char, DetectSecrets + 1> skills;
};
struct skilli {
	const char*			name;
	adat<class_s, 4>	allow;
};
struct spell_effect {
	dice				base;
	int					level;
	dice				perlevel;
	int					maximum_per_level;
};
struct effecti {
	typedef void(*callback)(creature* player, creature* target, const effecti& e, int level, int wand_magic);
	callback			proc;
	union {
		struct {
			state_s		state;
			duration_s	duration;
			save_s		save;
			char		save_bonus;
		};
		struct {
			damage_s	damage_type;
			dice		damage;
			dice		damage_per;
			char		damage_increment, damage_maximum;
			save_s		damage_save;
		};
		int				value;
	};
	static void			apply_effect(creature* player, creature* target, const effecti& e, int level, int wand_level);
	static void			apply_damage(creature* player, creature* target, const effecti& e, int level, int wand_level);
	constexpr effecti(callback proc, int value = 0) : proc(proc), value(value) {}
	constexpr effecti(duration_s duration, state_s state, save_s save = SaveNegate, char save_bonus = 0) : proc(apply_effect),
		duration(duration), state(state), save(save), save_bonus(save_bonus) {}
	constexpr effecti(damage_s type, dice damage, dice damage_per_level, char increment = 1, char maximum = 0, save_s save = SaveNegate) : proc(apply_damage),
		damage_type(type), damage(damage), damage_per(damage_per), damage_increment(increment), damage_maximum(maximum),
		damage_save(save) {}
};
struct spelli {
	const char*			name;
	int					levels[2]; // mage, cleric
	target_s			range;
	effecti				effect;
	item_s				throw_effect;
};
struct statei {
	const char*			name;
	skill_s				save;
};
class item {
	item_s				type;
	unsigned char		identified : 1;
	unsigned char		cursed : 1; // -2 to quality and not remove
	unsigned char		broken : 1; // -1 to quality and next breaking destroy item
	unsigned char		magic : 2; // 0, 1, 2, 3 this is plus item
	enchant_s			subtype; // spell scroll or spell of wand
	unsigned char		charges; // uses of item
public:
	constexpr item(item_s type = NoItem) : type(type), identified(0), cursed(0), broken(0), magic(0), subtype(NoEnchant), charges(0) {}
	constexpr item(item_s type, enchant_s enchant, int magic = 0) : type(type), identified(1), cursed(0), broken(0), magic(magic), subtype(enchant), charges(0) {}
	item(spell_s type);
	item(item_s type, int chance_magic, int chance_cursed, int chance_special);
	constexpr explicit operator bool() const { return type != NoItem; }
	constexpr bool operator==(const item i) const { return i.type == type && i.subtype == subtype && i.identified == identified && i.cursed == cursed && i.broken == broken && i.magic == magic && i.charges == charges; }
	void				clear();
	int					get(enchant_s value) const;
	void				get(combati& result, const creature* enemy) const;
	int					getac() const;
	int					getcharges() const { return charges; }
	int					getdeflect() const;
	enchant_s			getenchant() const;
	int					getmagic() const;
	char*				getname(char* result, const char* result_maximum) const;
	int					getportrait() const;
	int					getspeed() const;
	spell_s				getspell() const;
	item_s				gettype() const { return type; }
	wear_s				getwear() const { return bsmeta<itemi>::elements[type].equipment; }
	unsigned			getuse() const { return bsmeta<itemi>::elements[type].usability.data; }
	bool				is(usability_s v) const { return bsmeta<itemi>::elements[type].usability.is(v); }
	bool				is(item_feat_s v) const { return bsmeta<itemi>::elements[type].feats.is(v); }
	bool				isartifact() const { return magic == 3; }
	bool				isbroken() const { return broken != 0; }
	bool				iscursed() const { return cursed != 0; }
	bool				isidentified() const { return identified != 0; }
	bool				ismagical() const;
	bool				ismelee() const;
	bool				isranged() const { return is(Ranged); }
	bool				istwohanded() const { return is(TwoHanded); }
	void				setcharges(int value) { charges = value; }
	void				setbroken(int value) { broken = value; }
	void				setcursed(int value) { cursed = value; }
	void				setidentified(int value) { identified = value; }
	void				setspell(spell_s spell);
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
	unsigned			states[LastState + 1];
	cflags<feat_s>		feats;
	cflags<usability_s>	usability;
	short				hits, hits_rolled;
	char				initiative;
	bool				moved;
	char				levels[3];
	char				ability[Charisma + 1];
	item				wears[LastInvertory + 1];
	char				spells[LastSpellAbility + 1];
	char				prepared[LastSpellAbility + 1];
	char				known[LastSpellAbility + 1];
	char				avatar;
	unsigned			experience;
	unsigned char		name[2];
	char				str_exeptional;
	friend struct archive;
	int					armor_penalty(skill_s skill) const;
	int					get_base_save_throw(skill_s st) const;
	class_s				getbestclass() const { return getclass(getclass(), 0); }
	void				prepare_random_spells(class_s type, int level);
	char				racial_bonus(char* data) const;
	void				raise_level(class_s type);
	void				random_equipment();
	void				random_spells(class_s type, int level, int count);
	void				update_levelup(bool interactive);
	void				update_poison(bool interactive);
public:
	explicit operator bool() const { return states[0] != 0; }
	void				add(item i);
	bool				add(state_s type, unsigned duration = 0, save_s id = NoSave, char svae_bonus = 0);
	void				addexp(int value);
	static void			addexp(int value, int killing_hit_dice);
	void				attack(short unsigned index, direction_s d, int bonus);
	void				attack(creature* defender, wear_s slot, int bonus);
	bool				cast(spell_s id, class_s type, int wand_magic, creature* target = 0);
	void				create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive = false);
	void				clear();
	bool				canspeak(race_s language) const;
	spell_s				choosespell(class_s type) const;
	void				damage(damage_s type, int hits);
	int					damaged(const creature* defender, wear_s slot) const;
	void				equip(item it);
	void				finish();
	int					get(ability_s id) const;
	int					get(class_s id) const;
	static int			get(duration_s duration, int level);
	int					get(spell_s spell) const { return spells[spell]; }
	int					get(skill_s id) const;
	void				get(combati& e, wear_s slot = RightHand, creature* enemy = 0) const;
	item				get(wear_s id) const;
	alignment_s			getalignment() const { return alignment; }
	int					getac() const;
	int					getavatar() const { return avatar; }
	int					getawards() const;
	int					getbonus(enchant_s id) const;
	int					getbonus(enchant_s id, wear_s slot) const;
	class_s				getclass() const { return type; }
	static class_s		getclass(class_s id, int index);
	int					getclasscount() const;
	direction_s			getdirection() const;
	int					getexperience() const { return experience; }
	int					gethd() const;
	dice				gethitdice() const;
	int					gethitpenalty(int bonus) const;
	short				gethits() const { return hits; }
	short				gethitsmaximum() const;
	gender_s			getgender() const { return gender; }
	short unsigned		getindex() const;
	int					getinitiative() const { return initiative; }
	item*				getitem(wear_s id) { return &wears[id - FirstInvertory]; }
	int					getknown(spell_s id) const { return known[id]; }
	static int			getlevel(spell_s id, class_s type);
	const char*			getname(char* result, const char* result_maximum) const;
	int					getprepare(spell_s v) const { return prepared[v]; }
	race_s				getrace() const { return race; }
	resource_s			getres() const;
	int					getside() const;
	size_s				getsize() const;
	int					getspecialist(item_s weapon) const;
	int					getspeed() const;
	int					getspellsperlevel(class_s type, int spell_level) const;
	int					getstrex() const;
	int					getthac0(class_s cls, int level) const;
	bool				have_class(aref<class_s> source) const;
	bool				is(state_s id) const;
	bool				is(state_s id, wear_s slot) const;
	bool				is(feat_s v) const { return feats.is(v); }
	bool				is(usability_s v) const { return usability.is(v); }
	bool				is(spell_s v) const { return known[v] != 0; }
	static bool			isallow(class_s id, race_s r);
	static bool			isallow(alignment_s id, class_s c);
	bool				isallow(const item it, wear_s slot) const;
	bool				isallowremove(const item i, wear_s slot, bool interactive);
	bool				isenemy(creature* target) const;
	bool				isinvisible() const;
	bool				ishero() const;
	bool				ismoved() const { return moved; }
	bool				isready() const;
	bool				isuse(const item v) const;
	static creature*	newhero();
	void				preparespells();
	void				random_name();
	int					render_ability(int x, int y, int width, bool use_bold) const;
	int					render_combat(int x, int y, int width, bool use_bold) const;
	bool				roll(skill_s id, int bonus = 0);
	void				roll_ability();
	void				say(spell_s id) const;
	void				say(const char* format, ...);
	void				sayv(const char* format, const char* vl);
	void				scribe(item& it);
	static void			scriblescrolls();
	static unsigned		select(spell_s* result, spell_s* result_maximum, class_s type, int level);
	void				set(ability_s id, int v) { ability[id] = v; }
	void				set(alignment_s value) { alignment = value; }
	void				set(class_s value) { type = value; }
	void				set(gender_s value) { gender = value; }
	void				set(monster_s type);
	void				set(race_s value) { race = value; }
	bool				set(skill_s skill, short unsigned index);
	void				set(spell_s spell, char v) { spells[spell] = v; }
	bool				set(state_s id, unsigned rounds);
	void				set(direction_s value);
	void				setavatar(int value) { avatar = value; }
	void				setframe(short* frames, short index) const;
	void				sethits(short v) { hits = v; };
	void				sethitsroll(short v) { hits_rolled = v; }
	void				setindex(short unsigned value) { index = value; }
	void				setinitiative(char value) { initiative = value; }
	void				setknown(spell_s id, char v) { known[id] = v; }
	void				setmoved(bool value) { moved = value; }
	void				setprepare(spell_s id, char v) { prepared[id] = v; }
	void				setside(int value);
	static bool			swap(item* itm1, item* itm2);
	void				update(bool interactive);
	bool				use(skill_s skill, short unsigned index, int bonus, bool* firsttime, int exp, bool interactive);
	void				view_ability();
	static void			view_party();
	void				view_portrait(int x, int y) const;
};
struct dungeon {
	struct overlayi {
		cell_s			type; // type of overlay
		direction_s		dir; // puller direction
		short unsigned	subtype; // depends on value type
		short unsigned	index; // puller index
		bool			active;
	};
	struct groundi {
		item			value;
		short unsigned	index;
		unsigned char	side;
		unsigned char	flags;
	};
	struct overlayitem : item {
		overlayi*		storage;
	};
	struct statei {
		item_s			keys[2]; // two type of keys that fit locks
		monster_s		habbits[2]; // who dwelve here
		overlayi		up; // where is stairs up
		overlayi		down; // where is stairs down
		overlayi		portal; // where is portal
		unsigned char	messages; // count of messages
		unsigned char	secrets; // count of secret rooms
		unsigned char	artifacts; // count of powerful items (+4 or hight)
		unsigned char	rings; // count of magical rings
		unsigned char	weapons; // count of magical weapons
		unsigned char	elements; // count of corridors
		unsigned char	traps; // count of traps
	};
	dungeon_s			type;
	unsigned short		overland_index;
	unsigned char		level;
	bool				haspits;
	statei				stat;
	unsigned char		data[mpx*mpy];
	groundi				items[1024];
	overlayi			overlays[512];
	overlayitem			cellar_items[512];
	creature			monsters[200];
	//
	operator bool() const { return overland_index != 0; }
	void				add(overlayi* p, item it);
	creature*			addmonster(monster_s type, short unsigned index, char side, direction_s dir);
	void				addmonster(monster_s type, short unsigned index, direction_s dir = Up);
	bool				allaround(short unsigned index, cell_s t1 = CellWall, cell_s t2 = CellUnknown);
	void				clear();
	void				dropitem(short unsigned index, item rec, int side);
	void				fill(short unsigned index, int sx, int sy, cell_s value);
	void				finish(cell_s t);
	void				generate(dungeon_s type, unsigned short index, unsigned char level, unsigned short start = 0, bool interactive = false);
	cell_s				get(short unsigned index) const;
	cell_s				get(int x, int y) const;
	short unsigned		gettarget(short unsigned start, direction_s dir);
	void				getblocked(short unsigned* pathmap, bool treat_door_as_passable);
	int					getfreeside(short unsigned index);
	unsigned			getitems(item** result, item** result_maximum, short unsigned index, int side = -1);
	unsigned			getitems(item** result, item** result_maximum, overlayi* povr);
	int					getitemside(item* pi);
	short unsigned		getindex(int x, int y) const;
	race_s				getlanguage() const;
	void				getmonsters(creature** result, short unsigned index, direction_s dr);
	item_s				getkeytype(cell_s keyhole) const;
	short unsigned		getnearest(short unsigned index, int radius, cell_s t1);
	short unsigned*		getnearestfree(short unsigned* indicies, short unsigned index);
	direction_s			getpassable(short unsigned index, direction_s* dirs);
	short unsigned		getsecret() const;
	overlayi*			getoverlay(short unsigned index, direction_s dir);
	cell_s				gettype(cell_s id);
	cell_s				gettype(overlayi* po);
	bool				is(short unsigned index, cell_flag_s value) const;
	bool				isactive(overlayi* po);
	bool				isblocked(short unsigned index);
	bool				ismatch(short unsigned index, cell_s t1, cell_s t2);
	bool				ismonster(short unsigned index);
	void				link();
	void				makewave(short unsigned start, short unsigned* pathmap);
	short unsigned		random(short unsigned* indicies);
	bool				read(unsigned short overland_index, unsigned char level);
	void				remove(unsigned short index, cell_flag_s value);
	void				remove(overlayi* po, item it);
	void				remove(overlayi* po);
	void				set(short unsigned index, cell_s value);
	void				set(short unsigned index, cell_flag_s value);
	void				setactive(overlayi* po, bool active);
	void				setactive(short unsigned index, bool value);
	void				setactive(short unsigned index, bool value, int radius);
	void				setcontent(dungeon_s type, int level);
	void				setelement(short unsigned index, direction_s dir, cell_s type);
	overlayi*			setoverlay(short unsigned index, cell_s type, direction_s dir);
	void				traplaunch(short unsigned index, direction_s dir, item_s show, effecti& e);
	void				turnto(short unsigned index, direction_s dr);
	void				write();
};
struct site {
	struct overlayi {
		item_s			keys[2]; // two type of keys that fit locks
		monster_s		habbits[2]; // who dwelve here
		char			magic; // base chance for magic items
		char			curse; // base chance for cursed items
	};
};
namespace game {
namespace action {
void					attack(short unsigned index);
void					automap(dungeon& area, bool fow);
void					camp(item& food);
creature*				choosehero();
void					dropitem(item* itm, int side = -1);
void					fly(item_s item, int side);
void					getitem(item* itm, int side = -1);
bool					manipulate(item* itm, direction_s direction);
void					move(direction_s direction);
void					pause();
void					preparespells(class_s type);
bool					question(item* current_item);
void					scriblescrolls();
void					thrown(item* itm);
void					rotate(direction_s direction);
bool					use(item* itm);
}
void					endround();
void					enter(unsigned short index, unsigned char level);
void					findsecrets();
int						getavatar(race_s race, gender_s gender, class_s cls);
int						getavatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls);
int						getarmorpenalty(item_s armor, skill_s skill);
short unsigned			getcamera();
creature*				getdefender(short unsigned index, direction_s dr, creature* attacker);
direction_s				getdirection();
int						getfreeside(creature* sides[4]);
wear_s					getitempart(item* itm);
creature*				gethero(item* itm);
void					getheroes(creature** result, direction_s dir);
int						getrandom(int type, race_s race, gender_s gender, int prev_name);
int						getpartyskill(int rec, skill_s id);
size_s					getsize(item_s rec);
int						getside(int side, direction_s dr);
void					hearnoises();
extern creature*		party[7];
void					passround();
void					passtime(int minutes);
bool					read();
extern unsigned			rounds;
void					setcamera(short unsigned index, direction_s direction = Center);
void					write();
}
namespace draw {
struct menu {
	void(*proc)();
	const char*			text;
	operator bool() const { return proc != 0; }
};
struct enumelement {
	int					id;
	const char*			text;
};
namespace animation {
void					appear(dungeon& location, short unsigned index, int radius = 1);
void					attack(creature* attacker, wear_s slot, int hits);
void					clear();
void					damage(creature* target, int hits);
void					render(int pause = 300, bool show_screen = true, item* current_item = 0);
int						thrown(short unsigned index, direction_s dr, item_s rec, direction_s sdr = Center, int wait = 100);
int						thrownstep(short unsigned index, direction_s dr, item_s itype, direction_s sdr = Center, int wait = 100);
void					update();
}
void					adventure();
int						choose(aref<enumelement> elements, const char* title_string);
void					chooseopt(const menu* source);
bool					dlgask(const char* text);
void					mainmenu();
void					options();
void					setnext(void(*p)());
bool					settiles(dungeon_s id);
}
extern dungeon			location_above;
extern dungeon			location;
direction_s				devectorized(direction_s dr, direction_s d);
inline int				gx(short unsigned index) { return index % mpx; }
inline int				gy(short unsigned index) { return index / mpx; }
short unsigned			moveto(short unsigned index, direction_s d);
void					mslog(const char* format, ...);
void					mslogv(const char* format, const char* vl);
direction_s				pointto(short unsigned from, short unsigned to);
direction_s				rotateto(direction_s d, direction_s d1);
direction_s				vectorized(direction_s d, direction_s d1);