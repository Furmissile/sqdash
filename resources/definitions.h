/*

  This header defines enums and struct instances

*/

/* Init Player for db */
struct Player player = { 0 };
struct Scurry scurry = { 0 };

struct Rewards rewards = { 0 };

enum DB_TUPLE {
  DB_USER_ID = 0,
  DB_LEVEL,
  DB_XP,
  DB_ACORNS,
  DB_ACTIVE_SQUIRREL,
  DB_BIOME,
  DB_SELECT_ENCOUNTER,
  DB_COLOR,
  DB_MAIN_CD,
  DB_ENERGY,
  DB_GOLDEN_ACORNS,
  DB_SCURRY_ID,
  DB_STOLEN_ACORNS,
  DB_ACORN_COUNT,
  DB_CATNIP,
  DB_DAILY_CD,

  DB_SEEDS = 17,
  DB_PINE_CONES,
  DB_MOOSHRUMS,
  DB_CACTUS_FLOWERS,
  DB_JUNIPER_BERRIES,
  DB_BLUEBERRIES,
  DB_DARK_CHESTNUTS,

  DB_SMELL_LV = 25,
  DB_DEXTERITY_LV,
  DB_ACUITY_LV,
  DB_LUCK_LV,
  DB_PROFICIENCY_LV,

  DB_SMELL_ACORN = 31,
  DB_ENDURANCE_ACORN,
  DB_ACUITY_ACORN,
  DB_LUCK_ACORN,
  DB_PROFICIENCY_ACORN
};

enum SCURRY_RES {
  DB_SCURRY_OWNER_ID,
  DB_SCURRY_NAME,
  DB_COURAGE,
  DB_WAR_ACORNS,
  DB_WAR_FLAG
};

enum SCURRY_RANKS {
  SEED_NOT,
  ACORN_SNATCHER,
  SEED_SNIFFER,
  OAKFFICIAL,
  ROYAL_NUT
};

// UPDATE BIOME SIZE
enum BIOME {
  GRASSLANDS,
  SEEPING_SANDS,
  NATURE_END,
  DEATH_GRIP,
  NECROPOLIS,
  BIOME_SIZE = 2 // is last available biome (temporary)
};

#define LAST_BIOME (BIOME_SIZE -1)

/* Items */
enum ITEMS {
  ITEM_ACORNS,
  ITEM_CATNIP,
  ITEM_COURAGE,
  ITEM_ENERGY,
  ITEM_GOLDEN_ACORN,
  ITEM_ACORN_COUNT,
  ITEM_XP,
  ITEM_STAHR,
  ITEM_STOLEN_ACORNS,
  ITEM_SIZE
};

struct File *items = (struct File[]) 
{
  {
    .formal_name = "Acorns",
    .file_path = "Items/acorn.png",

    .emoji_name = "acorns",
    .emoji_id = 1045028765392187402,

    .stat_ptr = &player.acorns
  },
  {
    .formal_name = "Catnip",
    .file_path = "Items/catnip.png",

    .emoji_name = "catnip",
    .emoji_id = 1052250368039452732,

    .stat_ptr = &player.catnip
  },
  {
    .formal_name = "Courage",
    .file_path = "Scurry%20Utils/courage.png",

    .emoji_name = "courage",
    .emoji_id = 1045555306832347147,

    .stat_ptr = &scurry.courage
  },
  {
    .formal_name = "Energy",
    .file_path = "Items/energy.png",

    .emoji_name = "energy",
    .emoji_id = 911642184397258752,

    .stat_ptr = &player.energy
  },
  {
    .formal_name = "Golden Acorn",
    .file_path = "Items/golden_acorn.png",

    .emoji_name = "golden_acorn",
    .emoji_id = 1045032005420728430,

    .stat_ptr = &player.golden_acorns
  },
  {
    .formal_name = "Acorn Count",
    .emoji_name = "acorn_count",

    .file_path = "Items/acorn_count.png",
    .emoji_id = 1050407923823677502,

    .stat_ptr = &player.acorn_count
  },
  {
    .formal_name = "XP",
    .file_path = "Items/xp.png",

    .emoji_name = "xp",
    .emoji_id = 1001511753294815424,

    .stat_ptr = &player.xp
  },
  {
    .formal_name = "Level",
    .file_path = "Item%20Types/stahr.png",

    .emoji_name = "stahr",
    .emoji_id = 1045705606134251601,

    .stat_ptr = &player.level
  },
  {
    .formal_name = "Stolen Acorns",
    .file_path = "Items/stolen_acorn.png",

    .emoji_name = "stolen_acorns",
    .emoji_id = 1055143210839720067,

    .stat_ptr = &player.stolen_acorns
  }

};


