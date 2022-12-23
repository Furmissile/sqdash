PGresult* player_pos;

struct DB_Info {
  int db_idx;
  char* username;
  unsigned long user_id;
  int value;
};

struct UserData {
  struct Message *discord_msg;
  int db_rows;

  // how many requests have been completed
  int response_counter;

  // set data in an array to prevent disorganization
  struct DB_Info *row_data;

  int is_top_ten;
};

void create_leaderboard_interaction(struct discord *client, const struct discord_interaction *event, struct UserData *user_data) 
{
  struct Message *discord_msg = user_data->discord_msg;
  struct discord_embed *embed = discord_msg->embed;

  for (int idx = 0; idx < user_data->db_rows; idx++)
  {
    struct DB_Info info = user_data->row_data[idx];
    if (player.user_id == info.user_id)
    {
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, 
        "**%d**. <@%ld> **%s** \n",
        info.db_idx, info.user_id, num_str(info.value) );

      user_data->is_top_ten = 1;
    }
    else
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, 
        "**%d**. `%s` **%s** \n",
        info.db_idx, info.username, num_str(info.value) );
  }

  PQclear(player_pos);
  if (user_data->is_top_ten == 0) 
  {
    // player position in leaderboard
    player_pos = SQL_query(conn, "select row_idx, user_id, acorn_count \
        from (select dense_rank() over (order by acorn_count desc) as row_idx, user_id, acorn_count from public.player) \
        as lb where lb.user_id = %ld",
        player.user_id);

    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
      "\n**%d**. <@%ld> **%s** \n",
      strtoint(PQgetvalue(player_pos, 0, 0)),
      strtobigint(PQgetvalue(player_pos, 0, 1)), 
      num_str(strtoint(PQgetvalue(player_pos, 0, 2))) );
    PQclear(player_pos);
  }

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, 
    &(struct discord_edit_original_interaction_response)
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = embed,
        .size = 1
      }

    }, 
    NULL);

  scurry = (struct Scurry) { 0 };
  player = (struct Player) { 0 };

  discord_embed_cleanup(embed);
  free(discord_msg);
}

void is_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  struct UserData *user_data = resp->data;
  const struct discord_interaction *event = resp->keep;

  int idx = 0;
  // wait to match the id
  while (user_data->row_data[idx].user_id != user->id)
  {
    idx++;
    continue;
  }

  user_data->row_data[idx].username = format_str(SIZEOF_TITLE, user->username);

  user_data->response_counter++;

  if (user_data->response_counter == user_data->db_rows)
  {
    create_leaderboard_interaction(client, event, user_data);
    free(resp->data);
  }

}

void level_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct Message *discord_msg = resp->data;

  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Acorn Leaderboard");

  embed->color = strtol("6d198e", NULL, 10);
  embed->thumbnail = discord_set_embed_thumbnail( fill_git_url(items[ITEM_ACORN_COUNT].file_path) );

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));

  struct UserData *user_data = calloc(1, sizeof(struct UserData));
  user_data->discord_msg = calloc(1, sizeof(struct Message));

  user_data->db_rows = PQntuples(player_pos);
  user_data->row_data = calloc(user_data->db_rows, sizeof(struct DB_Info));

  user_data->discord_msg = discord_msg;

  for (int idx = 0; idx < user_data->db_rows; idx++)
  {
    user_data->row_data[idx].db_idx = strtoint(PQgetvalue(player_pos, idx, 0));
    user_data->row_data[idx].user_id = strtobigint(PQgetvalue(player_pos, idx, 1));
    user_data->row_data[idx].value = strtoint(PQgetvalue(player_pos, idx, 2));
  
    struct discord_ret_user ret_user = {
      .done = &is_user,
      .data = user_data,
      .keep = event
    };

    discord_get_user(client, user_data->row_data[idx].user_id, &ret_user);
  }

}


void courage_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  struct Message *discord_msg = resp->data;
  const struct discord_interaction *event = resp->keep;

  struct discord_embed *embed = discord_msg->embed;

  embed->title = format_str(SIZEOF_TITLE, "Courage Leaderboard");

  embed->color = strtol("1effff", NULL, 10);
  embed->thumbnail = discord_set_embed_thumbnail( fill_git_url(items[ITEM_COURAGE].file_path) );

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));

  int db_rows = PQntuples(player_pos);

  for (int idx = 0; idx < db_rows; idx++)
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
      (player.scurry_id == strtobigint(PQgetvalue(player_pos, idx, 1))) 
          ? "%d. "GUILD_ICON" %s **%s** \n" : "%d. `%s` **%s** \n",
      strtoint(PQgetvalue(player_pos, idx, 0)), 
      PQgetvalue(player_pos, idx, 2), 
      num_str(strtoint(PQgetvalue(player_pos, idx, 3))) );

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, 
    &(struct discord_edit_original_interaction_response)
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = embed,
        .size = 1
      }

    }, 
    NULL);

  scurry = (struct Scurry) { 0 };
  player = (struct Player) { 0 };

  discord_embed_cleanup(embed);
  free(discord_msg);
}

int get_leaderboard(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg)
{
  ERROR_INTERACTION((!event->data->options), "Please select the type of leaderboard you want!");

  char* command_type = event->data->options->array[0].value;

  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  // msg gets passed along regardless of leaderboard type
  struct discord_ret_interaction_response ret_response = { .data = msg, .keep = event };

  if (strcmp(command_type, "acorn_count") == 0)
  {
    ERROR_INTERACTION((player.acorn_count == 0), "You must be have an acorn count to view this leaderboard!");

    player_pos = SQL_query(conn, "select dense_rank() over (order by acorn_count desc) as rank_idx, user_id, acorn_count \
        from (select dense_rank() over (order by acorn_count desc) as rank_idx, user_id, acorn_count from public.player) as lb \
        where user_id != %ld and rank_idx <= 10 and acorn_count > 0", 
        OWNER_ID);

    ERROR_DATABASE_RET((PQntuples(player_pos) == 0), "There aren't enough entries yet!", player_pos);

    ret_response.done = &level_leaderboard;
  }
  else if (strcmp(command_type, "courage") == 0)
  {
    ERROR_INTERACTION((player.scurry_id == 0), "You must be in a scurry to view this leaderboard!");
    ERROR_INTERACTION((scurry.courage == 0), "Your scurry must participate in a war to view this leaderboard!");

    player_pos = SQL_query(conn, "select dense_rank() over (order by courage desc) as rank_idx, owner_id, s_name, courage \
        from (select dense_rank() over (order by courage desc) as rank_idx, owner_id, s_name, courage from public.scurry) as lb \
        where lb.owner_id != %ld and rank_idx <= 10 and courage > 0", 
        OWNER_ID);

    ERROR_DATABASE_RET((PQntuples(player_pos) == 0), "There aren't enough entries yet!", player_pos);

    ret_response.done = &courage_leaderboard;
  }
  else {
    error_message(client, event, "Sorry, this isn't a valid choice!");
    return ERROR_STATUS;
  }

  discord_create_interaction_response(client, event->id, event->token, 
    &(struct discord_interaction_response)
    {
      .type = DISCORD_INTERACTION_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .content = format_str(SIZEOF_DESCRIPTION, "Loading leaderboard...")
      }
    },
    &ret_response);

  return 0;
}
