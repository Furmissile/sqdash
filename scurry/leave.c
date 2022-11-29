/*

  This file handles leaving a scurry on the player's prerogative

*/

int scurry_leave(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *discord_msg) 
{
  ERROR_INTERACTION((player.scurry_id == 0), "You are not in a scurry yet!");

  ERROR_INTERACTION((scurry.war_flag == 1), "You cannot leave your scurry while at war! Ask the owner to retreat if you wish to leave.");

  ERROR_INTERACTION((player.scurry_id == player.user_id), "You can't leave your own scurry!");
  
  ERROR_INTERACTION((scurry.war_flag == 1), "You can't do this right now! Your scurry needs your help!");

  char* input = event->data->options->array[0].value;

  printf("%s \n", lowercase(input));

  ERROR_INTERACTION((strcmp(lowercase(input), "yes") != 0), "Please enter 'yes' to verify!");

  player.scurry_id = 0;

  discord_msg->content = format_str(SIZEOF_DESCRIPTION, "<@!%ld>, <@!%ld> has left your scurry!", 
      scurry.scurry_owner_id, player.user_id);

  return 0;
}

void leave_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg)
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  if (scurry_leave(client, event, msg) )
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

  update_player_row(event->member->user->id, player);
  scurry = (struct Scurry) { 0 };

  free(msg);
}