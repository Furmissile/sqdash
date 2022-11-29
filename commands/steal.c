/*

  This file handles acorn stealing
  - A random player is selected from the query
  - Players gain acorns and the victim loses acorns based on their level
  - If the player has more acorns than needed ? subtract acorns and add lucky coins : set to 0
  - If the steal failed, the player is notified, otherwise the steal is anonymous!
  - There should be a 50% chance to successfully steal another player's acorns

*/

enum TARGET_USER {
  TARGET_USER_ID,
  TARGET_USER_LEVEL,
  TARGET_USER_ACORNS
};

#define COMMON_STEAL_CHANCE 65
#define RARE_STEAL_CHANCE 90

void steal_acorns(struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  PGresult* t_user = SQL_query("select user_id, p_level, acorns from public.player where user_id != %ld and scurry_id != %ld", 
      player.user_id, player.scurry_id);

  int selected_player = genrand(0, PQntuples(t_user) -1);
  int base_value = genrand(50, MAX_CHANCE);
  int stolen_acorns = ( strtoint( PQgetvalue(t_user, selected_player, TARGET_USER_LEVEL) ) ) * base_value;
  unsigned long t_player_id = strtobigint(PQgetvalue(t_user, selected_player, TARGET_USER_ID));

  if (rand() % MAX_CHANCE < 50)
  {
    embed->color = (int)ACTION_FAILED;
    embed->title = format_str(SIZEOF_TITLE, "Steal Failed!");
    discord_msg->content = format_str(SIZEOF_DESCRIPTION, "<@!%ld>, someone failed to snatch your acorns!", t_player_id);
    embed->description = format_str(SIZEOF_DESCRIPTION, 
        "<@!%ld> failed to steal %d acorns! \n\n-**10** "ENERGY" Energy", 
        player.user_id, stolen_acorns);
  }
  else {
    int t_user_acorns = strtoint(PQgetvalue(t_user, selected_player, TARGET_USER_ACORNS));
    if (t_user_acorns - stolen_acorns < 0)
      SQL_query("update public.player set acorns = 0 where user_id = %ld", t_player_id);
    else
      SQL_query("update public.player set acorns = %d where user_id = %ld", 
          t_user_acorns - stolen_acorns, t_player_id);
    
    int golden_acorns = (base_value < COMMON_STEAL_CHANCE + 50) ? genrand(15, 10)
        : (base_value < RARE_STEAL_CHANCE + 50) ? genrand(25, 25) 
        : genrand(50, 25);

    player.acorns += stolen_acorns;
    player.golden_acorns += golden_acorns;

    embed->color = (int)ACTION_SUCCESS;
    embed->title = format_str(SIZEOF_TITLE, "Steal Successful!");
    embed->description = format_str(SIZEOF_DESCRIPTION,
        "You anonymously stole %d acorns! \n"
        "+**%d** "GOLDEN_ACORNS" Lucky Coins \n"
        "\n\n-**%d** "ENERGY" Energy",
        STEAL_ENERGY_COST, stolen_acorns, golden_acorns);
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