/*

  This file handles upgrading stats and expenses

  - Displays the player's balance and details about each stat
  - Details include level, cost to upgrade, and total value

  Changes made since last push:
    - detached stats from biomes
*/

/* Handles upgrading a stat */
struct discord_component upgrade_stat(
  const struct discord_interaction *event, 
  struct discord_component current_button,
  int current_stat)
{
  int* stat_lv_ptr = stat_files[current_stat].stat_ptr;
  int stat_cost = generate_price(*stat_lv_ptr, UNIT_ACORN, stat_files[current_stat].price_mult);

  //if there's a custom id, this is a response
  if (event->data->custom_id
    && event->data->custom_id[1] -48 == current_stat // compares button index at [1] to button index of button pressed
    && player.acorns >= stat_cost )
  {
    player.acorns -= stat_cost;

    (*stat_lv_ptr)++;
  }

  //build the button regardless to account for updated price
  if (player.acorns >= stat_cost)
  {
    current_button.style = DISCORD_BUTTON_PRIMARY;
  } 
  else {
    current_button.style = DISCORD_BUTTON_SECONDARY;
    current_button.disabled = true;
  }

  return current_button;
}

struct discord_components* build_upgrade_buttons(
  const struct discord_interaction *event,
  int button_size)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++)
  {
    buttons->array[i] = upgrade_stat(event, buttons->array[i], i);

    int is_evolution = ((*stat_files[i].stat_ptr +1) % STAT_EVOLUTION == 0) ? 1 : 0;

    buttons->array[i].label = stat_files[i].formal_name;

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    if (is_evolution) {
      emoji->name = evo_squirrels[player.active_squirrel].emoji_name;
      emoji->id = evo_squirrels[player.active_squirrel].emoji_id;
    } else {
      emoji->name = squirrels[player.active_squirrel].emoji_name;
      emoji->id = squirrels[player.active_squirrel].emoji_id;
    }

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_STORE, i, event->member->user->id);
    // i is used instead of a char!!! Match i with biome type to get squirrel!

    buttons->array[i].custom_id = set_custom_id;
    buttons->array[i].type = DISCORD_COMPONENT_BUTTON;
    buttons->array[i].emoji = emoji;
  }

  return buttons;
}

enum STORE_FORMAT {
  STORE_GENERAL = 0,
  STORE_SIZE = 1,
};

void player_shop(
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  discord_msg->buttons = build_upgrade_buttons(event, STAT_SIZE);

  embed->title = format_str(SIZEOF_TITLE, "Upgrades");

  embed->description = format_str(SIZEOF_DESCRIPTION,
    ""OFF_ARROW" By purchasing a stat level, you are increasing your earning. \n");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = STORE_SIZE + STAT_SIZE;
  embed->fields->array = calloc(STORE_SIZE + STAT_SIZE, sizeof(struct discord_embed_field));

  /* Fill in player balance */
  embed->fields->array[STORE_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[STORE_GENERAL].value = format_str(SIZEOF_FIELD_VALUE, "> "ACORNS" Acorns: **%s** \n", num_str(player.acorns) );

  /* Fill in upgade information in separate fields */
  for (int i = STORE_SIZE; i < STORE_SIZE + STAT_SIZE; i++)
  {
    int biome_index = i - STORE_SIZE;
    //References
    char* stahr_type = (*stat_files[biome_index].stat_ptr < BRONZE_BRACKET) ? BRONZE_STAHR
      : (*stat_files[biome_index].stat_ptr < SILVER_BRACKET) ? SILVER_STAHR : STAHR;

    embed->fields->array[i].name = format_str(SIZEOF_TITLE, 
        "%s (%s **%d**)", 
        stat_files[biome_index].formal_name, stahr_type, *stat_files[biome_index].stat_ptr);

    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        (biome_index == STAT_PROFICIENCY) ? " "OFF_ARROW" %s (x**%0.1f**) \n" : " "OFF_ARROW" %s (+**%0.0f**) \n",
        stat_files[biome_index].description, 
        generate_factor(stat_files[biome_index].value_mult, *stat_files[biome_index].stat_ptr) );

    ADD_TO_BUFFER(embed->fields->array[i].value, SIZEOF_FIELD_VALUE,
        "*Costs* **%s** "ACORNS" Acorns",
        num_str( generate_price(*stat_files[biome_index].stat_ptr, UNIT_ACORN, stat_files[biome_index].price_mult) ) );
  }

}


/* Listens for slash command interactions */
void store_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  player = load_player_struct(event->member->user->id);

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  player_shop(event, msg);

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
}