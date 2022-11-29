/*

  This file handles events specific to the support server

*/

enum R_TOPIC {
  R_RESPECT,
  R_CONTENT,
  R_POLICY,
  R_ADVERTIZING,
  R_TOPIC_SIZE
};

void create_rules(struct discord *client, const struct discord_message *event) 
{
  struct discord_embed *embed = calloc(1, sizeof(struct discord_embed));

  struct discord_guild_member bot = { 0 };
  struct discord_ret_guild_member ret_bot = { .sync = &bot };

  discord_get_guild_member(client, GUILD_ID, APPLICATION_ID, &ret_bot);

  embed->author = discord_set_embed_author(
      format_str(SIZEOF_TITLE, bot.user->username),
      format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
          bot.user->id, bot.user->avatar) );

  embed->title = format_str(SIZEOF_TITLE, "Rules");

  embed->description = format_str(SIZEOF_DESCRIPTION, "Before we get started, lets lay dowm some rules first. \n");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = R_TOPIC_SIZE;
  embed->fields->array = calloc(R_TOPIC_SIZE, sizeof(struct discord_embed_field));

  embed->fields->array[R_RESPECT].name = format_str(SIZEOF_TITLE, ""ACORNS" Respect "ACORNS"");
  embed->fields->array[R_RESPECT].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Please treat others with respect and be mindful of the things you say. \n\
      "OFF_ARROW" Hate speech, violent or obscene comments, and otherwise inappropriate or invasive behavior not tolerated.");

  embed->fields->array[R_CONTENT].name = format_str(SIZEOF_TITLE, ""ACORNS" Content "ACORNS"");
  embed->fields->array[R_CONTENT].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" Be mindful of the content posted and be sure the right channels are used. \n\
      "OFF_ARROW" This means no harmful links, spamming, malware, or adult content (NSFW).");

  embed->fields->array[R_POLICY].name = format_str(SIZEOF_TITLE, ""ACORNS" Bot Policy "ACORNS"");
  embed->fields->array[R_POLICY].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" There is a license on the bot discussing use of content. Visit the repo and review the license \
        [here](https://github.com/Furmissile/sqdash/blob/main/LICENCE).");

  embed->fields->array[R_ADVERTIZING].name = format_str(SIZEOF_TITLE, ""ACORNS" Advertising "ACORNS"");
  embed->fields->array[R_ADVERTIZING].value = format_str(SIZEOF_FIELD_VALUE,
      " "OFF_ARROW" All advertising, unless approved of, is prohibited. Please ask before posting!");

  embed->thumbnail = discord_set_embed_thumbnail(fill_git_url(squirrels[GRAY_SQUIRREL].file_path) );

  embed->image = discord_set_embed_image(fill_git_url(welcome_msg->file_path) );

  embed->footer = discord_set_embed_footer("Happy Foraging!",
      fill_git_url(items[ITEM_ACORNS].file_path) );

  discord_create_message(client, event->channel_id,
  &(struct discord_create_message)
  {
    .embeds = &(struct discord_embeds)
    {
      .array = embed,
      .size = 1
    }
  },
  NULL);
}

void welcome_embed(struct discord *client, const struct discord_guild_member *event)
{
  if (event->guild_id != GUILD_ID) return;

  struct discord_embed *embed = calloc(1, sizeof(struct discord_embed));

  //Load Author
  embed->author = discord_set_embed_author(
      format_str(SIZEOF_TITLE, event->user->username),
      format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
          event->user->id, event->user->avatar) );

  embed->color = (int)strtol("00eeff", NULL, 16);

  embed->title = format_str(SIZEOF_TITLE, "Welcome to Squirrel Dash!");

  embed->thumbnail = discord_set_embed_thumbnail(fill_git_url(squirrels[GRAY_SQUIRREL].file_path) );

  embed->description = format_str(SIZEOF_DESCRIPTION,
      " "OFF_ARROW" Read the <#1046640388456321126> to <#1046813534790635550>! \n"
      " "OFF_ARROW" Begin your adventure in <#1046635264883294259>! \n"
      " "OFF_ARROW" Chat with fellow squirrel advocators in <#1046628380222685255> \n"
      " "OFF_ARROW" Looking for extra help? Ask away in <#1046634911198629989>!");

  embed->image = discord_set_embed_image( fill_git_url(welcome_msg->file_path) );

  embed->footer = discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "Happy foraging!"),
    fill_git_url(items[ITEM_ACORNS].file_path) );

  struct discord_create_message intro_message = (struct discord_create_message) {
    .content = format_str(SIZEOF_DESCRIPTION, "*<@!%ld> is looking for acorns...*", event->user->id),
    .embeds = &(struct discord_embeds) {
      .array = embed,
      .size = 1
    }
  };

  discord_create_message(client, WELCOME_CHANNEL_ID, &intro_message, NULL);
  free(embed);
}

void verify(struct discord *client, const struct discord_message_reaction_add *event)
{
  if (event->member->user->id == APPLICATION_ID || event->channel_id != VERIFY_CHANNEL_ID)
    return;

  // delete whatever emoji user reacted with
  discord_delete_user_reaction(client, event->channel_id, event->message_id, event->member->user->id, event->emoji->id, event->emoji->name, NULL);

  // only assign member role if they reacted with the acorn
  if (event->emoji->id != items[ITEM_ACORNS].emoji_id)
    return;

  discord_add_guild_member_role(client, GUILD_ID, event->member->user->id, 1046627142345170984, 
  &(struct discord_add_guild_member_role) {
    .reason = "New member"
  }, 
  NULL);
}

void create_verify(struct discord *client, const struct discord_message *event)
{
  if (event->author->id != OWNER_ID || event->channel_id != VERIFY_CHANNEL_ID)
    return;

  struct discord_message msg = { 0 };
  struct discord_ret_message ret_msg = { .sync = &msg };

  struct discord_embed *embed = calloc(1, sizeof(struct discord_embed));

  // Load author with bot
  struct discord_guild_member bot = { 0 };
  struct discord_ret_guild_member ret_bot = { .sync = &bot };

  discord_get_guild_member(client, GUILD_ID, APPLICATION_ID, &ret_bot);

  embed->author = discord_set_embed_author(
      format_str(SIZEOF_TITLE, bot.user->username),
      format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
          bot.user->id, bot.user->avatar) );

  // load details
  embed->title = format_str(SIZEOF_TITLE, "Please Verify");

  embed->thumbnail = discord_set_embed_thumbnail(fill_git_url(squirrels[GRAY_SQUIRREL].file_path) );

  embed->description = format_str(SIZEOF_DESCRIPTION, "By reacting to this message, you have read and agree to the server rules. \n\
      Please react with thr Acorn "ACORNS" emoji to be able to chat. No other emoji will work!");

  discord_create_message(client, event->channel_id,
    &(struct discord_create_message)
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = embed,
        .size = 1
      }
    },
    &ret_msg);

  discord_create_reaction(client, event->channel_id, msg.id, items[ITEM_ACORNS].emoji_id, items[ITEM_ACORNS].emoji_name, NULL);
}