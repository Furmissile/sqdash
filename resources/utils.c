/*

  This file handles behind-the-scenes functionalities

*/

char* format_str(int size, char* format, ...)
{
  char* buffer = calloc(size, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, size, format, args);

  va_end(args);

  return buffer;
}

/* Fills discord attachment URL */
char* fill_git_url(char* file_path) {
  char* url_buffer = calloc(SIZEOF_URL, sizeof(char));

  snprintf(url_buffer, SIZEOF_URL, 
      "https://raw.githubusercontent.com/Furmissile/sqdash/main/Assets/%s",
      file_path);
  
  return url_buffer;
}

PGresult* SQL_query(char* format, ...)
{
  char* buffer = calloc(SIZEOF_SQL_COMMAND, sizeof(char));

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, SIZEOF_SQL_COMMAND, format, args);

  va_end(args);

  PGresult* query = PQexec(conn, buffer);

  free(buffer);

  return query;
}

struct discord_embed_author* discord_set_embed_author(char* name, char* icon_url)
{
  struct discord_embed_author *author = calloc(1, sizeof(struct discord_embed_author));

  author->name = calloc(SIZEOF_TITLE, sizeof(char));
  author->icon_url = calloc(SIZEOF_URL, sizeof(char));

  author->name = name;
  author->icon_url = icon_url;

  return author;
}

struct discord_embed_thumbnail* discord_set_embed_thumbnail(char* thumbnail_url)
{
  struct discord_embed_thumbnail *thumbnail = calloc(1, sizeof(struct discord_embed_thumbnail) );

  thumbnail->url = calloc(SIZEOF_URL, sizeof(char));
  thumbnail->url = thumbnail_url;

  return thumbnail;
}

struct discord_embed_image* discord_set_embed_image(char* image_url)
{
  struct discord_embed_image *image = calloc(1, sizeof(struct discord_embed_image) );

  image->url = calloc(SIZEOF_URL, sizeof(char));
  image->url = image_url;

  return image;
}

struct discord_embed_footer* discord_set_embed_footer(char* text, char* icon_url)
{
  struct discord_embed_footer *footer = calloc(1, sizeof(struct discord_embed_footer) );

  footer->text = calloc(SIZEOF_URL, sizeof(char));
  footer->icon_url = calloc(SIZEOF_URL, sizeof(char));

  footer->text = text;
  footer->icon_url = icon_url;

  return footer;
}

/* Returns total factor based on stat level and if it's a multiplier or incrementer */
float generate_factor(float base_value, int stat_lv)
{
  if (base_value == ACORN_MULTIPLIER || base_value == XP_MULTIPLIER)
    return (base_value * (stat_lv -1)) + (base_value * (stat_lv/STAT_EVOLUTION)) +1;
  else
    return base_value * (stat_lv -1);
}

/* Returns a total price based on stat level, unit price, and multiplication factor */
int generate_price(int stat_lv, int unit_price, float mult_factor)
{
  return ((unit_price * mult_factor) * stat_lv) + (unit_price * (stat_lv/STAT_EVOLUTION));
}

int factor_biome(int player_lv)
{
  int player_biome = (player_lv -20)/10;

  return (player_lv < 20) ? 0 : (player_biome >= BIOME_SIZE -1) ? BIOME_SIZE -1 : player_biome;
}

void check_level(struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  if (player.xp >= req_xp(player.level))
  {
    player.xp -= req_xp(player.level);
    player.level++;

    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n"STAHR" You have reached level **%d**! \n", player.level);
    
    struct File biome_icon = biomes[player.biome +1].biome_icon;
    if (player.level > 20 && (player.level - 20) % 10 == 0)
      ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION, 
          "\nYou have reached the <:%s:%ld> **%s**! \nCheck out `/info` for more! \n",
          biome_icon.emoji_name, biome_icon.emoji_id, biome_icon.formal_name);
  }
}

/* Checks for matching ids to make sure the player that sent the embed is the one pressing a button */
void error_message(struct discord *client, const struct discord_interaction *event, char* message) {
  struct discord_interaction_response error_message = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = message
    }
  };

  discord_create_interaction_response(client, event->id, event->token, &error_message, NULL);
}

unsigned long strtobigint(const char* str)
{
  if (!str)
    return -1;

  for (size_t i = 0; i < strlen(str); i++)
    if (str[i] -48 < 0
      || str[i] -48 > 9)
      return -1;

  return strtol(str, NULL, 10);
}


