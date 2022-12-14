/*

  This file handles buying materials from the Seedy Market

*/

struct discord_component seedy_purchase(
  const struct discord_interaction *event, 
  struct discord_component current_button,
  int current_material,
  int* material_type,
  struct Store *seedy_store)
{
  int* material_ptr = biomes[current_material].material_ptr;

  //if there's a custom id, this is a response
  if (event->data->custom_id
    && event->data->custom_id[1] -48 == current_material
    && player.golden_acorns > seedy_store[current_material].cost)
  {
    player.golden_acorns -= seedy_store[current_material].cost;

    *material_type = current_material;
    *material_ptr += seedy_store[current_material].quantity;

    player.materials.pine_cones += 15;
    player.materials.seeds += 15;
  }

  //build the button regardless to account for updated price
  if (player.golden_acorns >= seedy_store[current_material].cost )
  {
    current_button.style = DISCORD_BUTTON_PRIMARY;
  } 
  else {
    current_button.style = DISCORD_BUTTON_SECONDARY;
    current_button.disabled = true;
  }

  return current_button;
}

struct discord_components* build_seedy_buttons(
  const struct discord_interaction *event,
  int button_size,
  int* material_type,
  struct Store *seedy_store)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;
  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++)
  {
    buttons->array[i] = seedy_purchase(event, buttons->array[i], i, material_type, seedy_store);

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));

    struct File *material_index = seedy_store[i].item;

    emoji->name = material_index->emoji_name;
    emoji->id = material_index->emoji_id;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_SEEDY, i, event->member->user->id);

    buttons->array[i].custom_id = set_custom_id;
    buttons->array[i].type = DISCORD_COMPONENT_BUTTON;
    buttons->array[i].emoji = emoji;
  }

  return buttons;
}

enum SEEDY_FORMAT {
  SEEDY_GENERAL = 0,
  SEEDY_BIOMES = 1,
  SEEDY_SIZE = 2,
};

void seedy_shop(
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  CREATE_SEEDY_STORE;

  int material_type = 0;
  discord_msg->buttons = build_seedy_buttons(event, player.max_biome +1, &material_type, seedy_store);

  embed->title = format_str(SIZEOF_TITLE, "Seedy Market");

  embed->description = format_str(SIZEOF_DESCRIPTION,
      ""OFF_ARROW" Purchase any available biome material with Golden Acorns "GOLDEN_ACORNS". \n"
      ""OFF_ARROW" You also get **15** pine cones "PINE_CONES" and seeds "SEEDS"!");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = SEEDY_SIZE + player.max_biome +1;
  embed->fields->array = calloc(SEEDY_SIZE + player.max_biome +1, sizeof(struct discord_embed_field));

  /* Fill in player balance */
  embed->fields->array[SEEDY_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[SEEDY_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      "> "GOLDEN_ACORNS" Golden Acorns: **%s** \n"
      "> "SEEDS" Seeds: **%s** \n"
      "> "PINE_CONES" Pine Cones: **%s** \n\n",
      num_str(player.golden_acorns), num_str(player.materials.seeds), num_str(player.materials.pine_cones) );

  /* Fill in player materials (after button interactions) */
  char seedy_biome_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < player.max_biome +1; i++)
  {
    int* material_ptr = biomes[i].material_ptr;
    struct File biome_material = biomes[i].biome_material;

    ADD_TO_BUFFER(seedy_biome_field, SIZEOF_DESCRIPTION,
        "<:%s:%ld> %s: **%s** \n",
        biome_material.emoji_name, biome_material.emoji_id, 
        biome_material.formal_name, num_str(*material_ptr) );
  }

  embed->fields->array[SEEDY_BIOMES].name = format_str(SIZEOF_TITLE, "Biome Materials");
  embed->fields->array[SEEDY_BIOMES].value = format_str(SIZEOF_FIELD_VALUE, seedy_biome_field);

  /* Fill in upgade information in separate fields */
  for (int i = SEEDY_SIZE; i < SEEDY_SIZE + player.max_biome +1; i++)
  {
    int biome_index = i - SEEDY_SIZE;
    //References
    struct File *biome_material = seedy_store[biome_index].item;

    embed->fields->array[i].name = format_str(SIZEOF_TITLE, "%d <:%s:%ld> %s", 
        seedy_store[biome_index].quantity, biome_material->emoji_name, biome_material->emoji_id, biome_material->formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        " "INDENT" *Requires* **%s** "GOLDEN_ACORNS" Golden Acorns \n",
        num_str(seedy_store[biome_index].cost) );
  }

  embed->thumbnail = discord_set_embed_thumbnail(fill_git_url(SQ_BOOKIE_PATH) );

  embed->footer = (event->data->custom_id) ? discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "You received %d %ss!", 
        seedy_store[material_type].quantity, seedy_store[material_type].item->formal_name),
    fill_git_url(seedy_store[material_type].item->file_path)
  ) 
  : discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "/help | Details on how golden acorns work!"),
    fill_git_url(item_types[TYPE_NO_ACORNS].file_path) );
}

/* Listens for slash command interactions */
void seedy_interaction(
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

  seedy_shop(event, msg);

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