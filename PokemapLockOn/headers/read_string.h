#pragma once

#include "main.h"

// Info below from: 
// Form: https://sgpokemap.com/json/forms.json
// Moves: https://sgpokemap.com/json/moves.json
// Pokemon: https://sgpokemap.com/js/script_gym.js

char** read_gym_string(char* gym_string, int* number_of_lines);
void init_gym_linked_list(int total, char** gym_string_array, gym_node** gym_linked_list_head);
void init_raid_details_struct(gym_node** gym_linked_list_head, raid_details_struct* array_raid_details);
void populate_basic_fields(int total, char* raid_json_string, raid_details_struct* array_raid_details);
void populate_from_local(int total, raid_details_struct* array_raid_details);
void populate_pokemon_name(int total, char* pokemon_json_string, raid_details_struct* array_raid_details);
void populate_move_name(int total, char* moves_json_string, raid_details_struct* array_raid_details);
void populate_form_name(int total, char* forms_json_string, raid_details_struct* array_raid_details);
u_int raid_string_to_linked_list(gym_node** head, char* raid_json_string);
void stringcopy_named_json_value(char* destination, char* source, char* json_key);
void stringcopy_adjacent_json_value(char* destination, char* source);
void stringcopy_config_file(char* destination, char* source, char* key);
void extract_pokemon_json(char* pokemon_json_string);