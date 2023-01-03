/*

  This file controls data for stores

  - Macro'd store structs (the following macros were not made global due to variable values such as player.biome)
*/

// Bunny's Endeavor
#define CREATE_BUNNY_STORE \
  struct Store *bunny_store = (struct Store[]) \
  { \
    { \
      .item = &items[ITEM_ACORNS], \
      .quantity = player.level *200, \
      .cost = 200 \
    }, \
    { \
      .item = &items[ITEM_MATERIALS], \
      .quantity = ((player.level/STAT_EVOLUTION) +1) * 5, \
      .cost = 400 \
    }, \
    { \
      .item = &items[ITEM_GOLDEN_ACORN], \
      .quantity = ((player.level/STAT_EVOLUTION) +1) * 50, \
      .cost = 600 \
    }, \
    { \
      .item = &biomes[player.biome].biome_material, \
      .quantity = 25 - (5 *(player.biome +1)), \
      .cost = 800 \
    }, \
    { \
      .item = &items[ITEM_ENERGY], \
      .quantity = ((player.level/STAT_EVOLUTION) +1) * 15, \
      .cost = 1000 \
    } \
  }

// Seedy Market
#define CREATE_SEEDY_STORE \
  struct Store *seedy_store = (struct Store[]) \
  { \
    { \
      .item = &biomes[GRASSLANDS].biome_material, \
      .quantity = 10, \
      .cost = 250 \
    }, \
    { \
      .item = &biomes[SEEPING_SANDS].biome_material, \
      .quantity = 10, \
      .cost = 500 \
    } \
  }

// daily rewards requires player struct to be filled!
#define CREATE_DAILY_REWARDS \
  struct Store *daily_rewards = (struct Store[]) \
  { \
    { \
      .item = &items[ITEM_ACORNS], \
      .quantity = player.level * 250, \
    }, \
    { \
      .item = &items[ITEM_MATERIALS], \
      .quantity = (player.level/STAT_EVOLUTION +1) * 5, \
    }, \
    { \
      .item = &items[ITEM_GOLDEN_ACORN], \
      .quantity = (player.level/STAT_EVOLUTION +1) * 50, \
    }, \
    { \
      .item = &biomes[rand() % (player.max_biome +1)].biome_material, \
      .quantity = player.level/STAT_EVOLUTION +1, \
    }, \
    { \
      .item = &items[ITEM_ENERGY], \
      .quantity = (player.level/STAT_EVOLUTION +1) * 10, \
    } \
  }
