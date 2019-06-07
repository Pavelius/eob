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
	BORDER, SCENES,
	CHARGEN, CHARGENB, COMPASS, INVENT, ITEMS, ITEMGS, ITEMGL,
	AZURE, BLUE, BRICK, CRIMSON, DROW, DUNG, GREEN, FOREST, MEZZ, SILVER, XANATHA,
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
	Duration1PerLevel,
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
	ReadLanguagesSpell, ShieldSpell, ShokingGrasp, Sleep,
	// Spells (level 2)
	Aid, FlameBlade, Goodberry, HoldPerson, ProduceFlame, SlowPoison,
	// Spells (level 3)
	CreateFood, CureBlindnessDeafness, CureDisease, NegativePlanProtection,
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
	Orc, Shadow, Skeleton, SkeletonWarrior, Spider, Wight, Wolf, Zombie
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
enum condition_s : unsigned char {
	Blinded, Deafned, Diseased,
	Moved, ProtectedNegativeEnergy,
	Surprised,
};
enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
enum skill_s : unsigned char {
	SaveVsParalization, SaveVsPoison, SaveVsTraps, SaveVsMagic,
	ClimbWalls, HearNoise, MoveSilently, OpenLocks, RemoveTraps, ReadLanguages,
	LearnSpell,
	ResistCharm, ResistCold, ResistFire, ResistMagic,
	CriticalDeflect,
	DetectSecrets,
	FirstSave = SaveVsParalization, LastSave = SaveVsMagic,
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
	OfEnergyDrain,
	OfFear, OfFire, OfFireResistance, OfHealing, OfHolyness,
	OfIntellegence, OfInvisibility, OfKnowledge, OfLuck, OfMagicResistance, OfNeutralizePoison,
	OfParalize, OfPoison, OfPoisonStrong, OfPoisonResistance, OfProtection, OfRegeneration,
	OfSharpness, OfSmashing, OfSpeed, OfStrenght, OfStrenghtDrain,
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
	ResistBludgeon, ResistSlashing, ResistPierce, ImmuneNormalWeapon,
	BonusVsElfWeapon, BonusToHitVsGoblinoid, BonusDamageVsEnemy, BonusACVsLargeEnemy, BonusHP,
};
enum usability_s : unsigned char {
	UseLeatherArmor, UseMetalArmor, UseShield,
	UseTheifWeapon, UseMartialWeapon, UseLargeWeapon,
	UseScrolls, UseDivine, UseArcane, UseTheif
};
enum item_feat_s : unsigned char {
	TwoHanded, Light, Versatile, Ranged, Deadly, Quick,
	SevereDamageUndead,
	Natural, Charged,
	Unique, Wonderful, Magical,
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
	StartCombat, WinCombat, GainExperience,
	AddVariant, HaveVariant, RemoveVariant,
};
enum variant_s : unsigned char {
	NoVariant,
	Alignment, Class, Item, Race, Spell,
};
enum prop_s : unsigned char {
	Attack, Damage, CriticalRange, CriticalMultiply, Deflection,
	MV, AC, Speed, Hits, HitsBase, HitsBoost, ExeptionalStrenght,
	DrainEnergy, DrainStrenght, DrainConstitution,
};
class creature;
class item;
template<typename T> struct bsmeta {
	typedef T			data_type;
	static T			elements[];
};
struct varianti {
	variant_s			type;
	union {
		alignment_s		alignment;
		class_s			cls;
		item_s			item;
		race_s			race;
		spell_s			spell;
		unsigned char	value;
	};
	constexpr varianti() : type(NoVariant), value(0) {}
	constexpr varianti(const alignment_s v) : type(Alignment), alignment(v) {}
	constexpr varianti(const class_s v) : type(Class), cls(v) {}
	constexpr varianti(const item_s v) : type(Item), item(v) {}
	constexpr varianti(const race_s v) : type(Race), race(v) {}
	constexpr varianti(const spell_s v) : type(Spell), spell(v) {}
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
struct durationi {
	const char*			name;
	unsigned			multiplier, divider, addiction;
	int					get(int v) const;
};
struct enchanti {
	const char*			name;
	state_s				effect;
	const char**		names;
};
struct genderi {
	const char*			name;
};
struct intellegencei {
	const char*			name;
	char				v1, v2;
};
struct combati {
	attack_s			attack;
	damage_s			type;
	char				speed;
	dice				damage;
	char				bonus, critical_multiplier, critical_range;
	item*				weapon;
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
	intellegence_s		ins;
	cflags<feat_s>		feats;
	char				hd[2];
	char				ac;
	item_s				attacks[4];
	enchant_s			enchantments[2];
	adatc<skill_s, char, DetectSecrets + 1> skills;
	//
	int					getexperience() const;
	char				getpallette() const;
	bool				is(enchant_s id) const;
	bool				is(feat_s id) const;
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
	enchant_s			enchant;
	char				multiplier;
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
		struct {
			item_s		item_weapon;
		};
		int				value;
	};
	static void			apply_effect(creature* player, creature* target, const effecti& e, int level, int wand_level);
	static void			apply_damage(creature* player, creature* target, const effecti& e, int level, int wand_level);
	static void			apply_weapon(creature* player, creature* target, const effecti& e, int level, int wand_level);
	constexpr effecti(callback proc, int value = 0) : proc(proc), value(value) {}
	constexpr effecti(duration_s duration, state_s state, save_s save = SaveNegate, char save_bonus = 0) : proc(apply_effect),
		duration(duration), state(state), save(save), save_bonus(save_bonus) {}
	constexpr effecti(item_s item_weapon) : proc(apply_weapon),
		item_weapon(item_weapon) {}
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
struct sitei {
	struct headi {
		resource_s		type;
		monster_s		habbits[2]; // Who dwelve here
		item_s			keys[2]; // Two keys open all doors
		item_s			special[2]; // Two special items find on this level
		race_s			language; // All messages in this language
	};
	struct chancei {
		char			magic;
		char			curse;
		char			special;
	};
	headi				head;
	char				levels;
	chancei				chance;
	constexpr explicit operator bool() const { return head.type != NONE; }
	unsigned			getleveltotal() const;
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
	void				damage(const char* text_damage, const char* text_brokes);
	int					get(enchant_s value) const;
	void				get(combati& result, const creature* enemy) const;
	int					getac() const;
	int					getarmorpenalty(skill_s skill) const;
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
	bool				ischarged() const { return is(Charged); }
	bool				iscursed() const { return cursed != 0; }
	bool				isidentified() const { return identified != 0; }
	bool				ismagical() const;
	bool				ismelee() const;
	bool				isnatural() const { return is(Natural); }
	bool				isranged() const { return is(Ranged); }
	bool				istwohanded() const { return is(TwoHanded); }
	void				setcharges(int value);
	void				setbroken(int value) { broken = value; }
	void				setcursed(int value) { cursed = value; }
	void				setidentified(int value) { identified = value; }
	void				setspell(spell_s spell);
};
class creature {
	struct applyi {
		prop_s			id;
		char			bonus;
		unsigned		round;
	};
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
	cflags<condition_s> condition;
	short				hits, hits_aid, hits_rolled;
	char				initiative;
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
	char				drain_energy;
	char				drain_ability[Charisma + 1];
	char				pallette;
	short				food;
	reaction_s			reaction;
	//
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
	void				addaid(int v) { hits_aid += v; }
	void				addexp(int value);
	static void			addexp(int value, int killing_hit_dice);
	void				attack(short unsigned index, direction_s d, int bonus, bool ranged);
	void				attack(creature* defender, wear_s slot, int bonus);
	static void			camp(item& it);
	bool				cast(spell_s id, class_s type, int wand_magic, creature* target = 0);
	void				create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive = false);
	void				clear();
	bool				canspeak(race_s language) const;
	spell_s				choosespell(class_s type) const;
	void				damage(damage_s type, int hits, int magic_bonus = 0);
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
	int					getknown(spell_s id) const { return known[id]; }
	static int			getlevel(spell_s id, class_s type);
	const char*			getname(char* result, const char* result_maximum) const;
	int					getpallette() const { return pallette; }
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
	bool				have(aref<class_s> source) const;
	bool				have(item_s v) const;
	bool				identify(bool interactive);
	bool				is(condition_s v) const { return condition.is(v); }
	bool				is(state_s v) const;
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
	bool				ismoved() const { return is(Moved); }
	bool				isready() const;
	bool				isuse(const item v) const;
	static creature*	newhero();
	void				preparespells();
	bool				raise(enchant_s v);
	void				random_name();
	void				remove(condition_s v) { condition.remove(v); }
	int					render_ability(int x, int y, int width, bool use_bold) const;
	int					render_combat(int x, int y, int width, bool use_bold) const;
	bool				roll(ability_s id, int bonus = 0) const;
	bool				roll(skill_s id, int bonus = 0) const;
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
	void				set(condition_s value) { condition.add(value); }
	void				set(gender_s value) { gender = value; }
	void				set(monster_s type);
	void				set(race_s value) { race = value; }
	bool				set(skill_s skill, short unsigned index);
	void				set(spell_s spell, char v) { spells[spell] = v; }
	bool				set(state_s id, unsigned rounds);
	void				set(direction_s value);
	void				set(const item it, wear_s v) { wears[v] = it; }
	void				setavatar(int value) { avatar = value; }
	void				setframe(short* frames, short index) const;
	void				sethits(short v) { hits = v; };
	void				sethitsroll(short v) { hits_rolled = v; }
	void				setindex(short unsigned value) { index = value; }
	void				setinitiative(char value) { initiative = value; }
	void				setknown(spell_s id, char v) { known[id] = v; }
	void				setmoved(bool value) { remove(Moved); }
	void				setprepare(spell_s id, char v) { prepared[id] = v; }
	void				setside(int value);
	bool				setweapon(item_s v, int charges);
	void				slowpoison();
	void				subenergy();
	static bool			swap(item* itm1, item* itm2);
	void				update(bool interactive);
	bool				use(skill_s skill, short unsigned index, int bonus, bool* firsttime, int exp, bool interactive);
	static bool			use(item* pi);
	void				view_ability();
	static void			view_party();
	void				view_portrait(int x, int y) const;
};
struct dungeon {
	struct overlayi {
		cell_s			type; // type of overlay
		direction_s		dir; // overlay direction
		short unsigned	index; // index
		short unsigned	index_link; // linked to this location
		short unsigned	subtype; // depends on value type
		short unsigned	flags;
		constexpr explicit operator bool() const { return type != CellUnknown; }
		void			clear();
		bool			is(overlay_flag_s v) const { return (flags&(1<<v)) != 0; }
		void			remove(overlay_flag_s v) { flags &= ~(1 << v); }
		void			set(overlay_flag_s v) { flags |= 1 << v; }
	};
	struct groundi {
		item			value;
		short unsigned	index;
		unsigned char	side;
		unsigned char	flags;
		constexpr explicit operator bool() const { return value.operator bool(); }
	};
	struct overlayitem : item {
		overlayi*		storage;
	};
	struct statei {
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
		unsigned char	special; // count of special items generated
		short unsigned	items; // total count of items
		short unsigned	overlays; // total count of overlays
		short unsigned	monsters; // total count of monsters
	};
	unsigned short		overland_index;
	unsigned char		level;
	sitei::headi		head;
	statei				stat;
	sitei::chancei		chance;
	unsigned char		data[mpx*mpy];
	groundi				items[512];
	overlayi			overlays[256];
	overlayitem			cellar_items[256];
	creature			monsters[200];
	dungeon() { clear(); }
	operator bool() const { return head.type!=NONE; }
	overlayi*			add(short unsigned index, cell_s type, direction_s dir);
	void				add(overlayi* p, item it);
	creature*			addmonster(monster_s type, short unsigned index, char side, direction_s dir);
	void				addmonster(monster_s type, short unsigned index, direction_s dir = Up);
	bool				allaround(short unsigned index, cell_s t1 = CellWall, cell_s t2 = CellUnknown);
	void				attack(const combati& ci, creature* enemy) const;
	void				clear();
	static void			create(short unsigned index, const sitei* site, bool interactive = false);
	void				dropitem(short unsigned index, item rec, int side);
	void				dropitem(item* pi, int side = -1);
	void				fill(short unsigned index, int sx, int sy, cell_s value);
	void				finish(cell_s t);
	cell_s				get(short unsigned index) const;
	cell_s				get(int x, int y) const;
	short unsigned		gettarget(short unsigned start, direction_s dir);
	void				getblocked(short unsigned* pathmap, bool treat_door_as_passable);
	int					getfreeside(creature** sides);
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
	static size_s		getsize(creature** sides);
	overlayi*			getoverlay(short unsigned index, direction_s dir);
	cell_s				gettype(cell_s id);
	cell_s				gettype(overlayi* po);
	bool				is(short unsigned index, cell_flag_s value) const;
	bool				isactive(const overlayi* po);
	bool				isblocked(short unsigned index) const;
	bool				isblocked(short unsigned index, int side) const;
	bool				ismatch(short unsigned index, cell_s t1, cell_s t2);
	bool				ismonster(short unsigned index);
	static bool			isvisible(short unsigned index);
	dungeon::overlayi*	getlinked(short unsigned index);
	void				makewave(short unsigned start, short unsigned* pathmap);
	void				move(short unsigned index, direction_s dr);
	void				move(direction_s direction);
	void				passround();
	void				pickitem(item* itm, int side = -1);
	short unsigned		random(short unsigned* indicies);
	bool				read(unsigned short overland_index, unsigned char level);
	void				remove(unsigned short index, cell_flag_s value);
	void				remove(overlayi* po, item it);
	void				remove(overlayi* po);
	void				rotate(direction_s direction);
	void				set(short unsigned index, cell_s value);
	void				set(short unsigned index, cell_flag_s value);
	void				set(short unsigned index, direction_s dir, cell_s type);
	void				setactive(overlayi* po, bool active);
	void				setactive(short unsigned index, bool value);
	void				setactive(short unsigned index, bool value, int radius);
	void				stop(short unsigned index);
	void				traplaunch(short unsigned index, direction_s dir, item_s show, const combati& e);
	void				turnto(short unsigned index, direction_s dr);
	void				write();
};
struct dialogi {
	struct imagei {
		resource_s		res;
		short			id;
		unsigned		flags;
		constexpr explicit operator bool() const { return res != 0; }
	};
	struct elementi {
		const char*		text;
		const char*		success;
		const char*		fail;
		action_s		action;
		varianti		variant;
		constexpr explicit operator bool() const { return text != 0; }
		bool			isallow() const;
	};
	constexpr explicit operator bool() const { return id != 0; }
	imagei				overlay[4];
	const char*			id;
	const char*			text;
	elementi			variants[3];
	void				choose(bool border = false) const;
	const dialogi*		find(const char* id) const;
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
	int					choosebg(const char* title, bool border = false, const dialogi::imagei* pi = 0) const;
	int					choosesm(const char* title, bool allow_cancel = true) const;
	int					random() const;
	void				sort();
private:
	char				buffer[512];
	stringcreator		sc;
};
namespace game {
namespace action {
void					attack(short unsigned index, bool ranged);
void					automap(dungeon& area, bool fow);
creature*				choosehero();
void					fly(item_s item, int side);
bool					manipulate(item* itm, direction_s direction);
void					pause();
void					preparespells(class_s type);
bool					question(item* current_item);
void					thrown(item* itm);
}
void					endround();
void					enter(unsigned short index, unsigned char level);
void					findsecrets();
int						getavatar(race_s race, gender_s gender, class_s cls);
int						getavatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls);
short unsigned			getcamera();
creature*				getdefender(short unsigned index, direction_s dr, creature* attacker);
direction_s				getdirection();
wear_s					getitempart(item* itm);
creature*				gethero(item* itm);
void					getheroes(creature** result, direction_s dir);
int						getrandom(int type, race_s race, gender_s gender, int prev_name);
int						getpartyskill(int rec, skill_s id);
int						getside(int side, direction_s dr);
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
namespace animation {
void					appear(dungeon& location, short unsigned index, int radius = 1);
void					attack(creature* attacker, wear_s slot, int hits);
void					clear();
void					damage(creature* target, int hits);
void					render(int pause = 300, bool show_screen = true, item* current_item = 0);
int						thrown(short unsigned index, direction_s dr, item_s rec, direction_s sdr = Center, int wait = 100, bool block_monsters = false);
int						thrownstep(short unsigned index, direction_s dr, item_s itype, direction_s sdr = Center, int wait = 100);
void					update();
}
void					adventure();
void					chooseopt(const menu* source);
bool					dlgask(const char* text);
void					mainmenu();
void					options();
void					setnext(void(*p)());
bool					settiles(resource_s id);
}
extern dungeon			location_above;
extern dungeon			location;
direction_s				devectorized(direction_s dr, direction_s d);
inline int				gx(short unsigned index) { return index % mpx; }
inline int				gy(short unsigned index) { return index / mpx; }
short unsigned			to(short unsigned index, direction_s d);
void					mslog(const char* format, ...);
void					mslogv(const char* format, const char* vl);
direction_s				pointto(short unsigned from, short unsigned to);
int						rangeto(short unsigned i1, short unsigned i2);
direction_s				to(direction_s d, direction_s d1);
direction_s				vectorized(direction_s d, direction_s d1);