/* Item-Types */
enum ITEM_TYPE {
  TYPE_ACORN_HANDFUL,
  TYPE_ACORN_MOUTHFUL,
  TYPE_ACORN_SACK,
  TYPE_DIRT_PILE,
  TYPE_LOST_STASH,
  TYPE_NO_ACORNS,
  TYPE_ENCOUNTER,
  TYPE_SIZE
};

struct File *item_types = (struct File[]) 
{
  {
    .formal_name = "a Handful of Acorns",
    .file_path = "Item%20Types/acorn_handful.png",

    .emoji_name = "acorn_handful",
    .emoji_id = 1044620522236944384
  },
  {
    .formal_name = "a Mouthful of Acorns",
    .file_path = "Item%20Types/acorn_mouthful.png",

    .emoji_name = "acorn_mouthful",
    .emoji_id = 1044620523067416627
  },
  {
    .formal_name = "a Sack of Acorns",
    .file_path = "Item%20Types/acorn_sack.png",

    .emoji_name = "acorn_sack",
    .emoji_id = 1044620524275380286
  },
  {
    .formal_name = "Dirt Pile",
    .file_path = "Item%20Types/dirt_pile.png",

    .emoji_name = "dirt_pile",
    .emoji_id = 1044620525365895168
  },
  {
    .formal_name = "a Lost Stash",
    .file_path = "Item%20Types/lost_stash.png",

    .emoji_name = "lost_stash",
    .emoji_id = 1044620525944705146
  },
  {
    .formal_name = "an Empty Hole",
    .file_path = "Item%20Types/no_acorns.png",

    .emoji_name = "no_acorns",
    .emoji_id = 1044620527223975957
  },
  {
    .formal_name = "an Encounter",
    .file_path = "Item%20Types/quest_marker.png",

    .emoji_name = "quest_marker",
    .emoji_id = 1046493067500335104
  }

};


/* Buffs */
enum BUFFS {
  ACUITY_ACORN,
  ENDURANCE_ACORN,
  LUCK_ACORN,
  PROFICIENCY_ACORN,
  SMELL_ACORN,
  BUFF_SIZE
};

struct File *enchanted_acorns = (struct File[])
{
  {
    .formal_name = "Acorn of Acuity",

    .file_path = "Enchanted%20Acorns/acuity_acorn.png",
    .description = "*+2 material earnings*",

    .emoji_name = "acuity_acorn",
    .emoji_id = 1045027180549255208,

    .stat_ptr = &player.buffs.acuity_acorn
  },
  {
    .formal_name = "Acorn of Endurance",

    .file_path = "Enchanted%20Acorns/endurance_acorn.png",
    .description = "*Refills some of energy*",

    .emoji_name = "edurance_acorn",
    .emoji_id = 1045027181996286022,

    .stat_ptr = &player.buffs.endurance_acorn
  },
  {
    .formal_name = "Acorn of Luck",

    .file_path = "Enchanted%20Acorns/luck_acorn.png",
    .description = "*+2 biome material earnings*",

    .emoji_name = "luck_acorn",
    .emoji_id = 1045027183334264872,

    .stat_ptr = &player.buffs.luck_acorn
  },
  {
    .formal_name = "Acorn of Proficiency",

    .file_path = "Enchanted%20Acorns/proficiency_acorn.png",
    .description = "*100% increased XP earnings*",

    .emoji_name = "proficiency_acorn",
    .emoji_id = 1045027184475123813,

    .stat_ptr = &player.buffs.proficiency_acorn
  },
  {
    .formal_name = "Acorn of Smell",

    .file_path = "Enchanted%20Acorns/smell_acorn.png",
    .description = "*50% increase acorn earnings*",

    .emoji_name = "smell_acorn",
    .emoji_id = 1045027185754390578,

    .stat_ptr = &player.buffs.smell_acorn
  }
};


