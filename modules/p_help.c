/*

  This file handles the help embed.

  - Displays general concepts of the game.

*/

enum P_TOPIC {
  TOPIC_INTRO,
  TOPIC_MATERIALS,
  TOPIC_PROGRESSION,
  TOPIC_CHANGE_COLOR,
  TOPIC_STEAL,
  TOPIC_SIZE
};

void p_help(struct Message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Player Help");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help? Check out the info below to see if they answer your questions! \n"
      "Need help with purchases? Request the embed and it will explain how it works!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = TOPIC_SIZE;
  embed->fields->array = calloc(TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[TOPIC_INTRO].name = format_str(SIZEOF_TITLE, ""ACORNS" Bot Interaction "ACORNS"");
  embed->fields->array[TOPIC_INTRO].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Enter `/forage` to get started with an adventure and press any button. \n"
      " "OFF_ARROW" What each command does will pop up just below the command after inputing `/`. \n"
      " "OFF_ARROW" Acorn "ACORNS" is the main currency.");

  embed->fields->array[TOPIC_MATERIALS].name = format_str(SIZEOF_TITLE, ""ACORNS" Materials vs Biome Materials "ACORNS"");
  embed->fields->array[TOPIC_MATERIALS].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Seeds "SEEDS" and pine cones "PINE_CONES" can be found *anywhere* whereas biome materials are unique to their biome. \n"
      " "OFF_ARROW" Seeds "SEEDS" are used for enchanted acorns and pine cones "PINE_CONES" are used for stat upgrades. \n"
      " "OFF_ARROW" Biome materials are used for various purchases.");

  embed->fields->array[TOPIC_PROGRESSION].name = format_str(SIZEOF_TITLE, ""ACORNS" Biome Progression "ACORNS"");
  embed->fields->array[TOPIC_PROGRESSION].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" You must reach a certain level "STAHR" to explore the next biome. \n"
      " "OFF_ARROW" The associated biome material will come up in your player info. Just send `/info` to check!");

  embed->fields->array[TOPIC_CHANGE_COLOR].name = format_str(SIZEOF_TITLE, ""ACORNS" Changing Message Color "ACORNS"");
  embed->fields->array[TOPIC_CHANGE_COLOR].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" The color of your embed can be changed by inputing a hex value into `/colorchange`. \n"
      " "OFF_ARROW" Looking for a hex color? Check out this [link](https://htmlcolorcodes.com/color-picker/) for reference!");

  embed->fields->array[TOPIC_STEAL].name = format_str(SIZEOF_TITLE, ""ACORNS" Stealing Player Acorns "ACORNS"");
  embed->fields->array[TOPIC_STEAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Stealing from other players gives acorns "ACORNS" and golden acorns "GOLDEN_ACORNS". \n"
      " "OFF_ARROW" Golden acorns "GOLDEN_ACORNS" can be spend on materials from the Seedy Market. Check it out using `/seedy_market`! \n");
  
  embed->footer = discord_set_embed_footer("Happy Foraging!",
      fill_git_url(items[ITEM_ACORNS].file_path) );
}


/* Listens for slash command interactions */
void help_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  p_help(msg);

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