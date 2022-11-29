/*

  This file handles an owner kicking a member from their guild

*/


int scurry_kick(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *discord_msg) 
{
  // check if scurry is at war
  ERROR_INTERACTION((scurry.war_flag == 1), "You cannot kick a member while at war! Please retreat or finish the war first.");

  // check if owner
  ERROR_INTERACTION((event->member->user->id != player.scurry_id), "You don't have the permission to use this command!");

  // grab user
  struct discord_user user = { 0 };
  struct discord_ret_user ret_user = { .sync = &user };

  unsigned long user_id = strtobigint(trim_user_id(event->data->options->array[0].value));
  discord_get_user(client, user_id, &ret_user);

  //check if target user is trying to kick self
  ERROR_INTERACTION((user_id == event->member->user->id), "You can't kick yourself silly!");
  
  PGresult* target_user = SQL_query("select * from public.player where user_id = %ld and scurry_id = %ld", 
      user.id, event->member->user->id);

  // make sure scurry id matches owner id so you cant kick other scurry's members
  ERROR_DATABASE_RET((PQntuples(target_user) == 0), "That user isn't a part of your scurry!", target_user);

  // kick user by setting id = 0
  SQL_query("update public.player set scurry_id = 0 where user_id = %ld", user.id);

  discord_msg->content = format_str(SIZEOF_DESCRIPTION, 
      "**%s** has been kicked from **%s**!", user.username, scurry.scurry_name);

  PQclear(target_user);

  return 0;
}


/* Called on scurry_invite command */
void kick_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  //if an error is returned, an interaction was already generated
  if (scurry_kick(client, event, msg) )
    return;

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
}