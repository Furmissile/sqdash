

/*

  This file handles the event help embed.

    - Displays info regarding events

*/

// other event are to be added (including holiday events, etc.)
enum E_TOPIC {
  E_TOPIC_GOLDEN_ACORNS,
  E_TOPIC_SEASONS,
  E_TOPIC_SIZE
};

void e_help(struct Message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Event Help");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      "Looking for help on how events work? Check out the info below \n"
      "to see if they answer your questions!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = E_TOPIC_SIZE;
  embed->fields->array = calloc(E_TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[E_TOPIC_GOLDEN_ACORNS].name = format_str(SIZEOF_TITLE, ""GOLDEN_ACORNS" Golden Acorns "GOLDEN_ACORNS"");
  embed->fields->array[E_TOPIC_GOLDEN_ACORNS].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Golden acorn "GOLDEN_ACORNS" is also used as an event currency for all event-related purchases. \n");

  embed->fields->array[E_TOPIC_SEASONS].name = format_str(SIZEOF_TITLE, ""GOLDEN_ACORNS" Seasons "GOLDEN_ACORNS"");
  embed->fields->array[E_TOPIC_SEASONS].value = format_str(SIZEOF_DESCRIPTION,
      " "OFF_ARROW" Seasons change weekly and occur passively. Each season gives a different boost to acorn "ACORNS" gains. \n"
      " "OFF_ARROW" Seasons include Spring (+**20**%%), Summer (+**10**%%), Fall (+**5**%%), and Winter (No boost). \n"
      " "OFF_ARROW" Send `/season_info` to find out when the current season ends! \n");

  embed->footer = discord_set_embed_footer("Happy Foraging!",
      fill_git_url(items[ITEM_GOLDEN_ACORN].file_path) );
}


/* Listens for slash command interactions */
void event_help_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  e_help(msg);

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