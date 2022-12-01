/*

This file handles buff creation and effects

  - Displays balance and buff info
  - Information including name and effect
  - Buttons include materials the player can use to craft a buffs

Mechanic:
1. Select a button:
  -> Change the style of every other button to DISCORD_BUTTON_PRIMARY
  -> Disable all buttons
  -> Generate duration by what material was used

*/


struct discord_component select_materials(
  const struct discord_interaction *event,
  struct discord_component current_button,
  int button_index,
  int* rune_type)
{
  // if there's a custom id, this is a response
  if (event->data->custom_id
    && event->data->custom_id[1] -48 == button_index // compares button index at [1] to button index of button pressed
    && player.acorns >= ACORN_BUFF_COST
    && player.materials.seeds >= SEEDS_BUFF_COST
    && *biomes[button_index].material_ptr >= BIOME_MATERIAL_BUFF_COST )
  {
    player.acorns -= ACORN_BUFF_COST;
    player.materials.seeds -= SEEDS_BUFF_COST;
    *biomes[button_index].material_ptr -= BIOME_MATERIAL_BUFF_COST;

    //matched with Buffs enum
    *rune_type = rand() % BUFF_SIZE;

    switch (*rune_type) {
      case ACUITY_ACORN:
        player.buffs.acuity_acorn += ((button_index +1) * (MATERIAL_FACTOR));
        break;
      case ENDURANCE_ACORN:
        player.buffs.endurance_acorn += ((button_index +1) * (MATERIAL_FACTOR));
        break;
      case LUCK_ACORN:
        player.buffs.luck_acorn += ((button_index +1) * (MATERIAL_FACTOR));
        break;
      case PROFICIENCY_ACORN:
        player.buffs.proficiency_acorn += ((button_index +1) * (MATERIAL_FACTOR));
        break;
      case SMELL_ACORN:
        player.buffs.smell_acorn += ((button_index +1) * (MATERIAL_FACTOR));
        break;
      default:
        printf("\n\nThis buff doesnt exist! \n\n");
    }

  }

  if (player.acorns >= ACORN_BUFF_COST
    && player.materials.seeds >= SEEDS_BUFF_COST
    && *biomes[button_index].material_ptr >= BIOME_MATERIAL_BUFF_COST )
  {
    current_button.style = DISCORD_BUTTON_PRIMARY;
  }
  else {
    current_button.style = DISCORD_BUTTON_SECONDARY;
    current_button.disabled = true;
  }

  return current_button;
}

struct discord_components* build_buff_buttons(
  const struct discord_interaction *event,
  int button_size,
  int* rune_type)
{
  struct discord_components *buttons = calloc(1, sizeof(struct discord_components));

  buttons->size = button_size;

  buttons->array = calloc(button_size, sizeof(struct discord_component));

  for (int i = 0; i < buttons->size; i++)
  {
    buttons->array[i] = select_materials(event, buttons->array[i], i, rune_type);

    struct File biome_material = biomes[i].biome_material;

    struct discord_emoji *emoji = calloc(1, sizeof(struct discord_emoji));
    emoji->name = biome_material.emoji_name;
    emoji->id = biome_material.emoji_id;

    char* set_custom_id = calloc(SIZEOF_CUSTOM_ID, sizeof(char));
    snprintf(set_custom_id, SIZEOF_CUSTOM_ID, "%c%d_%ld", TYPE_E_ACORN, i, event->member->user->id);
  
    buttons->array[i].custom_id = set_custom_id;
    buttons->array[i].type = DISCORD_COMPONENT_BUTTON;
    buttons->array[i].emoji = emoji;
  }

  return buttons;
}

enum BUFFS_FORMAT {
  BUFFS_GENERAL = 0,
  BUFFS_MATERIALS = 1,
  BUFFS_PRICES = 2,
  BUFFS_SIZE = 3,
  BUFFS_TYPES = 8
};

void power_shop(
  const struct discord_interaction *event,
  struct Message *discord_msg)
{
  struct discord_embed *embed = discord_msg->embed;
  embed->color = player.color;

  int rune_type = 0;
  discord_msg->buttons = build_buff_buttons(event, player.max_biome +1, &rune_type);

  embed->title = format_str(SIZEOF_TITLE, "Buffs Shop");

