/*

  This header stores struct declarations

*/

//Base file object
struct File {
  char* formal_name;
  char* file_path;

  char* description;
  int* stat_ptr; // if a file is associated with a certain stat

  float value_mult; // multiplier for rewards and stat value
  float price_mult; // multiplier for price on upgrades

  char* emoji_name;
  long unsigned emoji_id;
};

//Encounter Stats
struct Encounter {
  char* name;
  char* file_path;
  char* conflict;
  char* solution[3];
};

struct Biome {
  struct File biome_icon;
  char* biome_scene_path;

  struct Encounter *encounters;
  struct File *encounter_files;

  int encounter_size;
};

struct Scurry {
  unsigned long scurry_owner_id;
  char* scurry_name;

  int courage; // most recent score
  int war_acorns;

  int war_flag;
  int rank; // not in db

  int war_acorn_cap; // not in db
};

struct Buffs {
  int smell_acorn;
  int endurance_acorn; //endurance is for energy
  int acuity_acorn;
  int luck_acorn;
  int proficiency_acorn;
};

struct Stats {
  int proficiency_lv; // Acorns and XP
  int strength_lv;    // HP
  int luck_lv;        // Golden Acorns
};

struct Player {
  unsigned long user_id;
  int level;
  int xp;
  int acorns; // main currency

  int active_squirrel;
  int biome;
  int max_biome;
  int select_encounter;

  int color;

  time_t main_cd;
  time_t daily_cd;
  int energy;

  unsigned long scurry_id;
  int stolen_acorns; // keeps track of player participation in a scurry war

  int golden_acorns; //event currency
  int acorn_count;
  int catnip;

  struct Stats stats;
  struct Buffs buffs;
};

/* Independent Structs */
struct Rewards {
  int xp;
  int acorns;

  int courage;
  int stolen_acorns;

  int golden_acorns;
  int catnip;
};

struct Store {
  struct File *item;
  int cost;
  int quantity;
};

struct Message {
  struct discord_embed *embed;
  struct discord_components *buttons;

  struct discord_channel *channel;
  char* content;
};