/* Squirrel Stats */
enum SQUIRREL_STATS {
  STAT_SMELL,
  STAT_DEXTERITY,
  STAT_ACUITY,
  STAT_LUCK,
  STAT_PROFICIENCY,
  STAT_SIZE
};

struct File *stat_files = (struct File[])
{
  {
    .formal_name = "Smell", // GL
    .description = "*Multiplies "ACORNS" acorn earnings*",

    .stat_ptr = &player.stats.smell_lv
  },
  {
    .formal_name = "Dexterity", // SP
    .description = "*Increases "PINE_CONES" pine cone earnings*",

    .stat_ptr = &player.stats.dexterity_lv
  },
  {
    .formal_name = "Acuity", // NE
    .description = "*Increases "SEEDS" seed earnings*",

    .stat_ptr = &player.stats.acuity_lv
  },
  {
    .formal_name = "Luck", // DG
    .description = "*Increases biome material earnings*",

    .stat_ptr = &player.stats.luck_lv
  },
  {
    .formal_name = "Proficiency", // NP
    .description = "*Multiplies "XP" XP earnings*",

    .stat_ptr = &player.stats.proficiency_lv
  }
};

/* Squirrel Objects */
enum SQUIRREL {
  GRAY_SQUIRREL
};

struct File *squirrels = (struct File[])
{
  {
    .formal_name = "Gray Squirrel",
    .file_path = "GRASSLANDS%20CONTENT/Utils/gl_squirrel.png",

    .emoji_name = "gl_squirrel",
    .emoji_id = 1014655900373504130
  }
};

struct File *evo_squirrels = (struct File[])
{
  {
    .formal_name = "Evolutions",
    .file_path = "GRASSLANDS%20CONTENT/Utils/gl_squirrel_evolve.png",

    .emoji_name = "gl_squirrel_evolve",
    .emoji_id = 1006206208333586493
  }
};

