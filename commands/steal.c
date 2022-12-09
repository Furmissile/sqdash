/*

  This file handles acorn stealing
  - A random player is selected from the query
  - Players literally steal from other players but chance is based on the difference of level
  - if the difference in player level > 0 ? 1 : abs(difference)
  - No need to set acorns to 0 if less than since the amount stolen is a percent of their acorns
  - If the steal failed, the player is notified, otherwise the steal is anonymous!

*/


// Instead of enumerating, create a struct to allocate
enum TARGET_USER {
  TARGET_USER_ID,
  TARGET_USER_LEVEL,
  TARGET_USER_ACORNS
};

void steal_acorns(struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  // select all players that isnt player or isnt in same scurry
  PGresult* t_user = (player.scurry_id > 0) ?
      SQL_query("select user_id, p_level, acorns from public.player where user_id != %ld and user_id != %ld and scurry_id != %ld order by random() LIMIT 1", 
      player.user_id, OWNER_ID, player.scurry_id)
      : SQL_query("select user_id, p_level, acorns from public.player where user_id != %ld and user_id != %ld order by random() LIMIT 1", 
      player.user_id, OWNER_ID);
  
  unsigned long t_user_id = strtobigint( PQgetvalue(t_user, 0, TARGET_USER_ID));
  int t_acorns = strtoint( PQgetvalue(t_user, 0, TARGET_USER_ACORNS) );

  // difference in level
  int delta_lv = player.level - strtoint( PQgetvalue(t_user, 0, TARGET_USER_LEVEL) );

  // what gets put into formula
  int offset = (delta_lv >= 0) ? 3 : abs(delta_lv);

  float chance = 1.0/(offset +1.0);

  int stolen_acorns = t_acorns * chance;

  if (rand() % MAX_CHANCE > chance *100)
  {
    embed->color = (int)ACTION_FAILED;
    embed->title = format_str(SIZEOF_TITLE, "Steal Failed!");
    discord_msg->content = format_str(SIZEOF_DESCRIPTION, "<@!%ld>, someone failed to snatch your acorns!", t_user_id);
    embed->description = format_str(SIZEOF_DESCRIPTION, 
        "<@!%ld> failed to steal **%s** "ACORNS" acorns! \n\n-**20** "ENERGY" Energy", 
        player.user_id, num_str(stolen_acorns));
  }
  else {
    SQL_query("update public.player set acorns = %d where user_id = %ld", 
        t_acorns - stolen_acorns, t_user_id);
    
    int golden_acorns = (chance *100 < 25) ? genrand(25, 15)
        : (chance *100 < 90) ? genrand(50, 25) 
        : genrand(75, 25);

    player.acorns += stolen_acorns;
    player.passive_acorns += stolen_acorns;
    player.golden_acorns += golden_acorns;

    embed->color = (int)ACTION_SUCCESS;
    embed->title = format_str(SIZEOF_TITLE, "Steal Successful!");
    embed->description = format_str(SIZEOF_DESCRIPTION,
        "You anonymously stole **%s** "ACORNS" acorns! \n"
        "+**%d** "GOLDEN_ACORNS" Golden Acorns \n"
        "\n-**%d** "ENERGY" Energy",
        num_str(stolen_acorns), golden_acorns, STEAL_ENERGY_COST);
  }
  
  if (player.user_id != OWNER_ID)
    player.energy -= STEAL_ENERGY_COST;
  player.main_cd = time(NULL) + COOLDOWN;
}

/* Listens for slash command interactions */
int steal_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  energy_regen();
  // Really more of an interaction error, just this macro fits the bill better
  ERROR_INTERACTION((time(NULL) < player.main_cd), "Please wait a little longer before sending another command!");
  ERROR_INTERACTION((player.energy < STEAL_ENERGY_COST), "You need more energy!");

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  steal_acorns(msg);

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = msg->content,

      .embeds = &(struct discord_embeds) 
      {
        .array = msg->embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(msg->embed);
  free(msg->buttons);
  free(msg);

  update_player_row(event->member->user->id, player);
  scurry = (struct Scurry) { 0 };

  return 0;
}