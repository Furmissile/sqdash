/*

  This header defines constants

  Changes made since last push:
    - Altered stats
*/

// switch beta on and off
#define BETA

#ifdef BETA
  #undef BETA
  #define BETA "beta_"
  #define APPLICATION_ID 1048439491607674930
  #define STATUS_CHANNEL 1017203573890752522
#else
  #define BETA
  #define APPLICATION_ID 905163158149287936
  #define STATUS_CHANNEL 1046635264883294259
#endif

#define genrand(base, range) ( (rand() % (range +1)) + base ) // range +1 since rand is [base, range)

#define ADD_TO_BUFFER(buffer, size, format, ...) \
    snprintf(buffer + strlen(buffer), size - strlen(buffer), format, ##__VA_ARGS__)

/* @@@ SERVER CONSTANTS @@@ */
#define GUILD_ID 905167903224123473
#define OWNER_ID 582648847881338961

// PATH CONSTANTS
#define SQ_CHEM_PATH "Squirrel%20Dash%20Utils/sq_alchemist.png"
#define SQ_BOOKIE_PATH "Squirrel%20Dash%20Utils/squirrel_bookie.png"
#define WELCOME_MSG_PATH "Squirrel%20Dash%20Utils/welcome_gif.gif"
#define RULES_BK_PATH "Squirrel%20Dash%20Utils/rules_book.png"
#define BUNNY_ED_PATH "Squirrel%20Dash%20Utils/bunny.png"
#define VERIFY_PLUS_PATH "Symbols/plus.png"

#define ACORN_LB_ICON_PATH
#define COURAGE_LB_ICON_PATH

// SUPPORT SERVER SPECIFIC IDS
#define WELCOME_CHANNEL_ID 1046634205729275955
#define MAIN_CHANNEL_ID 1046635264883294259 //to be used as system messages
#define VERIFY_CHANNEL_ID 1046813534790635550
#define MEMBER_ROLE_ID 1046627142345170984

#define ACTION_SUCCESS 0x00ff00
#define ACTION_FAILED 0xff0000
#define ACTION_UNDEFINED 0x0

#define sq_num(num) ( (num) * (num) )
#define req_xp(level) sq_num(level * 6) +125

#define SCURRY_MEMBER_MAX 15
#define SCURRY_MEMBER_REQ 5 // requirement to participate in wars

#define SCURRY_CREATION_COST 50000 //50,000
#define SCURRY_LV_REQ 20

// RANK REQUIREMENTS
#define SEED_NOT_MAX 5000
#define ACORN_SNATCHER_MAX 10000
#define SEED_SNIFFER_MAX 25000
#define OAKFFICIAL_MAX 50000
// Royal Nut max -> infinity

// WAR ACORN CAPS PER RANK
#define SEED_NOT_CAP 500
#define ACORN_SNATCHER_CAP 1000
#define SEED_SNIFFER_CAP 2500
#define OAKFFICIAL_CAP 5000
#define ROYAL_NUT_CAP 10000

// Multiply into rank
#define BASE_COURAGE_MULT 0.05f

#define MAX_ENERGY 100

/* @@@ COOLDOWNS @@@ */
#define COOLDOWN 3
#define BASE_ENERGY_CD 180 //3 minutes/ energy
#define INVITE_CD 120
#define MAIN_ENERGY_COST 2
#define STEAL_ENERGY_COST 20
#define STEAL_MINIMUM 500


/* @@@ CHANCE DATA @@@ */
#define JUNK_CHANCE      10  // 10 % -- No acorns
#define COMMON_CHANCE    60  // 50 % -- Handful of acorns
#define UNCOMMON_CHANCE  80  // 20 % -- Mouthful of acorns 
#define CONTAINER_CHANCE 95  // 15 % -- Lost Stash
#define MAX_CHANCE       100 // 5  % -- Sack of acorns

#define ENCOUNTER_CHANCE 20 // 20 % -- checked separately

#define MAX_MATERIAL_CHANCE 60 //highest chance to get a biome material

#define DAILY_COOLDOWN 86400

/* @@@ INTERACTION TYPES  @@@ */
#define TYPE_MAIN_MSG 'r'
#define TYPE_ENCOUNTER_MSG 'e'

#define TYPE_STORE 's'
#define TYPE_SEEDY 'd'
#define TYPE_BUNNY 'y'
#define TYPE_BIOME 'b'
#define TYPE_E_ACORN 'a'
#define TYPE_INVITE 'i'
#define TYPE_SCURRY_WAR 'x'


/* @@@ EMBED BUFFER SIZES @@@ */
#define SIZEOF_TITLE 256         // Includes field name, author name, and title

#define SIZEOF_DESCRIPTION 4096

#define SIZEOF_FIELD_VALUE 1024

#define SIZEOF_FOOTER_TEXT 2048

#define SIZEOF_CUSTOM_ID 64

#define SIZEOF_URL 1024

#define SIZEOF_SCURRY_NAME 16


/* @@@ STAT CONSTANTS @@@ */
#define STAT_EVOLUTION 5
#define PLAYER_EVOLUTION 10

//Unit per upgrade values
#define UNIT_ACORN 1000

#define GOLDEN_ACORN_BUFF_COST 1000

#define COLOR_COST 1500

/*
* Stats include:
	i. Smell -> Multiplies acorn earnings
	ii. Proficiency -> Multiplies XP earnings
	iii. Luck -> Increases golden acorn findings
	iv. Endurance -> Increase HP
*/

/* Price multiplier factors */
#define PROFICIENCY_FACTOR 2
#define LUCK_FACTOR 5
#define STRENGTH_FACTOR 10

/* @@@ The following stats are to increase resource earning, NOT to collect them @@@*/

// See generate_factor() for details
#define PROFICIENCY_VALUE 0.1f
#define LUCK_VALUE 25
#define STRENGTH_VALUE 50
 
// Stat level brackets (for some style)
#define BRONZE_BRACKET 20
#define SILVER_BRACKET 30
#define GOLD_BRACKET 50

// SEASON MULTIPLIERS
#define SPRING_MULT 1.2f
#define SUMMER_MULT 1.1f
#define FALL_MULT 1.05f
#define WINTER_MULT 1 // Explicit for definition

#define ERROR_STATUS -1

#define MATCH_USER_ID(user_id, message) \
    if (!strstr(event->data->custom_id, user_id)) { \
      error_message(client, event, message); \
      return; \
    }

// Game errors
#define ERROR_INTERACTION(error, message) \
    if (error) { \
      error_message(client, event, message); \
      return ERROR_STATUS; \
    }

#define SIZEOF_SQL_COMMAND 1024

// Errors involving clearing a query
#define ERROR_DATABASE_RET(error, message, PQ_result) \
    if (error) { \
      error_message(client, event, message); \
      PQclear(PQ_result); \
      return ERROR_STATUS; \
    }

/* @@@ SOME EMOJI CONSTANTS @@@ */
// *The following constants are sometimes used definitively

#define ENERGY "<:energy:911642184397258752>"

#define XP "<:xp:1001511753294815424>"
#define ACORNS "<:acorns:1045028765392187402>"

#define GOLDEN_ACORNS "<:golden_acorn:1045032005420728430>"
#define CATNIP "<:catnip:1052250368039452732>"

#define LOST_STASH "<:lost_stash:1044620525944705146>"

#define SEEDS "<:seeds:1003457649221828610>"
#define PINE_CONES "<:pine_cones:1044620708996722708>"

#define QUEST_MARKER "<:quest_marker:1046493067500335104>"
#define HELP_MARKER "<:no_acorns:1044620527223975957>"

#define BRONZE_STAHR "<:bronze_stahr:1010662617217511434>"
#define SILVER_STAHR "<:silver_stahr:1010662591615475835>"
#define STAHR "<:stahr:1045705606134251601>"

#define OFF_ARROW "<:offset_arrow:1010924151063715841>"
#define INDENT "<:empty_space:1019379796037337118>"

#define GUILD_ICON "<:guild_icon:1020714354351542362>"
#define OPPOSING_SCURRY "<:opposing_scurry:1037000813291241524>"
#define LEADER "<:leader:1035976066965196861>"
#define COURAGE "<:courage:1045555306832347147>"
#define STOLEN_ACORNS "<:stolen_acorn:1055143210839720067>"

#define ACORN_COUNT "<:passive_acorns:1050407923823677502>"
#define HEALTH "<:health:1060626677715181588>"