  embed->description = format_str(SIZEOF_DESCRIPTION,
    ""OFF_ARROW" Select a biome material and receive a random buff of the ones listed. \n"
    ""OFF_ARROW" Selecting a higher biome material grants a longer duration. \n"
    ""OFF_ARROW" Buffs are stackable, but deplete regardless of whether you receive the benefit! \n"
    ""OFF_ARROW" Active buffs show up with a "QUEST_MARKER" near it with its duration.");

  embed->fields = calloc(1, sizeof(struct discord_embed_fields));
  embed->fields->size = BUFFS_TYPES;
  embed->fields->array = calloc(BUFFS_TYPES, sizeof(struct discord_embed_field));

  embed->fields->array[BUFFS_GENERAL].name = format_str(SIZEOF_TITLE, "Balance");
  embed->fields->array[BUFFS_GENERAL].value = format_str(SIZEOF_FIELD_VALUE,
      "\n > "ACORNS" Acorns: **%s**"
      "\n > "SEEDS" Seeds: **%s**",
      num_str(player.acorns), num_str(player.materials.seeds) );

  // Fill in biome materials (after balance)
  char buffs_materials_field[SIZEOF_FIELD_VALUE] = {};
  for (int i = 0; i < player.max_biome +1; i++)
  {
    int* material_ptr = biomes[i].material_ptr;
    struct File biome_material = biomes[i].biome_material;

    ADD_TO_BUFFER(buffs_materials_field, SIZEOF_DESCRIPTION,
        "> <:%s:%ld> %s: **%s** \n",
        biome_material.emoji_name, biome_material.emoji_id, 
        biome_material.formal_name, num_str(*material_ptr) );
  }
  embed->fields->array[BUFFS_MATERIALS].name = format_str(SIZEOF_TITLE, "Biome Materials");
  embed->fields->array[BUFFS_MATERIALS].value = format_str(SIZEOF_FIELD_VALUE, buffs_materials_field);

  embed->fields->array[BUFFS_PRICES].name = format_str(SIZEOF_TITLE, "Enchanted Acorn Cost");
  embed->fields->array[BUFFS_PRICES].value = format_str(SIZEOF_FIELD_VALUE,
      "> **%s** "ACORNS" Acorns \n"
      "> **%d** "SEEDS" Seeds \n"
      "> **%d** Any selected biome material",
      num_str(ACORN_BUFF_COST), SEEDS_BUFF_COST, BIOME_MATERIAL_BUFF_COST);

  int enchanted_acorn[5] = {
    player.buffs.acuity_acorn, 
    player.buffs.endurance_acorn, 
    player.buffs.luck_acorn, 
    player.buffs.proficiency_acorn, 
    player.buffs.smell_acorn
  };

  for (int i = BUFFS_SIZE; i < BUFFS_TYPES; i++)
  {
    int buffs_index = i - BUFFS_SIZE;
  
    if (enchanted_acorn[buffs_index] > 0)
      embed->fields->array[i].name = format_str(SIZEOF_TITLE,
          "<:%s:%ld> %s ("QUEST_MARKER" **%d**) \n", 
          enchanted_acorns[buffs_index].emoji_name, enchanted_acorns[buffs_index].emoji_id, 
          enchanted_acorns[buffs_index].formal_name, enchanted_acorn[buffs_index]);
    else
      embed->fields->array[i].name = format_str(SIZEOF_TITLE,
          "<:%s:%ld> %s \n", 
          enchanted_acorns[buffs_index].emoji_name, enchanted_acorns[buffs_index].emoji_id, enchanted_acorns[buffs_index].formal_name);
    
    embed->fields->array[i].value = format_str(SIZEOF_FIELD_VALUE,
        " "OFF_ARROW" %s \n", enchanted_acorns[buffs_index].description);
  }

  embed->thumbnail = (event->data->custom_id) ? discord_set_embed_thumbnail( fill_git_url(enchanted_acorns[rune_type].file_path) )
      : discord_set_embed_thumbnail( fill_git_url(SQ_CHEM_PATH) );

  embed->footer = (event->data->custom_id) ? discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "You received the %s!", enchanted_acorns[rune_type].formal_name),
    fill_git_url(enchanted_acorns[rune_type].file_path)
  ) 
  : discord_set_embed_footer(
    format_str(SIZEOF_FOOTER_TEXT, "/help | Details on how enchanted acorns work!"),
    fill_git_url(item_types[TYPE_NO_ACORNS].file_path)
  );

}

/* Listens for slash command interactions */
int buffs_interaction(
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

  power_shop(event, msg);

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