/*

  This header stores struct declarations

*/

//Base file object
struct File {
  char* formal_name;
  char* file_path;

  char* description;

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
  struct File biome_material;

  int* material_ptr;
  int* stat_ptr;
  float stat_value_multiplier; // multiplier for rewards and stat value
  float stat_price_multiplier; // multiplier for price on upgrades

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
  int smell_lv;       // Acorns
  int dexterity_lv;   // Pine Cones
  int acuity_lv;      // Seeds
  int luck_lv;        // Biome Material
  int proficiency_lv; // XP
};

struct Materials {
  int seeds;        // used for alchemy
  int pine_cones;   // used for stat upgrades

  int mooshrums;
  int cactus_flowers;
  int juniper_berries;
  int blueberries;
  int dark_chestnuts;
};

struct Player {
  unsigned long user_id;
  int level;
  float xp;
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

  struct Materials materials;
  struct Stats stats;
  struct Buffs buffs;
};

/* Independent Structs */
struct Rewards {
  int xp;
  int acorns;
  int seeds;
  int pine_cones;

  int biome_material;

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