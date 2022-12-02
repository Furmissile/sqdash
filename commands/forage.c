/*

This header handles the main embed and interactions associated with it

• Main/Encounter/Reward
   - MAIN displays title, image, and dirt piles
   - ENCOUNTER displays thumbnail, conflict, name, and options
   - Reward displays an updated description of what the player got

*/

void factor_stats(void)
{
  /* Multipliers do not need biome condition as they are just multiplied by 1 */

  //factor proficiency stat into XP
  rewards.xp *= generate_factor(XP_MULTIPLIER, player.stats.proficiency_lv);

  //factor smell stat into acorns
  rewards.acorns *= generate_factor(ACORN_MULTIPLIER, player.stats.smell_lv);

  /* Only materials need biome condition as stat increments reward by default */
  if (rewards.pine_cones && player.max_biome > GRASSLANDS)
    rewards.pine_cones += generate_factor(PINE_CONE_INC, player.stats.dexterity_lv);

  if (rewards.seeds && player.max_biome > SEEPING_SANDS)
    rewards.seeds += generate_factor(SEEDS_INC, player.stats.acuity_lv);

  if (rewards.biome_material && player.max_biome > DEATH_GRIP)
    rewards.biome_material += generate_factor(BIOME_MATERIAL_INC, player.stats.luck_lv);
}

void factor_buff(void)
{
  /* Acorns and XP doesnt need to check if !0 because anything * 0 is still 0! */
  if (player.buffs.smell_acorn > 0) {
    rewards.acorns *= 1.5;
    player.buffs.smell_acorn--;
  }

  if (player.buffs.proficiency_acorn > 0) {
    rewards.xp *= 1.5;
    player.buffs.proficiency_acorn--;
  }

  if (rewards.biome_material && player.buffs.luck_acorn > 0) {
    rewards.biome_material += 2;
    player.buffs.luck_acorn--;
  }

  if (rewards.pine_cones && rewards.seeds && player.buffs.acuity_acorn > 0) {
    rewards.pine_cones += 2;
    rewards.seeds += 2;

    player.buffs.acuity_acorn--;
  }

}

/* Handle rewards based on item_type for main embed*/
void get_rewards(int item_type, char msg_id) 
{
  switch (item_type) {
    case TYPE_ACORN_HANDFUL:
      rewards = (struct Rewards) {
        .xp = genrand(25, 25), 
        .acorns = genrand(15, 5)
      };
      break;
    case TYPE_ACORN_MOUTHFUL:
      rewards = (struct Rewards) {
        .xp = genrand(50, 25), 
        .acorns = genrand(25, 15)
      };
      break;
    case TYPE_LOST_STASH:
      rewards = (struct Rewards) {
        .xp = genrand(75, 25), 
        .acorns = genrand(50, 25), 
        .seeds = genrand(1, 3),
        .pine_cones = genrand(1, 3),
        .biome_material = (rand() % MAX_CHANCE < MAX_MATERIAL_CHANCE - (player.biome * 10) ) ? 1 : 0
      };
      break;
    case TYPE_ACORN_SACK:
      rewards = (struct Rewards) {
        .xp = genrand(100, 50), 
        .acorns = genrand(75, 25)
      };
  }

  if (scurry.war_flag == 1 && (rand() % MAX_CHANCE) > 65)
    factor_war(item_type);
  else if (scurry.war_flag == 0)
    scurry.war_acorns = (scurry.war_acorns + rewards.acorns >= scurry.war_acorn_cap) 
        ? scurry.war_acorn_cap : scurry.war_acorns + rewards.acorns;

  //if encounter
  if (msg_id == TYPE_ENCOUNTER_MSG) {
    rewards.xp *= 2;
    rewards.acorns *= 2;
  }

  factor_stats();
  if (msg_id != TYPE_ENCOUNTER_MSG)
    factor_season(item_type);
  factor_buff();

  // passive buff
  rewards.acorns *= (scurry.rank > SEED_NOT) ? ((BASE_COURAGE_MULT * (scurry.rank +1)) +1) : 1;

  player.xp += rewards.xp;
  player.acorns += rewards.acorns;
  player.materials.seeds += rewards.seeds;
  player.materials.pine_cones += rewards.pine_cones;
  *biomes[player.biome].material_ptr += rewards.biome_material;
}

