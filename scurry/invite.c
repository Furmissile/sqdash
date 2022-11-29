/*

  This file handles verifying a player to join a scurry
  1. User creates slash command w. mentioned owner
    * Create interaction response to user w. success status
    * Create message in owner DM w. buttons attached
    * Obtain message context to store into db

  2. Owner respond w. button press
    * Edit DM message with update status
    * Create message in channel of slash command w. reply results

*/

PGresult* pending_invite;

enum INVITE_RES {
  DB_INVITE_OWNER,
  DB_INVITE_USER,
  DB_INVITE_MSG,
  DB_INVITE_CHANNEL,
  DB_INVITE_TIME
};

struct discord_component* build_button_container(const struct discord_interaction *event)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));
  buttons->array = calloc(2, sizeof(struct discord_component));
  buttons->size = 2;

  buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = "Accept",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c0_%ld", TYPE_INVITE, event->member->user->id),
  };
  buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_DANGER,
    .label = "Decline",
    .custom_id = format_str(SIZEOF_CUSTOM_ID,
        "%c1_%ld", TYPE_INVITE, event->member->user->id),
  };

  struct discord_component *action_rows = calloc(1, sizeof(struct discord_component));

  action_rows->type = DISCORD_COMPONENT_ACTION_ROW;
  action_rows->components = buttons;

  return action_rows;
}

void on_slash(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *discord_msg,
  unsigned long owner_id)
{
  struct discord_embed *embed = discord_msg->embed;

  //event user is the inquiring user -- FOR DM
  struct discord_message msg = { 0 };
  struct discord_ret_message ret_msg = { .sync = &msg };

  discord_create_message(client, discord_msg->channel->id,
    &(struct discord_create_message)
    {
      .content = format_str(SIZEOF_DESCRIPTION, "<@!%ld> **Pending Request!**", owner_id),

      .embeds = &(struct discord_embeds)
      {
        .array = (struct discord_embed[])
        {
          {
          .color = (int)ACTION_SUCCESS,
          .author = discord_set_embed_author(
              format_str(SIZEOF_TITLE, event->member->user->username),
              format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
                  event->member->user->id, event->member->user->avatar) ),
          .title = format_str(SIZEOF_TITLE, "User Invite"),
          .description = format_str(SIZEOF_DESCRIPTION,
              "<@!%ld> has requested to join your scurry!", event->member->user->id)
          }
        },
        .size = 1
      },

      .components = &(struct discord_components)
      {
        .array = build_button_container(event),
        .size = 1
      }
    }, 
    &ret_msg);



  SQL_query("INSERT INTO public.invites VALUES(%ld, %ld, %ld, %ld, %ld)", 
      owner_id, event->member->user->id, msg.id, event->channel_id, time(NULL) + INVITE_CD);

  // author here has already been created by invite_interaction!
  embed->color = (int)ACTION_SUCCESS;
  embed->title = format_str(SIZEOF_TITLE, "Request Success!");
  embed->description = format_str(SIZEOF_DESCRIPTION, "Your request was successfully sent to <@!%ld>!", owner_id);
}

void on_button_press(
  struct discord *client, 
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  // get the scurry info for its name -- scurry must be present if it got this far
  PGresult* get_scurry = SQL_query("select * from public.scurry where owner_id = %ld", event->user->id);

  unsigned long user_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_USER));

  struct discord_user user = { 0 };
  struct discord_ret_user ret_user = { .sync = &user };

  discord_get_user(client, user_id, &ret_user);

  if (event->data->custom_id[1] - 48 == 0)
    SQL_query("update public.player set scurry_id = %ld where user_id = %ld", event->user->id, user_id);
  
  discord_create_message(client, strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_CHANNEL)),
    &(struct discord_create_message)
      {
        .content = format_str(SIZEOF_DESCRIPTION, "<@!%ld> **Invite Returned!**", user_id),

        .embeds = &(struct discord_embeds)
        {
          .array = (struct discord_embed[])
          {
            {
              .color = (event->data->custom_id[1] - 48 == 0) ? (int)ACTION_SUCCESS : (int)ACTION_FAILED,
              .author = discord_set_embed_author(
                event->user->username,
                format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", event->user->id, event->user->avatar) ),
              .title = format_str(SIZEOF_TITLE,
                  (event->data->custom_id[1] - 48 == 0) ? "Request Accepted!" : "Request Denied!"),
              .description = format_str(SIZEOF_DESCRIPTION,
                "<@!%ld> has %s your invite request to **%s**",
                event->user->id, (event->data->custom_id[1] - 48 == 0) ? "accepted" : "denied",
                PQgetvalue(get_scurry, 0, DB_SCURRY_NAME))
            }
          },

          .size = 1
        }
      },
    NULL);

  discord_edit_message(client, discord_msg->channel->id, strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_MSG)), 
    &(struct discord_edit_message)
    {
      .content = format_str(SIZEOF_DESCRIPTION, "**Invite Read**"),
      .embeds = &(struct discord_embeds)
      {
        .array = (struct discord_embed[])
        {
          {
            .color = (event->data->custom_id[1] - 48 == 0) ? (int)ACTION_SUCCESS : (int)ACTION_FAILED,
            .author = discord_set_embed_author(
                user.username, 
                format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", user.id, user.avatar) ),
            .title = format_str(SIZEOF_TITLE, "User %s!", 
                (event->data->custom_id[1] - 48 == 0) ? "Accepted" : "Denied"),
            .description = format_str(SIZEOF_DESCRIPTION, 
                "<@!%ld> You have %s **%s**!",
              event->user->id, (event->data->custom_id[1] - 48 == 0) ? "accepted" : "denied", user.username)
          }
        },

        .size = 1
      },

      .components = &(struct discord_components) { 0 }
    },
    NULL);

  PQclear(get_scurry);
}

