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
    if (rand() % MAX_CHANCE > 10 && item_type != TYPE_ENCOUNTER_MSG && player.biome == GRASSLANDS)
      rewards.catnip = (item_type < TYPE_ACORN_MOUTHFUL) ? genrand(10, 5)
          : (item_type < TYPE_LOST_STASH) ? genrand(15, 10) : genrand(25, 10);
  }
}

void get_season_event(
  struct discord *client,
  const struct discord_interaction *event)
{
  struct tm *info = get_UTC();

  printf("info success! \n");

  char* months[12] = {"Jan", "Feb", "Mar", "April", "May", "June", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char* seasons[4] = {"Spring (Spring Chicken Live!)", "Summer", "Fall", "Winter (Bunny's Endeavor Live!)"};

  printf("%d months \n", info->tm_mon);

  discord_create_interaction_response(client, event->id, event->token, 
  &(struct discord_interaction_response)
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = format_str(SIZEOF_DESCRIPTION,
          "\nCurrent Season: **%s** (Ends on %s **%d**)", 
          seasons[(info->tm_mday/7 < 4) ? info->tm_mday/7 : 3],
          months[(info->tm_mday < 21) ? info->tm_mon : (info->tm_mon +1 == 12) ? 0 : info->tm_mon +1],
          (info->tm_mday < 7) ? 7 : (info->tm_mday < 14) ? 14 : (info->tm_mday < 21) ? 21 : 1)
    }
  },
  NULL);

}
