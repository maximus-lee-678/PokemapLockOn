#pragma once

#include "main.h"
#include "../headers/user_interface.h"

#define BIT_POKEMON 0
#define BIT_MOVES 1
#define BIT_FORMS 2
#define BIT_GYMS 3

#define MAIN_NAME "Options"
#define BACK_TITLE "Enter [-back] to exit."
#define BACK_COMMAND "-back"

#define SET_HOME_TITLE "Set Home Coordinates"
#define SET_HOME_COMMAND "sethome"
#define CHOOSE_GYM_TITLE "Edit Gym List"
#define CHOOSE_GYM_COMMAND "gyms"
#define SET_REFRESH_TITLE "Set Refresh Interval"
#define SET_REFRESH_COMMAND "interval"
#define CHOOSE_DISPLAY_TIER_TITLE "Choose Raid Tiers to Display"
#define CHOOSE_DISPLAY_TIER_COMMAND "tierdisplay"
#define CHOOSE_ALERT_TIER_TITLE "Choose Raid Tiers to Sound Alert"
#define CHOOSE_ALERT_TIER_COMMAND "tieralert"
#define CHOOSE_DISPLAY_MODE_TITLE "Choose Gym Information Detail Level"
#define CHOOSE_DISPLAY_MODE_COMMAND "displaymode"
#define RESET_USER_PREFS_TITLE "Reset User Preferences"
#define RESET_USER_PREFS_COMMAND "reset"
#define UPDATE_GAME_MASTER_TITLE "Refresh Assets"
#define UPDATE_GAME_MASTER_COMMAND "updatemaster"
#define UPDATE_GYM_INFO_TITLE "Refresh Cached Gym List (!)"
#define UPDATE_GYM_INFO_COMMAND "updatelist"
#define EXIT_PROGRAM_TITLE "Close Program"
#define EXIT_PROGRAM_COMMAND "stop"
#define EXIT_RETURN_TITLE "Back to Raid Information (Refreshes it)"
#define EXIT_RETURN_COMMAND "done"

#define PROCESS_GYM_TITLE_0 "Add New Gym"
#define PROCESS_GYM_TITLE_0_A "Results for Search"
#define PROCESS_GYM_TITLE_1 "Add New Gym after Index"
#define PROCESS_GYM_TITLE_2 "Delete Gym at Index"
#define PROCESS_GYM_TITLE_3 "Delete ALL Gyms"
#define PROCESS_GYM_TITLE_4 "Swap Gym Indexes"
#define PROCESS_GYM_ADD_ARG_0 "add"
#define PROCESS_GYM_ADD_ARG_1 "-pos"
#define PROCESS_GYM_ADD_ARG_2 "-curr"
#define PROCESS_GYM_DEL_ARG_0 "del"
#define PROCESS_GYM_DEL_ARG_1 "-all"
#define PROCESS_GYM_SWAP_ARG_0 "swap"
#define PROCESS_GYM_NUM_ARG "<num>"

u_int open_user_prefs_menu(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, u_int* changes, char* raid_json_string);
void load_user_prefs(user_preferences_struct* user_preferences, char* user_prefs_string);
void create_user_prefs_from_scratch(user_preferences_struct* user_preferences);
void create_user_gyms_from_scratch(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, char* raid_json_string);

u_int process_sethome(user_preferences_struct* user_preferences, int force_completion);
u_int process_interval(user_preferences_struct* user_preferences, int force_completion);
u_int process_tierdisplay(user_preferences_struct* user_preferences, int force_completion);
u_int process_tieralert(user_preferences_struct* user_preferences, int force_completion);
u_int process_displaymode(user_preferences_struct* user_preferences, int force_completion);
void process_reset(user_preferences_struct* user_preferences);

u_int process_gyms(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, char* raid_json_string);
u_int process_gyms_add(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, gym_node** all_gym_linked_list_head, u_int insertion_index);
u_int process_gyms_add_curr(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, gym_node** current_active_gym_linked_list_head, u_int insertion_index);
u_int process_gyms_delete(gym_node** user_gym_linked_list_head, u_int deletion_index);
u_int process_gyms_swap(gym_node** user_gym_linked_list_head, u_int index_1, u_int index_2);

u_int process_update_master();
u_int process_update_gym_list();

int confirmation_yes_no(int prompt_again);
int confirmation_back(char* buffer);

void receive_input(char* input_buffer);
int verify_string_is_uint(char* string);