char* trim_user_id(char* input)
{
  char* user_id = calloc(32, sizeof(char));

  char tmp_buffer[32] = {};
  if (!strstr(input, "<@") && input[strlen(input) -1] != '>')
    snprintf(tmp_buffer, sizeof(tmp_buffer), "<@%s>", input);
  else if (!strstr(input, "<@") || input[strlen(input) -1] != '>')
    return NULL;
  else 
    snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", input);

  int user_id_i = 2;
  while (tmp_buffer[user_id_i] != '>' && tmp_buffer[user_id_i])
  {
    user_id[user_id_i -2] = tmp_buffer[user_id_i];
    user_id_i++;
  }

  return user_id;
}

int strtoint(const char* str)
{
  int num = 0;
  size_t max_len = strlen(str);

  for (size_t i = 1; i < max_len +1; i++)
  {
    int base_multiplier = 1;
    for (size_t x = 1; x < i; x++)
      base_multiplier *= 10;

    num += (str[max_len - i] - 48) * base_multiplier;
  }

  return num;
}

char* num_str(long long num) 
{
  char tmp_buffer[32];
  snprintf(tmp_buffer, sizeof(tmp_buffer), "%lld", num);

  char* buffer = calloc(32, sizeof(char));

  // if num is less than 1000, return number
  if (strlen(tmp_buffer) < 4) {
    snprintf(buffer, 32, "%s", tmp_buffer);
    return buffer;
  }
  //define number offset by remainder of 3
  size_t first_set_n = (strlen(tmp_buffer) % 3 == 0) ? 3 : strlen(tmp_buffer) % 3;

  size_t buffer_offset = strlen(buffer);

  //apply offset and add comma
  for (size_t tmp_offset = 0; tmp_offset < first_set_n; tmp_offset++)
    buffer[buffer_offset++] = tmp_buffer[tmp_offset];
  buffer[buffer_offset++] = ',';

  //while there are still characters, add 3 numbers and a comma
  for (size_t tmp_offset = 0; tmp_offset < strlen(tmp_buffer) -first_set_n; tmp_offset++) {
    buffer[buffer_offset++] = tmp_buffer[tmp_offset + first_set_n];

    //do not apply comma if at end of string
    if (((tmp_offset +1) % 3 == 0) && (tmp_offset + first_set_n != strlen(tmp_buffer) -1))
      buffer[buffer_offset++] = ',';
  }

  return buffer;
}

char* lowercase(const char* str)
{
  char* buffer = calloc(strlen(str) +1, sizeof(char));

  for (size_t i = 0; i < strlen(str); i++)
    buffer[i] = (str[i] < 91 && str[i] > 64) ? str[i] + 32 : str[i];

  return buffer;
}

int retrieve_discord_user(struct discord *client, const struct discord_interaction *event, struct discord_user *target_user)
{
  struct discord_ret_user ret_user = { .sync = target_user };

  unsigned long target_id = (event->data->options) ? strtobigint(trim_user_id(event->data->options->array[0].value))
    : event->member->user->id;

  printf("%ld \n", target_id);
  ERROR_INTERACTION(
    (target_id == (unsigned long)ERROR_STATUS || discord_get_user(client, target_id, &ret_user) != CCORD_OK), 
    "This is an invalid user!" );
  
  PGresult* locate_player = SQL_query("select user_id from public.player where user_id = %ld", target_id);
  ERROR_DATABASE_RET((PQntuples(locate_player) == 0), "This player cannot be found!", locate_player);
  PQclear(locate_player);

  return 0;
}

void energy_status(struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;

  if (rand() % MAX_CHANCE < 80)
  {
    // if (player.user_id != OWNER_ID)
      player.energy -= 2;

    embed->footer = discord_set_embed_footer(
        format_str(SIZEOF_FOOTER_TEXT, "You have %d energy left!", player.energy),
        fill_git_url(items[ITEM_ENERGY].file_path));
    
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n-**2** "ENERGY" Energy \n");
  }
  else {
    ADD_TO_BUFFER(embed->description, SIZEOF_DESCRIPTION,
        "\n"ENERGY" No energy was lost! \n");
  }
}

void energy_regen() 
{
  if (player.buffs.endurance_acorn > 0)
  {
    player.energy += player.buffs.endurance_acorn;
    player.buffs.endurance_acorn = 0;
  }

  int add_energy = (player.energy < MAX_ENERGY) ? (time(NULL) - player.main_cd)/ BASE_ENERGY_CD : 0;

  if (player.energy + add_energy > MAX_ENERGY)
    player.energy += (player.energy > MAX_ENERGY) ? 0 : add_energy - (add_energy - (MAX_ENERGY - player.energy));
  else
    player.energy += add_energy;

  // wont disturb cooldown when info embed is sent
  player.main_cd = time(NULL) - ((time(NULL) - player.main_cd) % BASE_ENERGY_CD);
}
