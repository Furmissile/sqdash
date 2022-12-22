/*

  This file handles an owner kicking a member from their guild
  target user must
  - be a member of the scurry
  - not the owner
  - not their self
  - have scurry not at war

*/

struct Kick_Info {
  struct Message *discord_msg;
  unsigned long t_user_id;
  char* username;
};

void create_kick_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Kick_Info *kick_info)
{
  struct Message *msg = kick_info->discord_msg;
  // kick user by setting id = 0
  SQL_query("update public.player set scurry_id = 0 where user_id = %ld", kick_info->t_user_id);

  msg->content = format_str(SIZEOF_DESCRIPTION, 
      "%s has been kicked from **%s**!", kick_info->username, scurry.scurry_name);

  struct discord_interaction_response interaction = 
  {
    // function will always generate a new message
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .content = msg->content,
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
  player = (struct Player) { 0 };

  free(msg);
  free(kick_info);
}

void kick_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct Kick_Info *kick_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  kick_info->username = format_str(SIZEOF_TITLE, "**%s**", user->username);

  create_kick_interaction(client, event, kick_info);
}

void kick_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  struct Kick_Info *kick_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  kick_info->username = format_str(SIZEOF_TITLE, "<@%ld>", member->user->id);

  create_kick_interaction(client, event, kick_info);
}

void kick_not_member(struct discord *client, struct discord_response *resp)
{
  struct Kick_Info *kick_info = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_ret_user ret_user = {
    .done = &kick_user,
    .data = kick_info,
    .keep = event
  };

  discord_get_user(client, kick_info->t_user_id, &ret_user);
}

/* Called on scurry_invite command */
int kick_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  struct Kick_Info *kick_info = calloc(1, sizeof(struct Kick_Info));

  kick_info->discord_msg = msg;
  kick_info->t_user_id = strtobigint(trim_user_id(event->data->options->array[0].value));

  // check if scurry is at war
  ERROR_INTERACTION((scurry.war_flag == 1), "You cannot kick a member while at war! Please retreat or finish the war first.");

  // check if owner
  ERROR_INTERACTION((event->member->user->id != player.scurry_id), "You don't have the permission to use this command!");

  //check if target user is trying to kick self
  ERROR_INTERACTION((kick_info->t_user_id == event->member->user->id), "You can't kick yourself silly!");

  PGresult* target_user = SQL_query("select * from public.player where user_id = %ld and scurry_id = %ld", 
      kick_info->t_user_id, event->member->user->id);

  // make sure scurry id matches owner id so you cant kick other scurry's members
  ERROR_DATABASE_RET((PQntuples(target_user) == 0), "That user isn't a part of your scurry!", target_user);

  PQclear(target_user);

  struct discord_ret_guild_member ret_member = {
    .done = &kick_member,
    .fail = &kick_not_member,
    .data = kick_info,
    .keep = event
  };

  discord_get_guild_member(client, event->guild_id, kick_info->t_user_id, &ret_member);

  return 0;
}