/* Biomes */
struct Biome *biomes = (struct Biome[])
{
  { // GRASSLANDS
    .biome_material = {
      .formal_name = "Mooshrum",
      .file_path = "GRASSLANDS%20CONTENT/Utils/mooshrum.png",

      .emoji_name = "mooshrum",
      .emoji_id = 1033828590464278559
    },

    .biome_icon = {
      .formal_name = "Grasslands",
      .file_path = "GRASSLANDS%20CONTENT/Utils/grasslands_icon.png",

      .emoji_name = "gl_icon",
      .emoji_id = 1007390631016017991
    },

    .biome_scene_path = "GRASSLANDS%20CONTENT/Utils/grasslands_scene.gif",

    .material_ptr = &player.materials.mooshrums,
    .stat_ptr = &player.stats.smell_lv,

    .stat_value_multiplier = ACORN_MULTIPLIER,
    .stat_price_multiplier = ACORN_MULT_FACTOR,

    .encounters = (struct Encounter[]) { //encounters are listed in alphabetical order of their file name
      {
        .name = "Desolate Plains: Art of Hunting",
        .conflict = "A camoflauge snare has caught you! It didn't do much, the flimsy thing.",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/bandit_snare.png",

        .solution = {"Chew through the string", "Run as fast as you can to break the snare", "Wait for another squirrel to help you"}
      },
      {
        .name = "Imperium City: Rat Infestation",
        .conflict = "Rats! They surely don't like you. Who knows what they could be carrying...",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/city_rat.png",

        .solution = {"Give him a peace offering", "Hug him", "Back off"}
      },
      {
        .name = "Witch Swamp: Is it an Acorn?",
        .conflict = "This acorn looks like an acorn and tastes like an acorn. Is it an acorn?",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/conjured_acorn.png",

        .solution = {"Investigate the acorn", "Ignore it", "Take the acorn"}
      },
      {
        .name = "Imperium City: Bold Coyotes",
        .conflict = "These coyotes keep inching towards you. They're curious but do you want to take that chance?",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/coyote.png",

        .solution = {"Take your chances", "Punch him in the snoot and run up a tree", "Shriek to scare him off"}
      },
      {
        .name = "Desolate Plains: Bullseye",
        .conflict = "You spot a bandit hunter in the corner of your eye as he draws his bow!",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/hunter_detection.png",

        .solution = {"Hide behind a tree", "Lose his sight", "Throw an acorn"}
      },
      {
        .name = "Enchanted Woods: Unique Goldfish",
        .conflict = "This isn't your everyday goldfish. What's he doin' out of the water?",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/magical_goldfish.png",

        .solution = {"Go around him", "Eat him!", "Push him back into the water"}
      },
      {
        .name = "Desolate Plains: Suspicious Crops",
        .conflict = "Unlike the other crops, this one is sticky...",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/poisoned_crop.png",

        .solution = {"Walk away", "Try a kernel", "Wipe off the poison in hopes there's no trace"}
      },
      {
        .name = "Imperium City: Rampant Squirrels",
        .conflict = "Still cute and fluffy- sort of. They dont like other squirrels though.",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/savage_squirrel.png",

        .solution = {"Tame him", "Wait for him to look the other way", "Offer an acorn"}
      },
      {
        .name = "Witch Swamp: Are you you?",
        .conflict = "It's a friendly squirrel! Is it?",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/squirrel_doppleganger.png",

        .solution = {"Shake his paw", "Indicate you know it's a disguise", "Ignore him"}
      },
      {
        .name = "Witch Swamp: Poison from Below! Above?",
        .conflict = "A witch has spotted you and threw a flask!",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/thrown_flask.png",

        .solution = {"Dodge the flask", "Try to catch the flask", "Run at the witch!"}
      },
      {
        .name = "Enchanted Woods: Fluffy Unicorn",
        .conflict = "The unicorn seems friendly... perhaps you could just sneak that acorn from under him?",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/unicorn.png",

        .solution = {"Cause a distraction", "Bolt out and hope you scare it", "Aim for the throat!"}
      },
      {
        .name = "Enchanted Woods: Burly Pupper",
        .conflict = "A werewolf has spotted you and begins to chase you!",
        .file_path = "GRASSLANDS%20CONTENT/Encounters/werewolf.png",

        .solution = {"Run for your life!", "Aim for the throat!", "Run into a nearby bush"}
      }
    },

    .encounter_size = 12
  },

