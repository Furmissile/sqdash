/*

  This file handles changing the color of an embed
    - Checks if hex code is valid
    - sanitizes by lowercasing so switch statement is always correct by design

*/

/* Checks user input for hex value accuracy */
char* is_color(struct discord *client, const struct discord_interaction *event, char* input)
{
  if (strlen(input) != 6) {
    return NULL;
  } 
  else {
  for (int i = 0; i < 6; i++)
    switch (input[i])
    {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        continue;
      default:
        error_message(client, event, "Invalid input. Please try again!");
        return NULL;
    }
  }

  player.color = (int)strtol(input, NULL, 16);
  player.acorns -= 2500;

  return input;
}

/* Listens for slash command interactions */
int color_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);

  char* input = event->data->options->array[0].value;

  ERROR_INTERACTION((player.acorns < 2500), "You need more acorns!");
  ERROR_INTERACTION( ( !is_color(client, event, lowercase(input)) ), "Invalid input. Please try again!");

  msg->embed->title = format_str(SIZEOF_TITLE, "Color successfully changed!");
  msg->embed->color = player.color;

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
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

  return 0;
}