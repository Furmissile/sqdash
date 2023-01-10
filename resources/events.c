/*

  This file handles passive events

*/

void factor_season(int item_type)
{
  struct tm *info = get_UTC();

  if (info->tm_mday < 7) {
    if (rand() % MAX_CHANCE > 80 && item_type != TYPE_ENCOUNTER_MSG )
      rewards.golden_acorns = (item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
          : (item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);
    rewards.acorns *= SPRING_MULT;
  }
  else if (info->tm_mday < 14)
    rewards.acorns *= SUMMER_MULT;
  else if (info->tm_mday < 21)
    rewards.acorns *= FALL_MULT;
  else {
    rewards.acorns *= WINTER_MULT;
    if (rand() % MAX_CHANCE > 80 && item_type != TYPE_ENCOUNTER_MSG && player.biome == GRASSLANDS)
      rewards.catnip = (item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
          : (item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);
  }
}

void get_season_event(
  struct discord *client,
  const struct discord_interaction *event)
{
  struct tm *info = get_UTC();

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char* seasons[4] = {"Spring (Spring Chicken Live!)", "Summer", "Fall", "Winter (Bunny's Endeavor Live!)"};

  discord_create_interaction_response(client, event->id, event->token, 
  &(struct discord_interaction_response)
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = format_str(SIZEOF_DESCRIPTION,
          "Current Season: **%s** (Ends on %s **%d**)", 
          seasons[(info->tm_mday/7 < 4) ? info->tm_mday/7 : 3],
          months[(info->tm_mday < 21) ? info->tm_mon : (info->tm_mon +1 == 12) ? 0 : info->tm_mon +1],
          (info->tm_mday < 7) ? 7 : (info->tm_mday < 14) ? 14 : (info->tm_mday < 21) ? 21 : 1)
    }
  },
  NULL);

}

enum DAILY_REWARDS {
  DAILY_ACORNS,
  DAILY_MATERIALS,
  DAILY_GOLDEN_ACORNS,
  DAILY_BIOME_MATERIALS,
  DAILY_ENERGY
};

void give_daily(
  struct discord *client,
  const struct discord_interaction *event)
{
  player = load_player_struct(event->member->user->id);
  CREATE_DAILY_REWARDS;
  char update_message[SIZEOF_DESCRIPTION] = {};

  // if cooldown is still active...
  if (time(NULL) < player.daily_cd)
  {
    int time_past = player.daily_cd - time(NULL);
    if (time_past > 3600)
      ADD_TO_BUFFER(update_message, SIZEOF_DESCRIPTION, "Please wait **%d** hours!", time_past/3600);
    else if (time_past > 60)
      ADD_TO_BUFFER(update_message, SIZEOF_DESCRIPTION, "Please wait **%d** minutes!", time_past/60);
    else
      ADD_TO_BUFFER(update_message, SIZEOF_DESCRIPTION, "Please wait less than 1 minute!");
  }
  // otherwise, the reward will still be granted
  else {
    int chance = rand() % MAX_CHANCE;

    int daily_type = 
        (chance < JUNK_CHANCE) ? DAILY_ENERGY
      : (chance < COMMON_CHANCE) ? DAILY_ACORNS
      : (chance < UNCOMMON_CHANCE) ? DAILY_MATERIALS
      : (chance < CONTAINER_CHANCE) ? DAILY_GOLDEN_ACORNS : DAILY_BIOME_MATERIALS;

    struct File *selected_item = daily_rewards[daily_type].item;
    
    int quantity = daily_rewards[daily_type].quantity * (float)genrand(100, 130)/100;
  
    ADD_TO_BUFFER(update_message, SIZEOF_DESCRIPTION, 
        "<@%ld> received **%s** <:%s:%ld> %s \n", 
        event->member->user->id, num_str(quantity), selected_item->emoji_name, 
        selected_item->emoji_id, selected_item->formal_name);

    switch(daily_type) {
      case DAILY_ENERGY:
        player.energy += quantity;
        break;
      case DAILY_ACORNS:
        player.acorns += quantity;
        break;
      case DAILY_MATERIALS:
        player.materials.pine_cones += quantity;
        player.materials.seeds += quantity;
        break;
      case DAILY_GOLDEN_ACORNS:
        player.golden_acorns += quantity;
        break;
      case DAILY_BIOME_MATERIALS:
        *(biomes[rand() % player.max_biome].material_ptr) += quantity;
        break;
    }

    player.daily_cd = time(NULL) + DAILY_COOLDOWN;
    update_player_row(event->member->user->id, player);
  }

  discord_create_interaction_response(client, event->id, event->token, 
  &(struct discord_interaction_response)
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = (time(NULL) > player.daily_cd) ? DISCORD_MESSAGE_EPHEMERAL : 0,
      .content = format_str(SIZEOF_DESCRIPTION, update_message) 
    }
  },
  NULL);
}