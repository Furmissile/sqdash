/*

  This file handles all mechanics associated with scurry wars including:
    - war mechanics
    - aftermath

*/


void factor_war(int item_type)
{
  rewards.stolen_acorns = rewards.acorns;
  player.stolen_acorns += rewards.stolen_acorns;

  switch (item_type) 
  {
    case TYPE_ACORN_HANDFUL:
      rewards.courage = (scurry.courage < SEED_NOT_MAX) ? genrand(3, 5)
          : (scurry.courage < ACORN_SNATCHER_MAX) ? genrand(5, 10)
          : (scurry.courage < SEED_SNIFFER_MAX) ? genrand(10, 15)
          : (scurry.courage < OAKFFICIAL_MAX) ? genrand(15, 25) : genrand(25, 50);
      break;
    case TYPE_ACORN_MOUTHFUL:
      rewards.courage = (scurry.courage < SEED_NOT_MAX) ? genrand(10, 15)
          : (scurry.courage < ACORN_SNATCHER_MAX) ? genrand(15, 25)
          : (scurry.courage < SEED_SNIFFER_MAX) ? genrand(25, 50)
          : (scurry.courage < OAKFFICIAL_MAX) ? genrand(50, 75) : genrand(75, 125);
      break;
    case TYPE_LOST_STASH:
      rewards.courage = (scurry.courage < SEED_NOT_MAX) ? genrand(15, 30)
          : (scurry.courage < ACORN_SNATCHER_MAX) ? genrand(30, 50)
          : (scurry.courage < SEED_SNIFFER_MAX) ? genrand(50, 100)
          : (scurry.courage < OAKFFICIAL_MAX) ? genrand(100, 150) : genrand(150, 250);
      break;
    case TYPE_ACORN_SACK:
      rewards.courage = (scurry.courage < SEED_NOT_MAX) ? genrand(30, 50)
          : (scurry.courage < ACORN_SNATCHER_MAX) ? genrand(50, 75)
          : (scurry.courage < SEED_SNIFFER_MAX) ? genrand(100, 150)
          : (scurry.courage < OAKFFICIAL_MAX) ? genrand(150, 250) : genrand(250, 500);
  }

  scurry.courage += rewards.courage;
}

/* REQUIRES factor_war() to be called! */
void scurry_war_update(const struct discord_interaction *event) 
{
  // avoid conflict with forage button so check if not forage
  if (event->data->custom_id && event->data->custom_id[0] == TYPE_SCURRY_WAR)
  {
    scurry.war_flag = 0;
    return;
  }

  // search for another scurry
  PGresult* opponents = SQL_query(conn, "select owner_id from public.scurry where owner_id != %ld and war_flag = 1",
      scurry.scurry_owner_id);
  
  if (PQntuples(opponents) > 0)
  {
    struct Scurry opponent = load_scurry_struct(strtobigint(PQgetvalue(opponents, rand() % PQntuples(opponents), DB_SCURRY_OWNER_ID)));

    opponent.war_acorns -= rewards.stolen_acorns;
  
    if (opponent.war_acorns <= 0) 
    {
      opponent.war_acorns = 0;
      opponent.war_flag = 0;
    }

    update_scurry_row(opponent);
  }
  // simulate a bot object that steals scurry acorns
  else if (rand() % MAX_CHANCE < 50 && rewards.stolen_acorns)
  {
    scurry.war_acorns -= rewards.stolen_acorns;

    if (scurry.war_acorns <= 0)
    {
      scurry.war_acorns = 0;
      scurry.war_flag = 0;
    }
  }

}