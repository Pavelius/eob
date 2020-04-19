#include "dice.h"
#include "color.h"
#include "crt.h"
#include "rect.h"
#include "stringbuilder.h"

#define assert_enum(e, last) static_assert(sizeof(bsdata<e##i>::elements) / sizeof(bsdata<e##i>::elements[0]) == last + 1, "Invalid count of " #e " elements");

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
	TargetSelf, TargetItem,
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
	Moved,
	// Spells (level 1)
	Bless, BurningHands, CureLightWounds, DetectEvil, DetectMagic, Fear, FeatherFall,
	Identify, MageArmor, MagicMissile, Mending,
	ProtectionFromEvil, PurifyFood,
	ReadLanguagesSpell, ResistColdSpell, ShieldSpell, ShokingGrasp, Sleep,
	// Spells (level 2)
	AcidArrow, Aid, Blindness, Blur, Deafness, FlameBlade, FlamingSphere, Goodberry, HoldPerson,
	Invisibility, Knock, ProduceFlame, SlowPoison,
	// Spells (level 3)
	CreateFood, CureBlindnessDeafness, Disease, CureDisease, Haste, NegativePlanProtection,
	RemoveCurse, RemoveParalizes,
	// Spells (level 4)
	Poison,
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
	LastAbility = Charisma
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
	Necklage, NecklageRich, Jewelry,
	Arrow, Dart, Stone,
	Bones, DungeonMap,
	HolySymbol, HolyWarriorSymbol, MagicBook, TheifTools, MagicWand, MageScroll, PriestScroll,
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
	Ability, Alignment, Class, Creature, Item, Number, Race, Reaction, Spell,
};
typedef short unsigned indext;
typedef flagable<LastSpellAbility> spella;
typedef adatc<skill_s, char, DetectSecrets + 1> skilla;
typedef cflags<usability_s> usabilitya;
class creature;
class item;
struct variant {
	variant_s			type;
	unsigned char		value;
	constexpr variant() : type(NoVariant), value(0) {}
	constexpr variant(const alignment_s v) : type(Alignment), value(v) {}
	constexpr variant(const class_s v) : type(Class), value(v) {}
	constexpr variant(const item_s v) : type(Item), value(v) {}
	constexpr variant(const race_s v) : type(Race), value(v) {}
	constexpr variant(const reaction_s v) : type(Reaction), value(v) {}
	constexpr variant(const spell_s v) : type(Spell), value(v) {}
	constexpr variant(const unsigned char v) : type(Number), value(v) {}
	variant(const creature* v);
	constexpr explicit operator bool() const { return type == NoVariant; }
	constexpr bool operator==(const variant& e) const { return type == e.type && value == e.value; }
	void				clear() { type = NoVariant; value = 0; }
	creature*			getcreature() const;
};
struct spellprogi {
	char				elements[21][10];
};
struct abilityi {
	const char*			name;
	enchant_s			enchant;
	char				base;
	char				multiplier;
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
	usabilitya			usability;
	cflags<feat_s>		feats;
	char				minimum[Charisma + 1];
	adat<race_s, 12>	races;
	const spellprogi*	spells[2];
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
	usabilitya			usability;
	cflags<item_feat_s>	feats;
	weaponi				weapon;
	armori				armor;
	aref<enchant_s>		enchantments;
	aref<spell_s>		spells;
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
	skilla				skills;
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
	usabilitya			usability;
	skilla				skills;
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
	variant				type;
	duration_s			duration;
	save_s				save;
	char				save_bonus;
	dice				damage;
	dice				damage_per;
	char				damage_increment, damage_maximum;
	int					value;
	static void			apply_effect(creature* player, creature* target, const effecti& e, int level, int wand_level);
	static void			apply_damage(creature* player, creature* target, const effecti& e, int level, int wand_level);
	static void			apply_weapon(creature* player, creature* target, const effecti& e, int level, int wand_level);
	constexpr effecti(callback proc,
		variant type, duration_s duration, save_s save, char save_bonus,
		dice damage, dice damage_per, char damage_increment, char damage_maximum, int value) : proc(proc),
		type(type), duration(duration), save(save), save_bonus(save_bonus),
		damage(damage), damage_per(damage_per), damage_increment(damage_increment), damage_maximum(damage_maximum),
		value(value) {
	}
	constexpr effecti(callback proc) : effecti(proc, {}, Instant, NoSave, 0, {}, {}, 0, 0, 0) {}
	constexpr effecti(variant type, duration_s duration = Instant, save_s save = NoSave, char save_bonus = 0) : effecti(apply_effect, type, duration, save, save_bonus, {}, {}, 0, 0, 0) {}
	constexpr effecti(item_s item_weapon) : effecti(apply_weapon, item_weapon, Instant, NoSave, 0, {}, {}, 0, 0, 0) {}
	constexpr effecti(damage_s type, dice damage, dice damage_per_level, char increment = 1, char maximum = 0, save_s save = SaveNegate) :
		effecti(apply_damage, type, Instant, save, 0, damage, damage_per_level, increment, maximum, 0) {
	}
};
struct spelli {
	const char*			name;
	int					levels[2]; // mage, cleric
	target_s			range;
	effecti				effect;
	item_s				throw_effect;
};
struct sitei {
	struct headi {
		resource_s		type;
		monster_s		habbits[2]; // Who dwelve here
		item_s			keys[2]; // Two keys open all doors
		item_s			special[2]; // Two special items find on this level
		race_s			language; // All messages in this language
	};
	struct monsteri {
		monster_s		type;
		unsigned char	count;
	};
	struct crypti {
		monster_s		boss;
		explicit operator bool() const { return boss != NoMonster; }
	};
	struct chancei {
		char			magic;
		char			curse;
		char			special;
	};
	headi				head;
	char				levels;
	chancei				chance;
	crypti				crypt;
	constexpr explicit operator bool() const { return head.type != NONE; }
	unsigned			getleveltotal() const;
};
class item {
	item_s				type;
	unsigned char		identified : 1;
	unsigned char		cursed : 1; // -1 to quality and not remove
	unsigned char		broken : 1; // sometime -1 to quality and next breaking destroy item
	unsigned char		magic : 2; // 0, 1, 2, 3 this is plus item
	enchant_s			subtype; // spell scroll or spell of wand
	unsigned char		charges; // uses of item
public:
	constexpr item(item_s type = NoItem) : type(type), identified(0), cursed(0), broken(0), magic(0), subtype(NoEnchant), charges(0) {}
	constexpr item(item_s type, enchant_s enchant, int magic = 0) : type(type), identified(1), cursed(0), broken(0), magic(magic), subtype(enchant), charges(0) {}
	constexpr item(item_s type, spell_s spell, int magic = 0) : type(type), identified(1), cursed(0), broken(0), magic(magic), subtype((enchant_s)spell), charges(0) {}
	item(spell_s type);
	item(item_s type, int chance_magic, int chance_cursed, int chance_special);
	constexpr explicit operator bool() const { return type != NoItem; }
	constexpr bool operator==(const item i) const { return i.type == type && i.subtype == subtype && i.identified == identified && i.cursed == cursed && i.broken == broken && i.magic == magic && i.charges == charges; }
	void				clear();
	bool				damage(const char* text_damage, const char* text_brokes);
	int					get(enchant_s value) const;
	void				get(combati& result, const creature* enemy) const;
	int					getac() const;
	int					getarmorpenalty(skill_s skill) const;
	int					getcharges() const { return charges; }
	int					getdeflect() const;
	enchant_s			getenchant() const;
	int					getmagic() const;
	void				getname(stringbuilder& sb) const;
	int					getportrait() const;
	int					getspeed() const;
	variant				getspecial() const { return variant(); }
	spell_s				getspell() const;
	item_s				gettype() const { return type; }
	wear_s				getwear() const { return bsdata<itemi>::elements[type].equipment; }
	bool				is(usability_s v) const { return bsdata<itemi>::elements[type].usability.is(v); }
	bool				is(item_feat_s v) const { return bsdata<itemi>::elements[type].feats.is(v); }
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
struct boosti {
	variant				owner, id;
	unsigned			round;
	constexpr explicit operator bool() const { return id.type != NoVariant; }
	void				clear();
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
	cflags<feat_s>		feats;
	usabilitya			usability;
	short				hits, hits_aid, hits_rolled;
	char				initiative;
	char				levels[3];
	char				ability[LastAbility + 1];
	item				wears[LastInvertory + 1];
	char				spells[LastSpellAbility + 1];
	char				prepared[LastSpellAbility + 1];
	spella				known_spells;
	spella				active_spells;
	char				avatar;
	unsigned			experience;
	unsigned char		name[2];
	char				str_exeptional;
	char				drain_energy, drain_strenght, disease_progress;
	char				pallette;
	short				food;
	reaction_s			reaction;
	//
	void				addboost(variant id, unsigned duration) const;
	void				attack_drain(creature* defender, char& value, int& hits);
	void				dress_wears(int m);
	void				dressoff();
	void				dresson();
	void				drink(spell_s effect, int magic);
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
	explicit operator bool() const { return race != NoRace; }
	typedef void		(creature::*apply_proc)(bool);
	void				activate(spell_s v) { active_spells.set(v); }
	void				add(item i);
	bool				add(spell_s type, unsigned duration = 0, save_s id = NoSave, char svae_bonus = 0);
	void				addaid(int v) { hits_aid += v; }
	void				addexp(int value);
	static void			addexp(int value, int killing_hit_dice);
	static void			addparty(item i);
	static void			apply(apply_proc proc, bool interactive = true);
	void				apply(spell_s id, int magic, unsigned duration);
	void				attack(short unsigned index, direction_s d, int bonus, bool ranged);
	void				attack(creature* defender, wear_s slot, int bonus);
	static void			camp(item& it);
	bool				cast(spell_s id, class_s type, int wand_magic, creature* target = 0);
	void				create(gender_s gender, race_s race, class_s type, alignment_s alignment, bool interactive = false);
	void				clear();
	static void			clearboost();
	bool				canspeak(race_s language) const;
	static creature*	choosehero();
	spell_s				choosespell(class_s type) const;
	void				damage(damage_s type, int hits, int magic_bonus = 0);
	int					damaged(const creature* defender, wear_s slot) const;
	void				encounter(reaction_s id);
	void				equip(item it);
	item*				find(item_s v) const;
	void				finish();
	int					get(ability_s id) const;
	int					get(class_s id) const;
	int					get(spell_s spell) const { return spells[spell]; }
	int					get(skill_s id) const;
	void				get(combati& e, wear_s slot = RightHand, creature* enemy = 0) const;
	item				get(wear_s id) const;
	const spellprogi*	getprogress(class_s v) const;
	alignment_s			getalignment() const { return alignment; }
	int					getac() const;
	int					getavatar() const { return avatar; }
	int					getawards() const;
	int					getbonus(enchant_s id) const;
	int					getbonus(enchant_s id, wear_s slot) const;
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
	static int			getparty(skill_s id);
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
	bool				identify(bool interactive);
	void				interract();
	bool				is(spell_s v) const { return active_spells.is(v); }
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
	bool				ismoved() const { return is(Moved); }
	bool				isready() const;
	bool				isuse(const item v) const;
	bool				mending(bool interactive);
	void				poison(save_s save, char save_bonus = 0);
	void				preparespells();
	static void			preparespells(class_s type);
	bool				raise(enchant_s v);
	void				random_name();
	void				remove(spell_s v);
	void				removeboost(variant v) const;
	int					render_ability(int x, int y, int width, bool use_bold) const;
	int					render_combat(int x, int y, int width, bool use_bold) const;
	bool				roll(ability_s id, int bonus = 0) const;
	bool				roll(skill_s id, int bonus = 0) const;
	void				roll_ability();
	reaction_s			rollreaction(int bonus) const;
	void				say(spell_s id) const;
	void				say(const char* format, ...);
	void				sayv(const char* format, const char* vl);
	void				scribe(item& it);
	static void			scriblescrolls();
	static unsigned		select(spell_s* result, const spell_s* result_maximum, class_s type, int level);
	void				set(ability_s id, int v) { ability[id] = v; }
	void				set(alignment_s value) { alignment = value; }
	void				set(class_s value) { type = value; }
	void				set(gender_s value) { gender = value; }
	void				set(monster_s type);
	void				set(race_s value) { race = value; }
	void				set(reaction_s v) { reaction = v; }
	bool				set(skill_s skill, short unsigned index);
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
	void				uncurse();
	void				update(const boosti& e);
	void				update(bool interactive);
	void				update_turn(bool interactive);
	void				update_hour(bool interactive);
	static void			update_boost();
	bool				use(skill_s id, short unsigned index, int bonus, bool* firsttime, int exp, bool interactive);
	static bool			use(item* pi);
	bool				usequick();
	void				view_ability();
	static void			view_party();
	void				view_portrait(int x, int y) const;
};
class creaturea : public adat<creature*, 12> {
public:
	void				rollinitiative();
	void				select(short unsigned index);
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
	struct groundi {
		item			value;
		short unsigned	index;
		unsigned char	side;
		unsigned char	flags;
		constexpr explicit operator bool() const { return value.operator bool(); }
	};
	struct overlayitem : item {
		short unsigned	storage_index;
	};
	struct statei {
		overlayi		up; // where is stairs up
		overlayi		down; // where is stairs down
		overlayi		portal; // where is portal
		overlayi		crypt; // where is crypt located
		overlayi		crypt_button; // where is crypt located
		short unsigned	spawn[2]; // new monster appera here
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
		skill_s			skill;
		short unsigned	index;
		constexpr operator bool() const { return owner.operator bool(); }
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
	eventi				events[256];
	dungeon() { clear(); }
	operator bool() const { return head.type != NONE; }
	overlayi*			add(short unsigned index, cell_s type, direction_s dir);
	void				add(overlayi* p, item it);
	creature*			addmonster(monster_s type, short unsigned index, char side, direction_s dir);
	int					addmonster(monster_s type, short unsigned index, direction_s dir = Up);
	bool				allaround(short unsigned index, cell_s t1 = CellWall, cell_s t2 = CellUnknown);
	void				attack(const combati& ci, creature* enemy) const;
	void				automap(bool fow);
	void				clear();
	bool				create(rect& rc, int w, int h) const;
	static void			create(short unsigned index, const sitei* site, bool interactive = false);
	void				dropitem(short unsigned index, item rec, int side);
	void				dropitem(item* pi, int side = -1);
	void				fill(short unsigned index, int sx, int sy, cell_s value);
	void				finish(cell_s t);
	void				formation(short unsigned index, direction_s dr);
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
	unsigned			getmonstercount() const;
	void				getmonsters(creature** result, short unsigned index, direction_s dr);
	item_s				getkeytype(cell_s keyhole) const;
	short unsigned		getnearest(short unsigned index, int radius, cell_s t1);
	short unsigned*		getnearestfree(short unsigned* indicies, short unsigned index);
	direction_s			getpassable(short unsigned index, direction_s* dirs);
	short unsigned		getsecret() const;
	static size_s		getsize(creature** sides);
	overlayi*			getoverlay(short unsigned index, direction_s dir);
	overlayi*			getoverlay(const overlayitem& e) { return &overlays[e.storage_index]; }
	cell_s				gettype(cell_s id);
	cell_s				gettype(overlayi* po);
	bool				is(short unsigned index, cell_flag_s value) const;
	bool				is(short unsigned index, int width, int height, cell_s v) const;
	bool				is(const rect& rc, cell_s id) const;
	bool				isactive(const overlayi* po);
	bool				isblocked(short unsigned index) const;
	bool				isblocked(short unsigned index, int side) const;
	bool				islineh(short unsigned index, direction_s dir, int count, cell_s t1, cell_s t2 = CellUnknown) const;
	bool				ismatch(short unsigned index, cell_s t1, cell_s t2);
	bool				ismonster(short unsigned index);
	bool				isroom(short unsigned index, direction_s dir, int side, int height) const;
	static bool			isvisible(indext index);
	dungeon::overlayi*	getlinked(indext index);
	void				makedoor(const rect& rc, overlayi& door, direction_s dir, bool has_button, bool has_button_on_other_side);
	void				makeroom(const rect& rc, overlayi& door);
	void				makewave(short unsigned start, short unsigned* pathmap);
	void				move(indext index, direction_s dr);
	void				move(direction_s direction);
	void				passhour();
	void				passround();
	void				pickitem(item* itm, int side = -1);
	short unsigned		random(short unsigned* indicies);
	bool				read(indext overland_index, indext level);
	void				remove(indext index, cell_flag_s value);
	void				remove(overlayi* po, item it);
	void				remove(overlayi* po);
	void				makedoor(const rect& rc);
	void				rotate(direction_s direction);
	void				set(short unsigned index, cell_s value);
	void				set(short unsigned index, cell_flag_s value);
	void				set(short unsigned index, direction_s dir, cell_s type);
	void				set(short unsigned index, reaction_s v);
	void				setactive(overlayi* po, bool active);
	void				setactive(short unsigned index, bool value);
	void				setactive(short unsigned index, bool value, int radius);
	void				stop(short unsigned index);
	void				traplaunch(short unsigned index, direction_s dir, item_s show, const combati& e);
	void				turnto(short unsigned index, direction_s dr);
	void				write();
};
class string : public stringbuilder {
	creature*			source_hero;
	item*				source_item;
public:
	constexpr string(char* pb, const char* pe) : stringbuilder(pb, pe), source_hero(0), source_item(0) {}
	template<unsigned N> constexpr string(char(&result)[N]) : string(result, result + N - 1) {}
	void				set(creature* v) { source_hero = v; }
	void				set(item* v) { source_item = v; }
	virtual void		addidentifier(const char* identifier) override;
};
struct dialogi {
	struct actioni {
		action_s		action;
		variant			variant;
		void			apply();
		bool			isallow() const;
		constexpr explicit operator bool() const { return action != NoAction; }
	};
	struct imagei {
		resource_s		res;
		short			id;
		unsigned		flags;
		constexpr explicit operator bool() const { return res != 0; }
	};
	struct elementi {
		const char*		text;
		const char*		next[2];
		actioni			actions[4];
		constexpr explicit operator bool() const { return text != 0; }
		bool			isallow() const;
		void			apply();
	};
	constexpr explicit operator bool() const { return id != 0; }
	imagei				overlay[4];
	const char*			id;
	const char*			text;
	elementi			variants[5];
	void				choose(bool border = false) const;
	const dialogi*		find(const char* id) const;
};
class gamei {
	indext				camera_index;
	direction_s			camera_direction;
	indext				location_index;
	indext				location_level;
	unsigned			rounds;
	unsigned			rounds_turn;
	unsigned			rounds_hour;
	unsigned			killed[LastMonster + 1];
	unsigned			found_secrets;
public:
	void				add(monster_s id) { killed[id]++; }
	void				attack(indext index, bool ranged);
	void				endround();
	void				enter(indext index, indext level);
	void				findsecrets();
	int					getavatar(race_s race, gender_s gender, class_s cls);
	int					getavatar(int* result, const int* result_maximum, race_s race, gender_s gender, class_s cls);
	indext				getcamera() const { return camera_index; }
	creature*			getcreature(const item* itm) const;
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
	bool				manipulate(item* itm, direction_s direction);
	void				passround();
	void				passtime(int minutes);
	void				pause();
	bool				question(item* current_item);
	void				setcamera(short unsigned index, direction_s direction = Center);
	void				thrown(item* itm);
	bool				read();
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
	int					choosebg(const char* title, bool border = false, const dialogi::imagei* pi = 0, bool herizontal_buttons = true) const;
	int					choosesm(const char* title, bool allow_cancel = true) const;
	int					random() const;
	void				sort();
private:
	char				buffer[512];
	stringbuilder		sc;
};
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
extern gamei			game;
extern dungeon			location_above;
extern dungeon			location;
extern variant			party[6];
inline int				gx(short unsigned index) { return index % mpx; }
inline int				gy(short unsigned index) { return index / mpx; }
short unsigned			to(short unsigned index, direction_s d);
void					mslog(const char* format, ...);
void					mslogv(const char* format, const char* vl);
direction_s				pointto(short unsigned from, short unsigned to);
int						rangeto(short unsigned i1, short unsigned i2);
direction_s				to(direction_s d, direction_s d1);
inline int				d100() { return rand() % 100; }
DECLENUM(ability)
DECLENUM(alignment)
DECLENUM(class)
DECLENUM(enchant)
DECLENUM(monster)
DECLENUM(race)
DECLENUM(skill)
DECLENUM(spell)
DECLENUM(gender)