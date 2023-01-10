/*

  This file handles database interactions.

*/

PGconn* establish_connection(char* conninfo)
{
  PGconn* db_conn = PQconnectdb(conninfo);

  if (PQstatus(db_conn) != CONNECTION_OK)
  {
    fprintf(stderr, "%s", PQerrorMessage(db_conn) );
    PQfinish(db_conn);
    exit(1);
  }

  PQexec(db_conn, "SELECT pg_catalog.set_config('search_path', '', false)");

  return db_conn;
}

struct Player load_player_struct(unsigned long user_id)
{
  PGresult* search_player = SQL_query(conn, "select * from public.player where user_id = %ld",
    user_id);

  if (PQntuples(search_player) == 0)
  {
    SQL_query(conn, "BEGIN; \
      insert into public.player values(%ld, 1, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, %ld); \
      insert into public.materials values(%ld, 0, 0, 0, 0, 0, 0, 0); \
      insert into public.stats values(%ld, 1, 1, 1, 1, 1); \
      insert into public.buffs values(%ld, 0, 0, 0, 0, 0); \
      COMMIT;", 
      user_id, time(NULL), user_id, user_id, user_id);
  }

  PQclear(search_player);

  search_player = SQL_query(conn, "select * from public.player \
    join public.materials on player.user_id = materials.user_id \
    join public.stats on player.user_id = stats.user_id \
    join public.buffs on player.user_id = buffs.user_id \
    where player.user_id = %ld",
    user_id);

  player = (struct Player) { 0 };

  struct Player player_res = (struct Player)
  {
    .user_id = strtobigint(PQgetvalue(search_player, 0, DB_USER_ID) ),
    .level = strtoint( PQgetvalue(search_player, 0, DB_LEVEL) ),
    .xp = strtoint( PQgetvalue(search_player, 0, DB_XP) ),
    .acorns = strtoint( PQgetvalue(search_player, 0, DB_ACORNS) ),
    .active_squirrel = strtoint( PQgetvalue(search_player, 0, DB_ACTIVE_SQUIRREL) ),
    .biome = strtoint( PQgetvalue(search_player, 0, DB_BIOME) ),
    .select_encounter = strtoint( PQgetvalue(search_player, 0, DB_SELECT_ENCOUNTER) ),
    .color = strtoint( PQgetvalue(search_player, 0, DB_COLOR) ),
    .main_cd = strtobigint( PQgetvalue(search_player, 0, DB_MAIN_CD) ),
    .energy = strtoint( PQgetvalue(search_player, 0, DB_ENERGY) ),
    .golden_acorns = strtoint( PQgetvalue(search_player, 0, DB_GOLDEN_ACORNS) ),
    .scurry_id = strtobigint( PQgetvalue(search_player, 0, DB_SCURRY_ID) ),
    .stolen_acorns = strtoint( PQgetvalue(search_player, 0, DB_STOLEN_ACORNS) ),
    .acorn_count = strtoint( PQgetvalue(search_player, 0, DB_ACORN_COUNT) ),
    .catnip = strtoint( PQgetvalue(search_player, 0, DB_CATNIP) ),
    .daily_cd = strtobigint( PQgetvalue(search_player, 0, DB_DAILY_CD) ),

    .materials = {
      .pine_cones = strtoint( PQgetvalue(search_player, 0, DB_PINE_CONES) ),
      .seeds = strtoint( PQgetvalue(search_player, 0, DB_SEEDS) ),
      .mooshrums = strtoint( PQgetvalue(search_player, 0, DB_MOOSHRUMS) ),
      .cactus_flowers = strtoint( PQgetvalue(search_player, 0, DB_CACTUS_FLOWERS) ),
      .juniper_berries = strtoint( PQgetvalue(search_player, 0, DB_JUNIPER_BERRIES) ),
      .blueberries = strtoint( PQgetvalue(search_player, 0, DB_BLUEBERRIES) ),
      .dark_chestnuts = strtoint( PQgetvalue(search_player, 0, DB_DARK_CHESTNUTS) )
    },

    .stats = {
      .smell_lv = strtoint( PQgetvalue(search_player, 0, DB_SMELL_LV) ),
      .dexterity_lv = strtoint( PQgetvalue(search_player, 0, DB_DEXTERITY_LV) ),
      .acuity_lv = strtoint( PQgetvalue(search_player, 0, DB_ACUITY_LV) ),
      .luck_lv = strtoint( PQgetvalue(search_player, 0, DB_LUCK_LV) ),
      .proficiency_lv = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_LV) )
    },

    .buffs = {
      .smell_acorn = strtoint( PQgetvalue(search_player, 0, DB_SMELL_ACORN) ),
      .endurance_acorn = strtoint( PQgetvalue(search_player, 0, DB_ENDURANCE_ACORN) ),
      .acuity_acorn = strtoint( PQgetvalue(search_player, 0, DB_ACUITY_ACORN) ),
      .luck_acorn = strtoint( PQgetvalue(search_player, 0, DB_LUCK_ACORN) ),
      .proficiency_acorn = strtoint( PQgetvalue(search_player, 0, DB_PROFICIENCY_ACORN) )
    }
  };

  player_res.max_biome = factor_biome(player_res.level);

  PQclear(search_player);

  return player_res;
}

struct Scurry load_scurry_struct(unsigned long scurry_id)
{
  struct Scurry scurry_res = (struct Scurry) { 0 };

