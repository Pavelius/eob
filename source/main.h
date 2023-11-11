#include "ability.h"
#include "alignment.h"
#include "city.h"
#include "color.h"
#include "dataset.h"
#include "duration.h"
#include "event.h"
#include "feat.h"
#include "flagable.h"
#include "gender.h"
#include "goal.h"
#include "item.h"
#include "point.h"
#include "rect.h"
#include "shape.h"
#include "spell.h"
#include "target.h"
#include "stringbuilder.h"
#include "variable.h"
#include "wear.h"

const unsigned short Blocked = 0xFFFF;
const int walls_frames = 9;
const int walls_count = 6;
const int door_offset = 1 + walls_frames * walls_count;
const int decor_offset = door_offset + 9;
const int decor_count = 21;
const int decor_frames = 10;
const int scrx = 22 * 8;
const int scry = 15 * 8;
const int mpx = 38;
const int mpy = 23;

enum fcell : unsigned {
	Passable, EmpthyStartIndex,
	LookWall, LookOverlay, LookObject,
	PassableActivated
};
enum resource_s : unsigned char {
	NONE,
	BORDER, OUTTAKE, DECORS,
	CHARGEN, CHARGENB, COMPASS, INVENT, ITEMS, ITEMGS, ITEMGL,
	BLUE, BRICK, CRIMSON, DROW, DUNG, GREEN, FOREST, MEZZ, SILVER, XANATHA,
	MENU, PLAYFLD, INTRO, PORTM, THROWN, XSPL,
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
enum message_s : unsigned char {
	MessageMagicWeapons, MessageMagicRings, MessageSecrets, MessageTraps,
	MessageAtifacts, MessageSpecialItem, MessageBoss,
	MessageHabbits
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
enum save_s : unsigned char {
	NoSave, SaveHalf, SaveNegate,
};
enum flag_s : unsigned char {
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
enum reaction_s : unsigned char {
	Indifferent, Friendly, Hostile,
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
enum pack_s : unsigned char {
	PackDungeon, PackMonster, PackOuttake,
	PackInterface, PackBackground, PackCenter,
	Pack160x96, Pack320x120, Pack320x200
};
enum ambush_s : unsigned char {
	NoAmbush, MonsterAmbush, PartyAmbush
};
enum condition_s : unsigned char {
	Healed, BadlyWounded, Wounded,
};
enum actionf_s : unsigned char {
	CheckCondition, MayLoseReputation, DependOnReputation,
};
typedef cflags<action_s> actionf;
typedef cflags<good_s> goodf;
typedef cflags<feat_s> feata;
typedef flagable<1 + LastSpellAbility / 8> spellf;
typedef flagable<16> adventuref;
typedef flagable<4> flagf;
typedef short unsigned indext;
typedef adatc<ability_s, char, DetectSecrets + 1> skilla;
class creature;
class creaturea;
class item;
struct settlementi;
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
	fnevent				proc;
	fntestcase			test;
	int					key;
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
struct alignmenti {
	const char*			name;
	morale_s			law;
	morale_s			morale;
	cflags<class_s>		restricted;
	int					reaction;
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
struct namei {
	race_s				race;
	gender_s			gender;
	const char*			name;
};
struct usabilityi {
	const char*			name;
};
struct itemfeati {
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
	int					getexperience() const;
	char				getpallette() const;
	bool				is(variant id) const;
	bool				is(feat_s id) const;
};
struct racei {
	char				characters;
	const char*			name;
	abilitya			minimum, maximum, adjustment;
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
	const char*			name;
	pack_s				pack;
	void*				data;
	const packi&		gete() const { return bsdata<packi>::elements[pack]; }
	const char*			geturl() const;
	bool				isdungeon() const;
	bool				ismonster() const;
	static resourcei*	find(const char* id, unsigned size);
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
struct imagei {
	resource_s			res;
	unsigned short		frame;
	constexpr explicit operator bool() const { return res != NONE; }
	void				add(stringbuilder& sb) const;
	void				clear() { memset(this, 0, sizeof(*this)); }
	const resourcei&	gete() const { return bsdata<resourcei>::elements[res]; }
	void				set(resource_s r, unsigned short f) { res = r; frame = f; }
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
		cell_s			corner;
		int				corner_count;
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
struct boosti {
	variant				owner;
	spell_s				id;
	unsigned			round;
	constexpr explicit operator bool() const { return owner.type != NoVariant; }
	void				clear();
};
class itema : public adat<item*, 48> {
	typedef bool (item::*pitem)() const;
	void				select(pitem proc, bool keep);
public:
	void				broken(bool keep) { select(&item::isbroken, keep); }
	item*				choose(const char* format, bool cancel_button, fntext panel);
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
	int					gethitpenalty(int bonus) const;
	int					getstrex() const;
	static int			getthac0(class_s type, int level);
	constexpr bool		is(spell_s v) const { return active_spells.is(v); }
	constexpr bool		is(feat_s v) const { return feats.is(v); }
	constexpr bool		is(usability_s v) const { return usability.is(v); }
	void				random_ability(race_s race, gender_s gender, class_s type);
	bool				raiseability(race_s race, ability_s v, bool run);
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
	void				addboost(spell_s id, unsigned duration);
	void				attack_drain(creature* defender, char& value, int& hits);
	void				campcast(item& it);
	class_s				getbestclass() const { return getclass(getclass(), 0); }
	void				prepare_random_spells(class_s type, int level);
	char				racial_bonus(char* data) const;
	void				raise_level(class_s type);
	void				random_spells(class_s type, int level, int count);
	void				update_poison(bool interactive);
	void				update_wears();
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
	bool				enchant(spell_s id, int level, bool run);
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
	void				healing();
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
	void				select(indext index, bool only_ready = true);
	void				set(reaction_s v);
};
struct dungeoni {
	struct overlayi : cflags<flag_s> {
		cell_s			type; // type of overlay
		direction_s		dir; // overlay direction
		indext			index; // index
		indext			index_link; // linked to this location
		short unsigned	subtype; // depends on value type
		constexpr explicit operator bool() const { return index != Blocked; }
		void			clear();
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
		short unsigned	messages; // count of messages
		short unsigned	secrets; // count of secret rooms
		short unsigned	artifacts; // count of artifact items
		short unsigned	rings; // count of magical rings
		short unsigned	weapons; // count of magical weapons
		short unsigned	elements; // count of corridors
		short unsigned	traps; // count of traps
		short unsigned	bones; // count of bones
		short unsigned	gems; // count of gems
		short unsigned	relicts; // count of books and holy symbols
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
	int					getneightboard(indext index, cell_s t1, cell_s t2);
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
struct idable {
	const char*			id;
};
struct historyi {
	static constexpr unsigned history_max = 12;
	const char*			history[history_max];
	unsigned char		history_progress;
	unsigned			gethistorymax() const;
};
struct adventurei : idable, historyi {
	const char*			name;
	const char*			summary;
	const char*			agree;
	const char*			entering;
	const char*			finish;
	sitei				levels[8];
	unsigned char		stage; // 0 - non active, 1 - active, 2 - accepted, 0xFF - finished
	char				complete_goals[GrabAllSpecialItems + 1], goals[GrabAllSpecialItems + 1];
	sitei*				addsite() { for(auto& e : levels) if(!e) return &e; return 0; }
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				create(bool interactive) const;
	void				enter();
	int					getindex() const { return this - bsdata<adventurei>::elements; }
	const char*			getname() const { return name; }
	bool				iscomplete() const;
	void				read(const char* url);
};
extern adventurei* last_adventure;
struct chati {
	talk_s				action;
	conditiona			conditions;
	const char*			text;
	constexpr operator bool() const { return text[0] != 0; }
	const char*			find(const aref<variant>& variants) const;
	int					getconditions() const;
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
class gamei : public cityi {
	typedef void(creature::*fnparty)();
	indext				camera_index;
	direction_s			camera_direction;
	unsigned char		location_level;
	unsigned short		adventure_index;
	unsigned			rounds;
	unsigned			rounds_turn;
	unsigned			rounds_hour;
	unsigned			rounds_daypart;
	unsigned			killed[LastMonster + 1];
	unsigned			found_secrets, gold_donated;
	variant				players[6];
public:
	void				add(city_ability_s v, int n) { cityi::add(v, n); }
	void				add(cityabilitya& e) { cityi::add(e); }
	void				add(monster_s id) { killed[id]++; }
	void				addexp(morale_s id, unsigned v);
	void				addexpc(unsigned v, int killing_hit_dice);
	void				addspell(spell_s v, unsigned duration);
	void				apply(variant v);
	void				attack(indext index, bool ranged, ambush_s ambush);
	void				camp(item& it);
	void				camp(item_s food, bool cursed, int additional_bonus = 0);
	void				clear();
	void				clearfiles();
	void				endround();
	void				enter(unsigned short index, char level, bool set_camera = true);
	bool				enchant(spell_s id, int level, bool run);
	void				equiping();
	void				findsecrets();
	int					get(action_s v) const;
	int					get(city_ability_s v) const { return cityabilitya::get(v); }
	adventurei*			getadventure();
	int					getaverage(ability_s v) const;
	static int			getavatar(race_s race, gender_s gender, class_s cls);
	static int			getavatar(unsigned short* result, race_s race, gender_s gender, class_s cls);
	indext				getcamera() const { return camera_index; }
	creature*			getdefender(short unsigned index, direction_s dr, creature* attacker);
	static resource_s	getenviroment();
	int					gethour() const { return (rounds % (24 * 60)) / 60; }
	void				getheroes(creature** result, direction_s dir);
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
	bool				iseffect(spell_s v) const;
	bool				manipulate(item* itm, direction_s direction);
	static void			newgame();
	void				passround();
	void				passtime(int minutes);
	void				preserial(bool writemode);
	bool				question(item* current_item);
	bool				read();
	static bool			roll(int value);
	void				say(const char* format, ...);
	void				setcamera(indext index, direction_s direction = Center);
	void				thrown(item* itm);
	void				write();
};
class answers {
	char				buffer[512];
	stringbuilder		sc;
	struct element {
		int				id;
		const char*		text;
	};
public:
	answers();
	adat<element, 32>	elements;
	void				add(int id, const char* name, ...) { addv(id, name, xva_start(name)); }
	void				addv(int id, const char* name, const char* format);
	void				clear() { elements.clear(); sc.clear(); }
	int					choose(const char* title) const;
	int					choose(const char* title, bool interactive) const;
	int					choose(const char* format, const char* header, int* current_level, creature** current_creature, const creaturea* allowed, fnint getnumber) const;
	int					choosehz(const char* title) const;
	int					choosebg(const char* title) const;
	int					choosesm(const char* title, bool allow_cancel = true) const;
	int					choosems(const char* title, bool allow_cancel = true) const;
	int					choosemb(const char* title, bool allow_cancel = true) const;
	int					choosemn(int x, int y, int width, resource_s id) const;
	static int			compare(const void* v1, const void* v2);
	static bool			confirm(const char* title, const char* positive = "Accept", const char* negative = "Decline");
	static void			message(const char* format);
	int					random() const;
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
void					greenbarx(rect rc, int vc, int vm);
int						header(int x, int y, const char* text);
void					initialize();
bool					isallowmodal();
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
void					closeform();
const actioni*			dlgall(const char* format, const aref<actioni>& source);
bool					dlgask(const char* text);
void					dlgmsg(const char* text);
void					dlgmsgsm(const char* text);
void					mainmenu();
void					openform();
void					options(const char* header, aref<actioni> actions);
void					options();
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
extern imagei			last_image;
extern aref<actioni>	last_menu;
extern const char*		last_name;
extern const char*		last_menu_header;
inline int				gx(indext index) { return index % mpx; }
inline int				gy(indext index) { return index / mpx; }
indext					to(indext index, direction_s d);
void					mslog(const char* format, ...);
void					mslogv(const char* format, const char* vl);
direction_s				pointto(indext from, indext to);
int						rangeto(indext i1, indext i2);
direction_s				to(direction_s d, direction_s d1);
inline int				d100() { return rand() % 100; }
// Script functions
void add_small_miracle();
void enter_city();
void enter_inn();
void enter_temple();
void game_options();
void memorize_spells();
void pray_for_spells();
void play_adventure();
void play_city();
void return_to_city();
void scrible_scrolls();
// Function get comon name
NOBSDATA(abilitya)
NOBSDATA(dice)
NOBSDATA(imagei)
NOBSDATA(item)
NOBSDATA(itemi::weaponi)
NOBSDATA(itemi::armori)
NOBSDATA(historyi)
NOBSDATA(point)
NOBSDATA(sitei)
NOBSDATA(variant)
BSLNK(cell_s, celli)
BSLNK(city_ability_s, cityabilityi)
BSLNK(item_s, itemi)
BSLNK(goal_s, goali)
BSLNK(monster_s, monsteri)
BSLNK(race_s, racei)
BSLNK(resource_s, resourcei)