void invite_expired(struct discord *client, const struct discord_interaction *event)
{
  char* invite_user = event->message->embeds->array[0].author->name;

  //edit message
  discord_edit_message(client, event->channel_id, event->message->id, 
    &(struct discord_edit_message)
    {
      .content = format_str(SIZEOF_DESCRIPTION, "**Invite Read**"),
      .embeds = &(struct discord_embeds)
      {
        .array = (struct discord_embed[])
        {
          {
            .color = (int)ACTION_UNDEFINED,
            // .author = discord_set_embed_author(username, avatar),
            .author = discord_set_embed_author(
                event->user->username, 
                format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", event->user->id, event->user->avatar) ),
            .title = format_str(SIZEOF_TITLE, "Invite Expired!"),
            .description = format_str(SIZEOF_DESCRIPTION, 
                "<@!%ld> **%s**'s invite has expired!", event->user->id, invite_user)
          }
        },

        .size = 1
      },

      .components = &(struct discord_components) { 0 }
    },
    NULL);
}

int scurry_invite(
  struct discord *client,
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  //DM context is needed regardless of the interaction
  discord_msg->channel = calloc(1, sizeof(struct discord_channel));
  struct discord_ret_channel dm_ret = { .sync = discord_msg->channel };

  struct discord_create_dm dm_params = { 0 };

  // delete EXPIRED invites
  SQL_query("delete from public.invites where t_stamp < %ld", time(NULL));

  if (event->data->custom_id)
  {
    ERROR_INTERACTION((scurry.war_flag == 1), "You cannot accept invites while at war! Please retreat or finish the war first.");

    pending_invite = SQL_query("select * from public.invites where request_id = %ld", event->message->id);
  
    if ((unsigned long)time(NULL) > (event->message->timestamp/1000) + INVITE_CD
      || PQntuples(pending_invite) == 0)
    {
      invite_expired(client, event);
      return 1;
    }

    dm_params.recipient_id = event->user->id;

    // Return channel context for DM channel id
    discord_create_dm(client, &dm_params, &dm_ret);

    on_button_press(client, event, discord_msg);

    return 0;
  }

  char* input = event->data->options->array[0].value;

  ERROR_INTERACTION((player.scurry_id > 0), "You are already in a scurry!");

  PGresult* scurry_info = SQL_query("select * from public.scurry where s_name like '%s'", input);

  // check if scurry exists
  ERROR_DATABASE_RET((PQntuples(scurry_info) == 0), "Sorry, this scurry doesn't exist!", scurry_info);

  // check if scurry is at war
  ERROR_DATABASE_RET((strtoint(PQgetvalue(scurry_info, 0, DB_WAR_FLAG)) == 1), 
      "This scurry is currently at war! Please wait until they are ready.", scurry_info);

  // assume scurry exists, therefore take owner id
  unsigned long owner_id = strtobigint(PQgetvalue(scurry_info, 0, DB_SCURRY_OWNER_ID));

  PQclear(scurry_info);

  // use owner id to locate all members
  scurry_info = SQL_query("select * from public.player where scurry_id = %ld", owner_id);  

  // check if scurry is full
  ERROR_DATABASE_RET((PQntuples(scurry_info) == SCURRY_MEMBER_MAX), "This scurry is full!", scurry_info);
  PQclear(scurry_info);

  // check if user has a pending invite
  scurry_info = SQL_query("select * from public.invites where player_id = %ld", event->member->user->id);

  ERROR_DATABASE_RET((PQntuples(scurry_info) > 0), "You already have a pending request!", scurry_info);
  PQclear(scurry_info);

  // get user id for DM channel and search for the scurry to validate input
  dm_params.recipient_id = owner_id;

  // Return channel context for DM channel id
  discord_create_dm(client, &dm_params, &dm_ret);

  on_slash(client, event, discord_msg, owner_id);

  return 0;
}

/* Called on scurry_invite command */
void invite_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  // currently only loads to check player stats
  player = (event->data->custom_id) ? load_player_struct(event->user->id) : load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  // if an error is returned, an interaction was already generated
  // if a button is pressed, no interaction is needed
  if ( scurry_invite(client, event, msg) || event->data->custom_id ) 
  {
    discord_embed_cleanup(msg->embed);
    discord_channel_cleanup(msg->channel);
    free(msg);

    return;
  }

  // Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  struct discord_interaction_response interaction = 
  {
    // function will always generate a new message
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
  discord_channel_cleanup(msg->channel);
  free(msg->buttons);
  free(msg);

  PQclear(pending_invite);

  player = (struct Player) { 0 };
  scurry = (struct Scurry) { 0 };
}