/* Load rewards onto main embed */
void generate_rewards(
  const struct discord_interaction *event, 
  struct Message *discord_msg, 
  int item_type)
{
  struct discord_embed *embed = discord_msg->embed;
  rewards = (struct Rewards) { 0 };

  char msg_id = event->data->custom_id[0];
  get_rewards(item_type, msg_id);

  char* buffer = embed->description;

  switch (item_type) {
    case TYPE_ACORN_HANDFUL: case TYPE_ACORN_MOUTHFUL: case TYPE_ACORN_SACK:
      ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION,
          "+**%d** "XP" XP \n"
          "+**%d** "ACORNS" Acorns \n", 
          rewards.xp,rewards.acorns);
      break;
    case TYPE_LOST_STASH:
      ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION,
          "+**%d** "XP" XP \n"
          "+**%d** "ACORNS" Acorns \n"
          "+**%d** "SEEDS" Seeds \n"
          "+**%d** "PINE_CONES" Pine Cones \n",
          rewards.xp, rewards.acorns, rewards.seeds, rewards.pine_cones);

      if (rewards.biome_material)
      {
        struct File biome_material_file = biomes[player.biome].biome_material;
        ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION,
            "\n+**%d** <:%s:%ld> %s \n", 
            rewards.biome_material,
            biome_material_file.emoji_name, biome_material_file.emoji_id, biome_material_file.formal_name);
      }
      break;
    default:
      ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION, "You received no earnings! \n");
  }

  if (rewards.stolen_acorns)
    ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION, "\nYou successfully stole **%d** "ACORNS" Acorns! \n+**%d** "COURAGE" Courage\n", 
        rewards.stolen_acorns, rewards.courage);
    
  if (rewards.golden_acorns)
    ADD_TO_BUFFER(buffer, SIZEOF_DESCRIPTION, "\n+**%d** "GOLDEN_ACORNS" Golden Acorns \n", rewards.golden_acorns);

  energy_status(discord_msg);
  check_level(discord_msg);
}


/* Build components after button press */
struct discord_components* main_button_response(
  const struct discord_interaction *event, 
  int *item_type, 
  int button_size, 
  int msg_type) 
{
  struct discord_components* buttons = calloc(1, sizeof(struct discord_components));
  struct Encounter encounter = biomes[player.biome].encounters[player.select_encounter];

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++) {
    int chance = rand() % MAX_CHANCE;

    int set_item_type = 
        (chance < JUNK_CHANCE && msg_type != TYPE_ENCOUNTER_MSG) ? TYPE_NO_ACORNS
      : (chance < COMMON_CHANCE) ? TYPE_ACORN_HANDFUL
      : (chance < UNCOMMON_CHANCE) ? TYPE_ACORN_MOUTHFUL
      : (chance < CONTAINER_CHANCE && msg_type != TYPE_ENCOUNTER_MSG) ? TYPE_LOST_STASH : TYPE_ACORN_SACK;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", msg_type, i, event->member->user->id);

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
    emoji->name = item_types[set_item_type].file_path;
    emoji->id = item_types[set_item_type].emoji_id;

    buttons->array[i] = (struct discord_component) {
      .type = DISCORD_COMPONENT_BUTTON,
      .emoji = emoji,
      .custom_id = set_custom_id,
      .disabled = true,
      .label = (msg_type == TYPE_ENCOUNTER_MSG) ? encounter.solution[i] : NULL
    };

    if (strcmp(event->data->custom_id, set_custom_id) == 0) {
      *item_type = set_item_type;
      buttons->array[i].style = DISCORD_BUTTON_PRIMARY;
    } 
    else {
      buttons->array[i].style = DISCORD_BUTTON_SECONDARY;
    }
  }

  return buttons;
}

