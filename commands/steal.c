/*

  This file handles acorn stealing
  - A random player is selected from the query
  - Players literally steal from other players but chance is based on the difference of level
  - if the difference in player level > 0 ? 1 : abs(difference)
  - No need to set acorns to 0 if less than since the amount stolen is a percent of their acorns
  - If the steal failed, the player is notified, otherwise the steal is anonymous!
  - Players cannot steal below a certain amount of acorns on success
  - If the target is not a member of the guild, their username is used instead of a mention

*/

struct Steal_Info {
  unsigned long t_user_id;
  int t_level;
  int t_acorns;

  struct Message *discord_msg;
  char* username;
};

// Instead of enumerating, create a struct to allocate
enum TARGET_USER {
  TARGET_USER_ID,
  TARGET_USER_LEVEL,
  TARGET_USER_ACORNS
};

void create_steal_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg)
{
  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

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
  free(msg);

  update_player_row(event->member->user->id, player);
  scurry = (struct Scurry) { 0 };
}

void steal_acorns(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Steal_Info *steal_info)
{
  struct Message *discord_msg = steal_info->discord_msg;

  struct discord_embed *embed = discord_msg->embed;

  //Load Author
  embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  // difference in level
  int delta_lv = player.level - steal_info->t_level;

  // what gets put into formula
  int offset = (delta_lv >= 0) ? 1 : abs(delta_lv);

  float chance = 1.0/(offset +3.0);

  float random_percent = (float)genrand(chance*100, 25)/100;

  // int stolen_acorns = steal_info->t_acorns * chance;
  int stolen_acorns = steal_info->t_acorns * random_percent;

  if (rand() % MAX_CHANCE > chance *100)
  {
    embed->color = (int)ACTION_FAILED;
    embed->title = format_str(SIZEOF_TITLE, "Steal Failed!");

    discord_msg->content = format_str(SIZEOF_DESCRIPTION, "**%s**, someone failed to snatch your acorns!", steal_info->username);

    embed->description = format_str(SIZEOF_DESCRIPTION, 
        "<@%ld> failed to steal **%s** "ACORNS" acorns! \n", 
        player.user_id, num_str(stolen_acorns));
  }
  else {
    SQL_query(conn, "update public.player set acorns = %d where user_id = %ld", 
        steal_info->t_acorns - stolen_acorns, steal_info->t_user_id);
    
    int golden_acorns = (chance *100 < 25) ? genrand(25, 15)
        : (chance *100 < 90) ? genrand(50, 25) 
        : genrand(75, 25);

    player.acorns += stolen_acorns;
    player.acorn_count += stolen_acorns;
    player.golden_acorns += golden_acorns;

    embed->color = (int)ACTION_SUCCESS;
    embed->title = format_str(SIZEOF_TITLE, "Steal Successful!");
    embed->description = format_str(SIZEOF_DESCRIPTION,
        "You anonymously stole **%s** "ACORNS" acorns! \n"
        "+**%d** "GOLDEN_ACORNS" Golden Acorns \n",
        num_str(stolen_acorns), golden_acorns);
  }

  free(steal_info);
  
  energy_status(discord_msg, STEAL_ENERGY_COST);

  player.main_cd = time(NULL) + COOLDOWN;

  create_steal_interaction(client, event, discord_msg);
}

void steal_from_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  steal_info->username = format_str(SIZEOF_TITLE, "**%s**", user->username);

  steal_acorns(client, event, steal_info);
}

void steal_from_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  steal_info->username = format_str(SIZEOF_TITLE, "<@%ld>", member->user->id);

  steal_acorns(client, event, steal_info);
}

void steal_not_member(struct discord *client, struct discord_response *resp)
{
  struct Steal_Info *steal_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_ret_user ret_user = {
    .done = &steal_from_user,
    .data = steal_info,
    .keep = event
  };

  discord_get_user(client, steal_info->t_user_id, &ret_user);
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

  // select all players that isnt the player, owner, or isnt in same scurry
  PGresult* t_user = (player.scurry_id > 0) ?
      SQL_query(conn, "select user_id, p_level, acorns from public.player \
          where user_id != %ld and user_id != %ld and acorns > %d and scurry_id != %ld and acorns > 0 \
          order by random() LIMIT 1", 
          event->member->user->id, OWNER_ID, STEAL_MINIMUM, player.scurry_id)
      : SQL_query(conn, "select user_id, p_level, acorns from public.player \
          where user_id != %ld and user_id != %ld and acorns > %d \
          order by random() LIMIT 1", 
          event->member->user->id, OWNER_ID, STEAL_MINIMUM );

  ERROR_DATABASE_RET((PQntuples(t_user) == 0), "No target users are available right now!", t_user);

  struct Steal_Info *steal_info = calloc(1, sizeof(struct Steal_Info));
  steal_info->discord_msg = msg;

  steal_info->t_user_id = strtobigint( PQgetvalue(t_user, 0, TARGET_USER_ID));
  steal_info->t_level = strtoint( PQgetvalue(t_user, 0, TARGET_USER_LEVEL) );
  steal_info->t_acorns = strtoint( PQgetvalue(t_user, 0, TARGET_USER_ACORNS) );

  struct discord_ret_guild_member ret_member = {
    .done = &steal_from_member,
    .fail = &steal_not_member,
    .data = steal_info,
    .keep = event
  };

  discord_get_guild_member(client, event->guild_id, steal_info->t_user_id, &ret_member);

  return 0;
}