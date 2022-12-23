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

struct Invite_Info {
  struct Message *discord_msg;
  // struct discord_channel *channel;
  struct discord_user *user;

  unsigned long owner_id;
  unsigned long channel_id;
};

enum INVITE_RES {
  DB_INVITE_OWNER, // scurry owner
  DB_INVITE_USER, // inquiring user
  DB_INVITE_MSG, // message id of invite
  DB_INVITE_CHANNEL, // channel slash command was sent in
  DB_INVITE_TIME // time slash command was sent
};

void create_invite_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg)
{
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
  free(msg->buttons);
  free(msg);

  PQclear(pending_invite);
}

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

// void invite_response(struct discord *client, struct discord_response *resp, const struct discord_channel *channel)
void invite_response(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  const struct discord_interaction *event = resp->keep;

  // get the scurry info for its name -- scurry must be present if it got this far
  PGresult* get_scurry = SQL_query(conn, "select * from public.scurry where owner_id = %ld", event->user->id);

  unsigned long user_id = strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_USER));

  if (event->data->custom_id[1] - 48 == 0)
    SQL_query(conn, "update public.player set scurry_id = %ld where user_id = %ld", event->user->id, user_id );
  
  // message that gets sent to channel of original interaction
  discord_create_message(client, strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_CHANNEL)),
    &(struct discord_create_message)
      {
        .content = format_str(SIZEOF_DESCRIPTION, "<@%ld> **Invite Returned!**", user_id),

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
                  "<@%ld> has %s your invite request to **%s**",
                  event->user->id, 
                  (event->data->custom_id[1] - 48 == 0) ? "accepted" : "denied",
                  PQgetvalue(get_scurry, 0, DB_SCURRY_NAME))
            }
          },

          .size = 1
        }
      },
    NULL);

  PQclear(get_scurry);

  discord_edit_message(client, 
    event->channel_id, 
    strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_MSG)), 
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
                user->username, 
                format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", user->id, user->avatar) ),
            .title = format_str(SIZEOF_TITLE, "User %s!", 
                (event->data->custom_id[1] - 48 == 0) ? "Accepted" : "Denied"),
            .description = format_str(SIZEOF_DESCRIPTION, 
                "<@%ld> You have %s **%s**!",
              event->user->id, (event->data->custom_id[1] - 48 == 0) ? "accepted" : "denied", user->username)
          }
        },

        .size = 1
      },

      .components = &(struct discord_components) { 0 }
    },
    NULL);
  
  SQL_query(conn, "delete from public.invites where request_id = %ld", 
      strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_MSG)) );
}

void invite_success(struct discord *client, struct discord_response *resp, const struct discord_message *message)
{
  struct Invite_Info *invite_info = resp->data;
  struct Message *discord_msg = invite_info->discord_msg;
  struct discord_embed *embed = discord_msg->embed;

  const struct discord_interaction *event = resp->keep;

  SQL_query(conn, "INSERT INTO public.invites VALUES(%ld, %ld, %ld, %ld, %ld)", 
      invite_info->owner_id, event->member->user->id, message->id, event->channel_id, time(NULL) + INVITE_CD);

  // author here has already been created by invite_interaction!
  embed->color = (int)ACTION_SUCCESS;
  embed->title = format_str(SIZEOF_TITLE, "Request Success!");
  embed->description = format_str(SIZEOF_DESCRIPTION, "Your request was successfully sent to <@!%ld>!", invite_info->owner_id);

  create_invite_interaction(client, event, invite_info->discord_msg);

  free(resp->data);
}

void send_invite_dm(struct discord *client, struct discord_response *resp, const struct discord_channel *channel)
{
  struct Invite_Info *invite_info = resp->data;

  const struct discord_interaction *event = resp->keep;

  invite_info->channel_id = channel->id;

  struct discord_ret_message ret_msg = {
    .done = &invite_success,
    .data = invite_info,
    .keep = event
  };

  // creates message at dm channel
  discord_create_message(client, channel->id,
    &(struct discord_create_message)
    {
      .content = format_str(SIZEOF_DESCRIPTION, "<@%ld> **Pending Request!**", invite_info->owner_id),

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
              "<@%ld> has requested to join your scurry!", event->member->user->id)
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
}

/* Called on scurry_invite command */
int invite_interaction(struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = (event->data->custom_id) ? load_player_struct(event->user->id) : load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  // delete EXPIRED invites
  SQL_query(conn, "delete from public.invites where t_stamp < %ld", time(NULL));

  // if a button was pressed, this is a response
  if (event->data->custom_id)
  {
    ERROR_INTERACTION((scurry.war_flag == 1), "You cannot accept invites while at war! Please retreat or finish the war first.");

    pending_invite = SQL_query(conn, "select * from public.invites where request_id = %ld", event->message->id);
  
    if (PQntuples(pending_invite) == 0) {
      invite_expired(client, event);
      return ERROR_STATUS;
    }

    struct discord_ret_user ret_user = {
      .done = &invite_response,
      .keep = event
    };

    discord_get_user(client, strtobigint(PQgetvalue(pending_invite, 0, DB_INVITE_USER)), &ret_user);

    return 0;
  }

  // check if player is already in a scurry
  ERROR_INTERACTION((player.scurry_id > 0), "You are already in a scurry! Try leaving your current scurry.");

  // check scurry status and existence
  PGresult* scurry_info = SQL_query(conn, "select * from public.scurry where s_name like '%s'", event->data->options->array[0].value);
  ERROR_DATABASE_RET((PQntuples(scurry_info) == 0), "Sorry, this scurry doesn't exist!", scurry_info);
  ERROR_DATABASE_RET((strtoint(PQgetvalue(scurry_info, 0, DB_WAR_FLAG)) == 1), 
      "This scurry is currently at war! Please wait until they are ready.", scurry_info);
  
  unsigned long owner_id = strtobigint(PQgetvalue(scurry_info, 0, DB_SCURRY_OWNER_ID));
  PQclear(scurry_info);

  // check scurry capacity
  scurry_info = SQL_query(conn, "select * from public.player where scurry_id = %ld", owner_id );  
  ERROR_DATABASE_RET((PQntuples(scurry_info) == SCURRY_MEMBER_MAX), "This scurry is full!", scurry_info);
  PQclear(scurry_info);

  // check if user has a pending invite
  scurry_info = SQL_query(conn, "select * from public.invites where player_id = %ld", event->member->user->id);
  ERROR_DATABASE_RET((PQntuples(scurry_info) > 0), "You already have a pending request!", scurry_info);

  // invite info is used to pass information along functions
  struct Invite_Info *invite_info = calloc(1, sizeof(struct Invite_Info));
  invite_info->discord_msg = msg;
  invite_info->owner_id = owner_id;
  PQclear(scurry_info);

  // get owner id for DM channel
  struct discord_create_dm dm_params = { .recipient_id = invite_info->owner_id };
  struct discord_ret_channel dm_ret = { 
    .done = &send_invite_dm,
    .data = invite_info,
    .keep = event
  };

  // Return channel context for DM channel id
  discord_create_dm(client, &dm_params, &dm_ret);

  return 0;
}