  {// SEEPING SANDS
    .biome_material = {
      .formal_name = "Cactus Flower",
      .file_path = "SEEPING%20SANDS%20CONTENT/Utils/cactus_flower.png",

      .emoji_name = "cacus_flowers",
      .emoji_id = 1006181928761839617
    },

    .biome_icon = {
      .formal_name = "Seeping Sands",
      .file_path = "SEEPING%20SANDS%20CONTENT/Utils/sp_icon.png",

      .emoji_name = "sp_icon",
      .emoji_id = 1042082262566572153
    },

    .biome_scene_path = "SEEPING%20SANDS%20CONTENT/Utils/seeping_sands_scene.gif",

    .material_ptr = &player.materials.cactus_flowers,
    .stat_ptr = &player.stats.dexterity_lv,

    .stat_value_multiplier = PINE_CONE_INC,
    .stat_price_multiplier = PINE_CONE_MULT_FACTOR,

    .encounters = (struct Encounter[]) {
      {
        .name = "Forgotten City: Unhappy Camels",
        .conflict = "This camel doesn't look too happy. Watch out for the spit!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/angry_camel.png",

        .solution = {"Sneak around", "Aim for the face!", "Cause a distraction"}
      },
      {
        .name = "Sandy Shores: Desparate Bobcat",
        .conflict = "There's an acorn next to the feasting bobcat.",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/bobcat.png",

        .solution = {"Don't look!", "Snatch it and hope you don't get eaten", "Shake a nearby bush"}
      },
      {
        .name = "Sandy Shores: Sleeping Cobra",
        .conflict = "You've caught the attention of this cobra!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/cobra.png",

        .solution = {"Flee the scene", "Aim for the neck!", "Dodge the lunge"}
      },
      {
        .name = "Forgotten City: Starving Coyotes",
        .conflict = "You've accidentally approached a pack of coyotes!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/coyote.png",

        .solution = {"Snatch the acorn and run", "Look somewhere else", "Run to the nearby group of people"}
      },
      {
        .name = "Wormhole: Shifting Doom",
        .conflict = "A massive worm is shifting through the sands...",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/death_worm.png",

        .solution = {"Lure and fight it", "Move in the opposite direction", "Run past it"}
      },
      {
        .name = "Golden Dunes: Desert Scavengers",
        .conflict = "A pack of hyenas are giving you the death stare...",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/hyena.png",

        .solution = {"Try to touch one", "Aim for the throat!", "Run to a nearby carcass"}
      },
      {
        .name = "Golden Dunes: The Curious Jackal",
        .conflict = "This jackal doesn't seem threatened nor scared. He's, curious?",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/jackal.png",

        .solution = {"Hand him an acorn", "Give him head pats", "Ignore him and scurry away"}
      },
      {
        .name = "Wormhole: Blood-sucking Fiends!",
        .conflict = "Leeches! Leeches everywhere! Small, but best not let too many latch on!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/leech.png",

        .solution = {"Crush them", "Scurry away!", "Run to a nearby carcass"}
      },
      {
        .name = "Sandy Shores: Distracting Vibrations",
        .conflict = "The rattling of the rattlesnake's tail catches your attention and you lock eyes...",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/rattlesnake.png",

        .solution = {"Ignore it and scurry away", "Passively approach it", "Aim for the throat!"}
      },
      {
        .name = "Forgotten City: Vicious Land Lobsters",
        .conflict = "A scorpion finds you and charges to attack you!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/scorpion.png",

        .solution = {"Dodge the pincer", "Throw an acorn", "Scurry Away!"},
      },
      {
        .name = "Ancient Wastelands: Here Comes the Calvary!",
        .conflict = "A living skeleton? A horse? Better hope it's not that unicorn...",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/skeletal_horse.png",

        .solution = {"Cause a distraction", "Bolt out and hope you scare it", "Aim for the throat!"}
      },
      {
        .name = "Ancient Wastelands: Squirrel Meets Squirrel",
        .conflict = "What looks to be a skeletal squirrel makes eye contaact with you... Well? You can't just ignore it!",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/skeletal_squirrel.png",

        .solution = {"Shake its paw", "Tame it", "Offer an acorn"}
      },
      {
        .name = "Golden Dunes: Desert Puppers",
        .conflict = "They look a bit hungry. Maybe not a friendly foe...",
        .file_path = "SEEPING%20SANDS%20CONTENT/Encounters/wild_dog.png",

        .solution = {"Offer an acorn", "Show them to some food", "Scurry awway!"}
      }
    },

    .encounter_size = 13
  }
};