  if (scurry_id > 0)
  {
    PGresult* scurry_db = SQL_query(conn, "select * from public.scurry where owner_id = %ld", scurry_id);

    scurry = (struct Scurry) { 0 };

    scurry_res = (struct Scurry) 
    {
      .scurry_owner_id = strtobigint( PQgetvalue(scurry_db, 0, DB_SCURRY_OWNER_ID) ),
      .scurry_name = format_str(32, PQgetvalue(scurry_db, 0, DB_SCURRY_NAME) ),
      .courage = strtoint( PQgetvalue(scurry_db, 0, DB_COURAGE) ),
      .war_acorns = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_ACORNS) ),
      .war_flag = strtoint( PQgetvalue(scurry_db, 0, DB_WAR_FLAG) ),
      .rank = (scurry_res.courage < SEED_NOT_MAX) ? SEED_NOT
          : (scurry_res.courage < ACORN_SNATCHER_MAX) ? ACORN_SNATCHER
          : (scurry_res.courage < SEED_SNIFFER_MAX) ? SEED_SNIFFER
          : (scurry_res.courage < OAKFFICIAL_MAX) ? OAKFFICIAL : ROYAL_NUT,
      
      .war_acorn_cap = (scurry_res.courage < SEED_NOT_MAX) ? SEED_NOT_CAP
          : (scurry_res.courage < ACORN_SNATCHER_MAX) ? ACORN_SNATCHER_CAP
          : (scurry_res.courage < SEED_SNIFFER_MAX) ? SEED_SNIFFER_CAP
          : (scurry_res.courage < OAKFFICIAL_MAX) ? OAKFFICIAL_CAP : ROYAL_NUT_CAP
    };

    if (scurry_res.courage < SEED_NOT_MAX)
    {
      scurry_res.rank = SEED_NOT;
      scurry_res.war_acorn_cap = SEED_NOT_CAP;
    }
    else if (scurry_res.courage < ACORN_SNATCHER_MAX)
    {
      scurry_res.rank = ACORN_SNATCHER;
      scurry_res.war_acorn_cap = ACORN_SNATCHER_CAP;
    }
    else if (scurry_res.courage < SEED_SNIFFER_MAX)
    {
      scurry_res.rank = SEED_SNIFFER;
      scurry_res.war_acorn_cap = SEED_SNIFFER_CAP;
    }
    else if (scurry_res.courage < OAKFFICIAL_MAX)
    {
      scurry_res.rank = OAKFFICIAL;
      scurry_res.war_acorn_cap = OAKFFICIAL_CAP;
    }
    else {
      scurry_res.rank = ROYAL_NUT;
      scurry_res.war_acorn_cap = ROYAL_NUT_CAP;
    }
    
    PQclear(scurry_db);
  }

  return scurry_res;
}

void update_player_row(unsigned long user_id, struct Player player_res)
{
  char* sql_str = format_str(SIZEOF_SQL_COMMAND,
    "BEGIN; \
    update public.player set \
      p_level = %d, \
      xp = %0.0f, \
      acorns = %d, \
      biome = %d, \
      select_encounter = %d, \
      color = %d, \
      main_cd = %ld, \
      energy = %d, \
      golden_acorns = %d, \
      scurry_id = %ld, \
      stolen_acorns = %d, \
      acorn_count = %d, \
      catnip = %d, \
      daily_cd = %ld \
    where user_id = %ld;",
      player_res.level, player_res.xp, player_res.acorns, player_res.biome, player_res.select_encounter,
      player_res.color, player_res.main_cd, player_res.energy, player_res.golden_acorns, player_res.scurry_id,
      player_res.stolen_acorns,player_res.acorn_count, player_res.catnip, player_res.daily_cd,
      user_id);
  
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.materials set \
      pine_cones = %d, \
      seeds = %d, \
      mooshrums = %d, \
      cactus_flowers = %d, \
      juniper_berries = %d, \
      blueberries = %d, \
      dark_chestnuts = %d \
    where user_id = %ld;",
      player_res.materials.pine_cones, player_res.materials.seeds, player_res.materials.mooshrums,
      player_res.materials.cactus_flowers, player_res.materials.juniper_berries, player_res.materials.blueberries,
      player_res.materials.dark_chestnuts, user_id);
    
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.stats set \
      smell_lv = %d, \
      dexterity_lv = %d, \
      acuity_lv = %d, \
      luck_lv = %d, \
      proficiency_lv = %d \
    where user_id = %ld;",
      player_res.stats.smell_lv, player_res.stats.dexterity_lv, player_res.stats.acuity_lv, player_res.stats.luck_lv,
      player_res.stats.proficiency_lv, user_id);
  
  ADD_TO_BUFFER(sql_str, SIZEOF_SQL_COMMAND,
    "update public.buffs set \
      smell_acorn = %d, \
      endurance_acorn = %d, \
      acuity_acorn = %d, \
      luck_acorn = %d, \
      proficiency_acorn = %d \
    where user_id = %ld; \
    COMMIT;",
      player_res.buffs.smell_acorn, player_res.buffs.endurance_acorn, player_res.buffs.acuity_acorn, player_res.buffs.luck_acorn,
      player_res.buffs.proficiency_acorn, user_id);

  SQL_query(conn, sql_str);

  free(sql_str);
}

void update_scurry_row(struct Scurry scurry_res)
{
  char* sql_str = format_str(SIZEOF_SQL_COMMAND,
    "update public.scurry set \
        courage = %d, \
        war_acorns = %d, \
        war_flag = %d \
      where owner_id = %ld",
        scurry_res.courage, scurry_res.war_acorns, scurry_res.war_flag,
        scurry_res.scurry_owner_id);
  
  PQexec(conn, sql_str);

  free(sql_str);
}