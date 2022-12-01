/*

  This file builds the guild help embed for guild owners

  Topics to include are:
    * Scurry Utils
      - Invites, kicking, leaving
    * Scurry Wars 
      - Courage and ranks, Prereqs, Retreating
    * Scurry War Aftermath
      - Acorn Benefit, reseting of the Courage score

  * Send on scurry creation!

*/

enum S_TOPIC {
  S_TOPIC_UTILS,
  S_TOPIC_WARS,
  S_TOPIC_AFTERMATH,
  S_TOPIC_RANKS,
  S_TOPIC_SIZE
};

void s_help(struct Message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Scurry Help");

  embed->color = (int)strtol("F0C318", NULL, 16);

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help on what scurries are about? Check out the info below \n"
      "to see if they answer your questions!");
  
  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = S_TOPIC_SIZE;
  embed->fields->array = calloc(S_TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[S_TOPIC_UTILS].name = format_str(SIZEOF_TITLE, ""GUILD_ICON" Scurry Utils "GUILD_ICON"");
  embed->fields->array[S_TOPIC_UTILS].value = format_str(SIZEOF_FIELD_VALUE, 
      " "OFF_ARROW" Member invites come in through DM. Press a button to accept or decline. Plan accordingly because invites expire in **2** minutes! \n"
      " "OFF_ARROW" Only the owner "LEADER" can kick a member. Simply follow up `/scurry_kick` with the user mention to kick! \n"
      " "OFF_ARROW" Members can leave on their own prerogative too using `/scurry_leave`. \n"
      "**Please know that your stats related to the scurry will not carry over!**");
  
  embed->fields->array[S_TOPIC_WARS].name = format_str(SIZEOF_TITLE, ""GUILD_ICON" Scurry Wars "GUILD_ICON"");
  embed->fields->array[S_TOPIC_WARS].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Courage "COURAGE" is the main scurry war currency that is obtained from stealing other scurries' war acorns. \n"
      " "OFF_ARROW" Any scurry current in the arena has the chance to steal from your war stash! \n"
      " "OFF_ARROW" To join the arena (found in scurry info), scurries need their war stash "LOST_STASH" full and have at least 5 members present! \n");
  
  embed->fields->array[S_TOPIC_AFTERMATH].name = format_str(SIZEOF_TITLE, ""GUILD_ICON" Arena Aftermath "GUILD_ICON"");
  embed->fields->array[S_TOPIC_AFTERMATH].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Every scurry is dropped from the arena upon running out of war acorns. \n"
      " "OFF_ARROW" Every time you re-enter the arena, your courage is reset. \n"
      " "OFF_ARROW" Scurries have the option to retreat at any point, but be aware your rank will match your new courage score! \n"
      " "OFF_ARROW" Scurry ranks provide passive acorn boosts!");
    
  embed->fields->array[S_TOPIC_RANKS].name = format_str(SIZEOF_TITLE, ""GUILD_ICON" War Ranks "GUILD_ICON"");
  embed->fields->array[S_TOPIC_RANKS].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Courage determines scurry rank as follows: \n"
      INDENT BRONZE_STAHR "Seed-Nots (*Base*) \n"
      INDENT BRONZE_STAHR "Acorn Snatchers (> **%s** "COURAGE" Courage): x**1.05** \n"
      INDENT BRONZE_STAHR "Seed Sniffers (> **%s** "COURAGE" Courage) x**1.1** \n"
      INDENT SILVER_STAHR "Oakfficials (> **%s** "COURAGE" Courage) x**1.15** \n"
      INDENT STAHR "Royal Nuts (**%s**+ "COURAGE" Courage) x**1.2**",
      num_str(SEED_NOT_MAX), num_str(ACORN_SNATCHER_MAX), num_str(SEED_SNIFFER_MAX), num_str(OAKFFICIAL_MAX) );

  embed->footer = discord_set_embed_footer("Happy Foraging!",
      fill_git_url(items[ITEM_ACORNS].file_path) );
}

/* Listens for slash command interactions */
void scurry_help_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  s_help(msg);

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

  free(msg);
}