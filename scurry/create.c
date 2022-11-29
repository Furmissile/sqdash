/*

  This file handles scurry creation

*/

int scurry_create(
  struct discord *client,
  const struct discord_interaction *event, 
  struct Message *discord_msg)
{
  /* Check to make sure the player isnt in a scurry, the input is right, and the player has enough acorns */
  ERROR_INTERACTION((player.scurry_id != 0), 
      "You are already in a scurry! Try leaving your current scurry.");

  ERROR_INTERACTION((player.acorns < SCURRY_CREATION_COST), 
      "You need more acorns to create a scurry!");
  
  ERROR_INTERACTION((player.level < 20), 
      "You must be level 20 before you can create a scurry!");

  char* input = event->data->options->array[0].value;
  ERROR_INTERACTION((strlen(input) > SIZEOF_SCURRY_NAME), 
      "The name needs to be less than 16 characters.");
  
  PGresult* check_scurry = SQL_query("select * from public.scurry where s_name like '%s'", input);
  ERROR_DATABASE_RET(
      (PQntuples(check_scurry) > 0), 
      format_str(SIZEOF_DESCRIPTION, "Sorry, the name \"%s\" is already taken!", input), 
      check_scurry);
  PQclear(check_scurry);

  struct discord_embed *embed = discord_msg->embed;

  check_scurry = SQL_query("INSERT INTO public.scurry VALUES(%ld, '%s', 0, 0, 0)", 
      event->member->user->id, input);
  PQclear(check_scurry);

  //Only set the player guild id to the 
  check_scurry = SQL_query("select * from public.scurry where owner_id = %ld", event->member->user->id);
  ERROR_DATABASE_RET((PQntuples(check_scurry) == 0), "An error has occurred. Scurry could not be found!", check_scurry);

  if (PQntuples(check_scurry) > 0) {
    player.scurry_id = event->member->user->id; // the player is the owner
    player.acorns -= SCURRY_CREATION_COST;
  }

  embed->title = format_str(SIZEOF_TITLE, "Scurry successfully created!");
  embed->color = ACTION_SUCCESS;

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Your scurry is named **%s**. Check it out with the command `/scurry_info`!",
      PQgetvalue(check_scurry, 0, DB_SCURRY_NAME));
  
  PQclear(check_scurry);

  embed->footer = discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "/scurry_help | For info about scurries!"),
    fill_git_url(item_types[TYPE_NO_ACORNS].file_path) );

  return 0;
}

/* Listens for slash command interactions */
void create_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  if (scurry_create(client, event, msg) )
    return;

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

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

  update_player_row(event->member->user->id, player);
}