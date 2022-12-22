/*

  This file will handle Bunny's Endeavor event

  - Acorns
  - Golden Acorn
  - Materials
  - Biome Materials
  - Energy

*/

enum STORE_ITEMS {
  BUNNY_ACORNS,
  BUNNY_MATERIAL,
  BUNNY_GOLDEN_ACORNS,
  BUNNY_BIOME_MATERIAL,
  BUNNY_ENERGY,
  BUNNY_STORE_SIZE
};

// widely used arrays only in the scope of this file
int quantity_array[5] = {ACORN_QUANTITY, MATERIAL_QUANTITY, GOLDEN_ACORN_QUANTITY, BIOME_MATERIAL_QUANTITY, ENERGY_QUANTITY};
int item_index_values[5] = {ITEM_ACORNS, ITEM_MATERIALS, ITEM_GOLDEN_ACORN, 0, ITEM_ENERGY};

struct discord_component bunny_purchase(
  const struct discord_interaction *event, 
  struct discord_component current_button,
  int current_item,
  int* item_type)
{
  int bunny_price = CATNIP_UNIT_COST * (current_item +1);

  int *material_ptr = biomes[player.biome].material_ptr;

  //if there's a custom id, this is a response
  if (event->data->custom_id
    && event->data->custom_id[1] -48 == current_item
    && player.catnip > bunny_price)
  {
    player.catnip -= bunny_price;

    switch (current_item) {
      case BUNNY_ACORNS:
        player.acorns += ACORN_QUANTITY;
        *item_type = ITEM_ACORNS;
        break;
      case BUNNY_MATERIAL:
        player.materials.seeds += MATERIAL_QUANTITY;
        player.materials.pine_cones += MATERIAL_QUANTITY;
        *item_type = ITEM_MATERIALS;
        break;
      case BUNNY_GOLDEN_ACORNS:
        player.golden_acorns += GOLDEN_ACORN_QUANTITY;
        *item_type = ITEM_GOLDEN_ACORN;
        break;
      case BUNNY_BIOME_MATERIAL:
        *material_ptr += BIOME_MATERIAL_QUANTITY;
        *item_type = player.biome;
        break;
      case BUNNY_ENERGY:
        player.energy += ENERGY_QUANTITY;
        *item_type = ITEM_ENERGY;
        break;
    }

  }

  //build the button regardless to account for updated price
  if (player.catnip >= bunny_price )
  {
    current_button.style = DISCORD_BUTTON_PRIMARY;
  } 
  else {
    current_button.style = DISCORD_BUTTON_SECONDARY;
    current_button.disabled = true;
  }

  return current_button;
}

struct discord_components* build_bunny_buttons(
  const struct discord_interaction *event,
  int button_size,
  int* item_type)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++)
  {
    buttons->array[i] = bunny_purchase(event, buttons->array[i], i, item_type);

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    struct File *item_index = (i == BUNNY_BIOME_MATERIAL) ? &(biomes[player.biome].biome_material) : &(items[item_index_values[i]]);

    emoji->name = item_index->emoji_name;
    emoji->id = item_index->emoji_id;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_BUNNY, i, event->member->user->id);

    buttons->array[i].custom_id = set_custom_id;
    buttons->array[i].type = DISCORD_COMPONENT_BUTTON;
    buttons->array[i].emoji = emoji;
  }

  return buttons;
}

enum BUNNY_FORMAT {
  BUNNY_GENERAL,
  BUNNY_SIZE
};

void bunny_shop(
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  int item_type = 0;
  discord_msg->buttons = build_bunny_buttons(event, 5, &item_type);

  embed->title = format_str(SIZEOF_TITLE, "Seedy Market");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      ""OFF_ARROW" Purchase an item with *Catnip* "CATNIP". \n"
      ""OFF_ARROW" The biome material sold is based on the biome you're currently in!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = BUNNY_SIZE +5;
  embed->fields->array = calloc(BUNNY_SIZE +5, sizeof(struct discord_embed_field));

  struct File biome_material = biomes[player.biome].biome_material;

  /* Fill in player balance */
  embed->fields->array[BUNNY_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[BUNNY_GENERAL].value = format_str(SIZEOF_FIELD_VALUE, 
      "> "CATNIP" Catnip: **%s** \n"
      "> "ACORNS" Acorns: **%s** \n"
      "> "PINE_CONES" Pine Cones: **%s** \n"
      "> "SEEDS" Seeds: **%s** \n"
      "> "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
      "> <:%s:%ld> %s: **%s** \n"
      "> "ENERGY" Energy: **%s**/%d \n", 
      num_str(player.catnip), num_str(player.acorns), num_str(player.materials.pine_cones), num_str(player.materials.seeds), num_str(player.golden_acorns),
      biome_material.emoji_name, biome_material.emoji_id, biome_material.formal_name, num_str(*biomes[player.biome].material_ptr),
      num_str(player.energy), MAX_ENERGY );

  /* Fill in upgade information in separate fields */
  for (int i = BUNNY_SIZE; i < BUNNY_SIZE +5; i++)
  {
    int file_index = i - BUNNY_SIZE;

    struct File *item_index = (file_index == BUNNY_BIOME_MATERIAL) ? &(biomes[player.biome].biome_material) : &(items[item_index_values[file_index]]);

    embed->fields->array[i].name = format_str(SIZEOF_TITLE, "%s <:%s:%ld> %s", 
        num_str(quantity_array[file_index]), item_index->emoji_name, item_index->emoji_id, item_index->formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        " "INDENT" *Requires* **%s** "CATNIP" Catnip \n",
        num_str(CATNIP_UNIT_COST * (file_index +1)));
  }

  embed->thumbnail = discord_set_embed_thumbnail(fill_git_url(BUNNY_ED_PATH) );

  embed->footer = (event->data->custom_id) ? discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "You received %s %s!",
        num_str(quantity_array[event->data->custom_id[1] -48]), items[item_type].formal_name),
    fill_git_url(items[item_type].file_path) 
  ) 
  : discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "/help | Details on how catnip works!"),
    fill_git_url(item_types[TYPE_NO_ACORNS].file_path) );
}

/* Listens for slash command interactions */
int bunny_interaction(
  struct discord *client, 
  const struct discord_interaction *event, 
  struct Message *msg) 
{
  struct tm *info = get_UTC();

  ERROR_INTERACTION((info->tm_mday < 21), "This event is not active!");

  energy_regen();
  player = load_player_struct(event->member->user->id);

  //Load Author
  msg->embed->author = discord_set_embed_author(
    format_str(SIZEOF_TITLE, event->member->user->username),
    format_str(SIZEOF_URL, "https://cdn.discordapp.com/avatars/%lu/%s.png", 
        event->member->user->id, event->member->user->avatar) );

  bunny_shop(event, msg);

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

  return 0;
}