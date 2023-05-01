#pragma once

#include "main.h"

#define DIRECTORY_RESOURCE "./resource"
#define DIRECTORY_CONFIG "./config"

#define FILENAME_POKEMON "./resource/pokemon.json"
#define FILENAME_MOVES "./resource/moves.json"
#define FILENAME_FORMS "./resource/forms.json"
#define FILENAME_ALLGYMS "./resource/all_gyms.txt"

#define FILENAME_PREFS "./config/user.cfg"
#define FILENAME_GYMS "./config/gyms.cfg"

int count_new_lines(FILE* fp);
u_int check_if_file_exists(char* source_file_path);
int read_file_to_buffer_generic(char* destination_buffer, char* source_file_path);
int write_file_generic(char* writable_string, char* destination_file_path);
void write_to_file_user_preferences(user_preferences_struct* user_preferences);
void write_to_file_gyms(gym_node** user_gym_linked_list_head, char* destination_file_path);
int remove_file(char* remove_file_path);