/* Build components on embed startup */
struct discord_components* build_buttons(
  const struct discord_interaction *event,
  int button_size, 
  int item_type, 
  int msg_type) 
{
  struct discord_components* buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
  emoji->name = item_types[item_type].emoji_name;
  emoji->id = item_types[item_type].emoji_id;

  struct Encounter encounter = biomes[player.biome].encounters[player.select_encounter];

  for (int i = 0; i < buttons->size; i++) {
    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, 32, "%c%d_%ld", msg_type, i, event->member->user->id);

    buttons->array[i] = (struct discord_component) {
      .type = DISCORD_COMPONENT_BUTTON,
      .style = DISCORD_BUTTON_PRIMARY,
      .custom_id = set_custom_id,
      .emoji = emoji,
      .label = (event->data->custom_id) ? encounter.solution[i] : NULL
    };
  }

  return buttons;
}

/* If an encounter was found, generate embed or response depending on response_flag */
void encounter_embed(
  const struct discord_interaction *event, 
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  struct Encounter encounter = biomes[player.biome].encounters[player.select_encounter];

  embed->title = format_str(SIZEOF_TITLE, encounter.name);

  embed->description = format_str(SIZEOF_DESCRIPTION, "*%s*\n\n", encounter.conflict);

  embed->thumbnail = discord_set_embed_thumbnail( fill_git_url(encounter.file_path) );

  if (event->data->custom_id 
    && event->data->custom_id[0] == TYPE_ENCOUNTER_MSG) 
  { //this is an encounter response
    int item_type = 0;
    discord_msg->buttons = main_button_response(event, &item_type, 3, TYPE_ENCOUNTER_MSG);

    generate_rewards(event, discord_msg, item_type);
    player.main_cd = time(NULL) + COOLDOWN;
  }
  else {
    discord_msg->buttons = build_buttons(event, 3, TYPE_ENCOUNTER, TYPE_ENCOUNTER_MSG);
  }

}

void main_embed(
  const struct discord_interaction *event, 
  struct Message *discord_msg) 
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  embed->description = calloc(SIZEOF_DESCRIPTION, sizeof(char));
  int item_type = 0;

  if (event->data->custom_id)
  { //this is a response
    if (event->data->custom_id[0] == TYPE_ENCOUNTER_MSG) 
    { //this is an encounter response
      encounter_embed(event, discord_msg);
    } 
    else if ((rand() % MAX_CHANCE) < ENCOUNTER_CHANCE && scurry.war_flag == 0)
    { // otherwise, can the player get an encounter? (scurry cannot be at war)
      player.select_encounter = rand() % biomes[player.biome].encounter_size;
      encounter_embed(event, discord_msg);
    }
    else { // if not, create the normal response
      discord_msg->buttons = main_button_response(event, &item_type, 3, TYPE_MAIN_MSG);

      embed->thumbnail = discord_set_embed_thumbnail( fill_git_url(item_types[item_type].file_path) );

      embed->title = format_str(SIZEOF_TITLE, "You found %s!", item_types[item_type].formal_name);
    
      generate_rewards(event, discord_msg, item_type);
      player.main_cd = time(NULL) + COOLDOWN;
    } 
  } else { //this is an init
    embed->title = format_str(SIZEOF_TITLE, "Scouring the lands...");

    discord_msg->buttons = build_buttons(event, 3, TYPE_DIRT_PILE, TYPE_MAIN_MSG);
  }

  embed->image = discord_set_embed_image( fill_git_url(biomes[player.biome].biome_scene_path) );

  if (embed->thumbnail)
    printf("%s \n", embed->thumbnail->url);
}

/* Listens for slash command interactions */
int main_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);
  scurry = load_scurry_struct(player.scurry_id);

  energy_regen();
  ERROR_INTERACTION((time(NULL) < player.main_cd), "Cooldown not ready!");
  ERROR_INTERACTION((player.energy < 2), "You need more energy!");

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  main_embed(event, msg);

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

  if (event->data->custom_id)
    scurry_war_update(event);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);

  discord_embed_cleanup(msg->embed);
  free(msg->buttons);
  free(msg);

  update_player_row(event->member->user->id, player);
  update_scurry_row(scurry);

  return 0;
}