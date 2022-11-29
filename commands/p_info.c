/*

This file handles displaying player info (no buttons exist)

  - Gives brief description of player stats and guild affiliation
  - Info includes level, energy, balance, stats, and event participation
  - Another segment displays stats
  - Each field represents a biome with its corresponding material
  - Has buttons for biome swapping
*/

struct discord_components* build_biome_buttons(
  const struct discord_interaction *event,
  int button_size)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < button_size; i++)
  {
    struct File biome_icon = biomes[i].biome_icon;

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
    emoji->name = biome_icon.emoji_name;
    emoji->id = biome_icon.emoji_id;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_BIOME, i, event->member->user->id);

    buttons->array[i] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .label = biome_icon.formal_name,
      .custom_id = set_custom_id,
      .emoji = emoji,
    };

    if ( event->data->custom_id 
      && strcmp(event->data->custom_id, set_custom_id) == 0)
      player.biome = i;

    if (player.biome != i) {
      buttons->array[i].style = DISCORD_BUTTON_PRIMARY;
    } else {
      buttons->array[i].style = DISCORD_BUTTON_SECONDARY;
      buttons->array[i].disabled = true;
    }

  }

  return buttons;
}

char* load_e_acorn_status()
{
  char* buffer = calloc(SIZEOF_FIELD_VALUE, sizeof(char));

  int enchanted_acorn[5] = {
    player.buffs.acuity_acorn, 
    player.buffs.endurance_acorn, 
    player.buffs.luck_acorn, 
    player.buffs.proficiency_acorn, 
    player.buffs.smell_acorn
  };

  if (player.buffs.smell_acorn == 0
    && player.buffs.endurance_acorn == 0 
    && player.buffs.acuity_acorn == 0
    && player.buffs.luck_acorn == 0
    && player.buffs.proficiency_acorn == 0 )
  {
    ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION, "\n "OFF_ARROW" *No enchanted acorns are active!*");
    return buffer;
  }

  // Stats are put into an array according to the enchanted acorn enum for readability
  for (int i = 0; i < 5; i++)
  {
    if (enchanted_acorn[i] > 0)
      ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION,
          "\n <:%s:%ld> %s (**%d**)",
          enchanted_acorns[i].emoji_name, enchanted_acorns[i].emoji_id, enchanted_acorns[i].formal_name, enchanted_acorn[i] );
  }

  return buffer;
}

enum INFO_FORMAT {
  INFO_GENERAL,
  INFO_STATS,
  INFO_BUFFS,
  INFO_BIOMES,
  INFO_SIZE
};

/* Builds parts of the player info embed */
void player_info(
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  embed->title = format_str(SIZEOF_TITLE, "Player Info");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = INFO_SIZE;
  embed->fields->array = calloc(INFO_SIZE, sizeof(struct discord_embed_field));

  /* Load general stats */
  float req_xp = req_xp(player.level);
  float percent = (player.xp / req_xp) *100;

  embed->fields->array[INFO_GENERAL].name = format_str(SIZEOF_TITLE, "General Stats");
  embed->fields->array[INFO_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      " "INDENT" "ENERGY" Energy: **%d**/%d \n"
      " "INDENT" "STAHR" Level: **%d** **(**%0.1f%%**)** \n"
      " "INDENT" "ACORNS" Acorns: **%s** \n"
      " "INDENT" "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
      " "INDENT" "SEEDS" Seeds: **%s** \n"
      " "INDENT" "PINE_CONES" Pine Cones: **%s** \n",
      player.energy, MAX_ENERGY,
      player.level, percent, num_str(player.acorns), 
      num_str(player.golden_acorns), num_str(player.materials.seeds), num_str(player.materials.pine_cones) );
  
  if (player.scurry_id > 0)
    ADD_TO_BUFFER(embed->fields->array[INFO_GENERAL].value, SIZEOF_FIELD_VALUE,
        " "INDENT" "GUILD_ICON" Current Guild: %s \n",
        format_str(32, "**%s**", scurry.scurry_name) );

  /* Loads squirrel stats */
  char player_stat_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < player.max_biome +1; i++)
  {
    char* stahr_type = (*biomes[i].stat_ptr < BRONZE_BRACKET) ? BRONZE_STAHR
      : (*biomes[i].stat_ptr < SILVER_BRACKET) ? SILVER_STAHR : STAHR;

    float stat_value = generate_factor(biomes[i].stat_value_multiplier, *(biomes[i].stat_ptr));

    ADD_TO_BUFFER(player_stat_field, SIZEOF_FIELD_VALUE,
        (i == STAT_PROFICIENCY) ? ""OFF_ARROW" *%s* (Lv **%d** %s) x**%0.2f** \n" :
        (i == STAT_SMELL) ? ""OFF_ARROW" *%s* (Lv **%d** %s) x**%0.1f** \n"
            : ""OFF_ARROW" *%s* (Lv **%d** %s) +**%0.0f** \n",
        stat_files[i].formal_name, *biomes[i].stat_ptr, stahr_type, stat_value );
  }

  embed->fields->array[INFO_STATS].name = format_str(SIZEOF_TITLE, "Squirrel Stats");
  embed->fields->array[INFO_STATS].value = format_str(SIZEOF_FIELD_VALUE, player_stat_field);

  /* Loads active buffs */
  embed->fields->array[INFO_BUFFS].name = format_str(SIZEOF_TITLE, "Active Buffs");
  embed->fields->array[INFO_BUFFS].value = format_str(SIZEOF_FIELD_VALUE, load_e_acorn_status() );
  
  /* Loads biome materials */
  char player_biome_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < player.max_biome +1; i++) {
    struct File biome_icon = biomes[i].biome_icon;
    struct File biome_material = biomes[i].biome_material;

    ADD_TO_BUFFER(player_biome_field, SIZEOF_FIELD_VALUE,
        "<:%s:%ld> **%s** <:%s:%ld> %s \n",
        biome_icon.emoji_name, biome_icon.emoji_id, num_str(*biomes[i].material_ptr),
        biome_material.emoji_name, biome_material.emoji_id, biome_material.formal_name );
  }

  embed->fields->array[INFO_BIOMES].name = format_str(SIZEOF_TITLE, "Biome Materials");
  embed->fields->array[INFO_BIOMES].value = format_str(SIZEOF_FIELD_VALUE, player_biome_field);

  /* Loads buttons for biome swap */
  discord_msg->buttons = build_biome_buttons(event, player.max_biome +1);
}


/* Listens for slash command interactions */
int info_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  energy_regen();
  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  player_info(event, msg);

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = msg->buttons
  };

  struct discord_interaction_response interaction = 
  {
    .type = (event->data->custom_id) ? DISCORD_INTERACTION_UPDATE_MESSAGE : DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = msg->embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(msg->embed);
  free(msg->buttons);
  free(msg);

  update_player_row(event->member->user->id, player);
  // scurry doesnt need to be updated!
  scurry = (struct Scurry) { 0 };

  return 0;
}