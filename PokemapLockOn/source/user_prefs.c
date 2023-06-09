#include "../headers/user_prefs.h"
#include "../headers/read_file.h"
#include "../headers/read_string.h"

// Draws customisation menu when user interrupts normal execution
u_int open_user_prefs_menu(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, u_int* changes, char* raid_json_string) {
	int must_print_header = 1, return_code = 0;
	int changes_to_prefs = 0, changes_to_user_gyms = 0;

	while (1) {
		if (must_print_header) {
			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			print_in_center(get_screen_dims('x'), 1, MAIN_NAME);
			print_divider(get_screen_dims('x'), "full");

			custom_print('i', "%s", SET_HOME_TITLE, SET_HOME_COMMAND);
			SET_IN_FOR; custom_print('0', " [%s]", SET_HOME_COMMAND);
			SET_CYAN; custom_print('0', " {%f, %f}\n", user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);

			custom_print('i', "%s", CHOOSE_GYM_TITLE, CHOOSE_GYM_COMMAND);
			SET_IN_FOR; custom_print('0', " [%s]\n", CHOOSE_GYM_COMMAND);

			custom_print('i', "%s", SET_REFRESH_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]", SET_REFRESH_COMMAND);
			SET_CYAN; custom_print('0', " {%d minute(s)}\n", user_preferences->refresh_interval);

			custom_print('i', "%s", CHOOSE_DISPLAY_TIER_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]", CHOOSE_DISPLAY_TIER_COMMAND);
			SET_CYAN; custom_print('0', " {%s}\n", user_preferences->display_tiers);

			custom_print('i', "%s", CHOOSE_ALERT_TIER_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]", CHOOSE_ALERT_TIER_COMMAND);
			SET_CYAN; custom_print('0', " {%s}\n", user_preferences->alert_tiers);

			custom_print('i', "%s", CHOOSE_DISPLAY_MODE_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]", CHOOSE_DISPLAY_MODE_COMMAND);
			SET_CYAN; custom_print('0', " {%s}\n", user_preferences->display_mode == DISPLAY_MODE_MINIMAL ? "Minimal" : "Verbose");

			custom_print('i', "%s", RESET_USER_PREFS_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]\n", RESET_USER_PREFS_COMMAND);

			custom_print('i', "%s", UPDATE_GAME_MASTER_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]\n", UPDATE_GAME_MASTER_COMMAND);

			custom_print('i', "%s", UPDATE_GYM_INFO_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]\n", UPDATE_GYM_INFO_COMMAND);

			custom_print('i', "%s", EXIT_PROGRAM_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]\n", EXIT_PROGRAM_COMMAND);

			custom_print('i', "%s", EXIT_RETURN_TITLE);
			SET_IN_FOR; custom_print('0', " [%s]\n", EXIT_RETURN_COMMAND);

			print_divider(get_screen_dims('x'), "full");

			must_print_header = 0;
		}

		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		// ugliest switch statement ever
		if (!strcmp(input_buffer, SET_HOME_COMMAND)) {
			if (process_sethome(user_preferences, 0)) {
				changes_to_prefs = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, CHOOSE_GYM_COMMAND)) {
			if (process_gyms(user_preferences, user_gym_linked_list_head, raid_json_string)) {
				changes_to_user_gyms = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, SET_REFRESH_COMMAND)) {
			if (process_interval(user_preferences, 0)) {
				changes_to_prefs = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, CHOOSE_DISPLAY_TIER_COMMAND)) {
			if (process_tierdisplay(user_preferences, 0)) {
				changes_to_prefs = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, CHOOSE_ALERT_TIER_COMMAND)) {
			if (process_tieralert(user_preferences, 0)) {
				changes_to_prefs = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, CHOOSE_DISPLAY_MODE_COMMAND)) {
			if (process_displaymode(user_preferences, 0)) {
				changes_to_prefs = 1;
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, RESET_USER_PREFS_COMMAND)) {
			process_reset(user_preferences);

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, UPDATE_GAME_MASTER_COMMAND)) {
			if (process_update_master()) {
				set_bit(changes, BIT_POKEMON, 1);
				set_bit(changes, BIT_MOVES, 1);
				set_bit(changes, BIT_FORMS, 1);
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, UPDATE_GYM_INFO_COMMAND)) {
			if (process_update_gym_list()) {
				set_bit(changes, BIT_GYMS, 1);
			}

			must_print_header = 1;
		}
		else if (!strcmp(input_buffer, EXIT_PROGRAM_COMMAND)) {
			return_code = 1;
			break;
		}
		else if (!strcmp(input_buffer, EXIT_RETURN_COMMAND)) {
			return_code = 0;
			break;
		}
		else {
			custom_print('!', "No valid inputs found.\n");
		}
	}

	if (changes_to_prefs) {
		write_to_file_user_preferences(user_preferences);
	}
	if (changes_to_user_gyms) {
		write_to_file_gyms(user_gym_linked_list_head, FILENAME_GYMS);
	}

	return return_code;
}

// Startup: reads preferences from file
void load_user_prefs(user_preferences_struct* user_preferences, char* user_prefs_string) {
	char working_buffer[64] = "";
	char* token = NULL;

	stringcopy_config_file(working_buffer, user_prefs_string, SET_HOME_COMMAND);
	token = strtok(working_buffer, ",");
	user_preferences->home_coordinates[0] = strtod(token, NULL);
	token = strtok(NULL, ",");
	user_preferences->home_coordinates[1] = strtod(token, NULL);

	stringcopy_config_file(working_buffer, user_prefs_string, SET_REFRESH_COMMAND);
	user_preferences->refresh_interval = atoi(working_buffer);

	stringcopy_config_file(user_preferences->display_tiers, user_prefs_string, CHOOSE_DISPLAY_TIER_COMMAND);

	stringcopy_config_file(user_preferences->alert_tiers, user_prefs_string, CHOOSE_ALERT_TIER_COMMAND);

	stringcopy_config_file(working_buffer, user_prefs_string, CHOOSE_DISPLAY_MODE_COMMAND);
	user_preferences->display_mode = atoi(working_buffer);
}

// Startup: no file found, writes new preferences to file and uses new preferences
void create_user_prefs_from_scratch(user_preferences_struct* user_preferences) {
	process_sethome(user_preferences, 1);
	process_interval(user_preferences, 1);
	process_tierdisplay(user_preferences, 1);
	process_tieralert(user_preferences, 1);
	process_displaymode(user_preferences, 1);

	write_to_file_user_preferences(user_preferences);
}

// Startup: no file found, writes gyms to file and uses gym linked list
void create_user_gyms_from_scratch(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, char* raid_json_string) {
	process_gyms(user_preferences, user_gym_linked_list_head, raid_json_string);

	write_to_file_gyms(user_gym_linked_list_head, FILENAME_GYMS);
}

// Handles sethome command
u_int process_sethome(user_preferences_struct* user_preferences, int force_completion) {
	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, SET_HOME_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Used when adding gyms to distinguish gyms with the same name.\n");
	custom_print('i', "Google Maps is a good place to find this value.\n");
	custom_print(':', "[Input Format] Decimal Degrees (%clat, %clng)\n", 241, 241);	// chars are +-
	if (!force_completion) {
		custom_print(':', "%s\n", BACK_TITLE);
	}
	print_divider(get_screen_dims('x'), "full");

	while (1) {
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (!force_completion && confirmation_back(input_buffer)) {
			return 0;
		}

		char* temp_ptr;
		double result = 0;
		errno = 0;

		// Extract the first token (lat)
		char* token = strtok(input_buffer, ",");
		if (token == NULL) {
			custom_print('!', "Incorrect format. (Number of parameters)\n");
			continue;
		}
		result = strtod(token, &temp_ptr);
		if (errno != 0 || *temp_ptr != '\0') {	// Checks if value is in ERANGE and string did not have letters
			custom_print('!', "Incorrect format. (Parameter format)\n");
			continue;
		}
		user_preferences->home_coordinates[0] = result;

		// Extract the second token (lng)
		token = strtok(NULL, ",");
		if (token == NULL) {
			custom_print('!', "Incorrect format. (Number of parameters)\n");
			continue;
		}
		result = strtod(token, &temp_ptr);
		if (errno != 0 || *temp_ptr != '\0') {	// Checks if value is in ERANGE and string did not have letters
			custom_print('!', "Incorrect format. (Parameter format)\n");
			continue;
		}

		user_preferences->home_coordinates[1] = result;

		// Ensure no more tokens
		token = strtok(NULL, ",");
		if (token != NULL) {
			custom_print('!', "Incorrect format. (Number of parameters)\n");
			continue;
		}

		if (!confirmation_yes_no(1)) {
			continue;
		}

		break;
	}

	custom_print('>', "Saved: %f, %f\n", user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);
	press_any();

	return 1;
}

// Handles interval command
u_int process_interval(user_preferences_struct* user_preferences, int force_completion) {
	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, SET_REFRESH_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Specify auto-refresh interval.\n");
	custom_print(':', "[Input Format] Digit Minutes\n");
	if (!force_completion) {
		custom_print(':', "%s\n", BACK_TITLE);
	}
	print_divider(get_screen_dims('x'), "full");

	while (1) {
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (!force_completion && confirmation_back(input_buffer)) {
			return 0;
		}

		if (!verify_string_is_uint(input_buffer)) {
			custom_print('!', "Incorrect format. (Parameter format)\n");
			continue;
		}

		if (!confirmation_yes_no(1)) {
			continue;
		}

		user_preferences->refresh_interval = atoi(input_buffer);

		break;
	}

	custom_print('>', "Saved: %d minute(s)\n", user_preferences->refresh_interval);
	press_any();

	return 1;
}

// Handles tierdisplay command
u_int process_tierdisplay(user_preferences_struct* user_preferences, int force_completion) {
	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, CHOOSE_DISPLAY_TIER_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Select tiers to display.\n");
	custom_print('i', "Following completion, alert tiers will be reset.\n");
	custom_print(':', "[Input Format]: Comma separated string (no., no., ...)\n");
	custom_print(':', "Accepted Inputs: \n");
	for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
		if (level_array[i][0] != '-') {
			custom_print(':', "%d (%s)\n", i, level_array[i]);
		}
	}
	if (!force_completion) {
		custom_print(':', "%s\n", BACK_TITLE);
	}
	print_divider(get_screen_dims('x'), "full");

	while (1) {
		int tier_store_array[LEVEL_ARRAY_SIZE] = { {0} };
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (!force_completion && confirmation_back(input_buffer)) {
			return 0;
		}

		// Extract the first token
		char* token = strtok(input_buffer, ",");
		// loop through the string to extract all other tokens
		while (token != NULL) {
			if (verify_string_is_uint(token)) {
				int chosen_level = atoi(token);
				if (chosen_level >= 0 && chosen_level < LEVEL_ARRAY_SIZE && level_array[atoi(token)][0] != '-') {
					tier_store_array[atoi(token)] = 1;
				}
			}

			token = strtok(NULL, ",");
		}

		char final_buffer[64] = "";

		for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
			if (tier_store_array[i] == 1) {
				char temp_buffer[8];
				snprintf(temp_buffer, 8, "\"%d\",", i);
				strcat(final_buffer, temp_buffer);
			}
		}

		if (strlen(final_buffer) == 0) {
			custom_print('!', "No valid inputs found. (Parameter format)\n");
			continue;
		}

		custom_print('>', "Detected Inputs: <");
		for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
			if (tier_store_array[i] == 1) {
				custom_print('0', "%s,", level_array[i]);
			}
		}
		custom_print('0', "\b>\n");


		if (!confirmation_yes_no(1)) {
			continue;
		}

		// Removes final ,
		final_buffer[strlen(final_buffer) - 1] = '\0';
		strcpy(user_preferences->display_tiers, final_buffer);

		// Reset alert tiers
		strcpy(user_preferences->alert_tiers, "");

		break;
	}

	custom_print('>', "Saved: Tiers %s\n", user_preferences->display_tiers);
	press_any();

	return 1;
}

// Handles tieralert command
u_int process_tieralert(user_preferences_struct* user_preferences, int force_completion) {
	int allowed_tier_array[LEVEL_ARRAY_SIZE] = { {0} };

	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, CHOOSE_ALERT_TIER_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Select tiers to ping.\n");
	custom_print(':', "[Input Format] Comma separated string (no., no., ...)\n");
	custom_print(':', "Accepted Inputs: \n");
	for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
		char temp_buffer[8];
		snprintf(temp_buffer, 8, "\"%d\"", i);

		if (level_array[i][0] != '-' && strstr(user_preferences->display_tiers, temp_buffer) != NULL) {
			custom_print(':', "%d (%s)\n", i, level_array[i]);
			allowed_tier_array[i] = 1;
		}
	}
	if (!force_completion) {
		custom_print(':', "%s\n", BACK_TITLE);
	}
	print_divider(get_screen_dims('x'), "full");

	while (1) {
		int tier_store_array[LEVEL_ARRAY_SIZE] = { {0} };
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (!force_completion && confirmation_back(input_buffer)) {
			return 0;
		}

		// Extract the first token
		char* token = strtok(input_buffer, ",");
		// loop through the string to extract all other tokens
		while (token != NULL) {
			if (verify_string_is_uint(token)) {
				int chosen_level = atoi(token);
				if (chosen_level >= 0 && chosen_level < LEVEL_ARRAY_SIZE && level_array[atoi(token)][0] != '-' && allowed_tier_array[atoi(token)] == 1) {
					tier_store_array[atoi(token)] = 1;
				}
			}

			token = strtok(NULL, ",");
		}

		char final_buffer[64] = "";

		for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
			if (tier_store_array[i] == 1) {
				char temp_buffer[8];
				snprintf(temp_buffer, 8, "\"%d\",", i);
				strcat(final_buffer, temp_buffer);
			}
		}

		if (strlen(final_buffer) == 0) {
			custom_print('>', "No valid inputs found. Are you sure you do not want any alerts?\n");
		}
		else {
			custom_print('>', "Detected Inputs: <");
			for (int i = 0; i < LEVEL_ARRAY_SIZE; i++) {
				if (tier_store_array[i] == 1) {
					custom_print('0', "%s,", level_array[i]);
				}
			}
			custom_print('0', "\b>\n");
		}

		if (!confirmation_yes_no(1)) {
			continue;
		}

		if (strlen(final_buffer) == 0) {
			strcpy(user_preferences->alert_tiers, "NA");
		}
		else {
			// Removes final ,
			final_buffer[strlen(final_buffer) - 1] = '\0';
			strcpy(user_preferences->alert_tiers, final_buffer);
		}

		break;
	}

	custom_print('>', "Saved: Tiers %s\n", user_preferences->alert_tiers);
	press_any();

	return 1;
}

// Handles displaymode command
u_int process_displaymode(user_preferences_struct* user_preferences, int force_completion) {
	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, CHOOSE_DISPLAY_MODE_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Select gym information granularity: Minimal/Verbose Mode.\n");
	custom_print(':', "[Input Format] Word (min OR max)\n");
	if (!force_completion) {
		custom_print(':', "%s\n", BACK_TITLE);
	}
	print_divider(get_screen_dims('x'), "full");

	while (1) {
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (!force_completion && confirmation_back(input_buffer)) {
			return 0;
		}

		if (strcmp(input_buffer, "min") && strcmp(input_buffer, "max")) {
			custom_print('!', "Incorrect format. (Parameter format)\n");
			continue;
		}

		if (!confirmation_yes_no(1)) {
			continue;
		}

		if (!strcmp(input_buffer, "min")) {
			user_preferences->display_mode = DISPLAY_MODE_MINIMAL;
		}
		else {
			user_preferences->display_mode = DISPLAY_MODE_VERBOSE;
		}

		break;
	}

	custom_print('>', "Saved: Mode %s\n", user_preferences->display_mode == DISPLAY_MODE_MINIMAL ? "Minimal" : "Verbose");
	press_any();

	return 1;
}

// Handles reset command
void process_reset(user_preferences_struct* user_preferences) {
	CLEAR_TERMINAL;

	print_divider(get_screen_dims('x'), "full");
	print_in_center(get_screen_dims('x'), 1, RESET_USER_PREFS_TITLE);
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "This will reset your user preferences file and prompt a full re-entry.\n");
	print_divider(get_screen_dims('x'), "full");

	if (!confirmation_yes_no(0)) {
		return;
	}

	int error_code = remove_file(FILENAME_PREFS);
	if (error_code) {
		custom_print('!', "Could not delete file. Error code: %d\n", error_code);
		return;
	}

	create_user_prefs_from_scratch(user_preferences);
}

// Handles gyms command
u_int process_gyms(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, char* raid_json_string) {
	// Init all_gyms linked list
	char* gym_string = (char*)malloc_safe(TWOFIVESIX_KILOBYTES * sizeof(char));
	int read_count = 0;
	read_file_to_buffer_generic(gym_string, FILENAME_ALLGYMS);
	char** gym_string_array = read_gym_string(gym_string, &read_count);
	free(gym_string);
	gym_node* all_gym_linked_list_head = NULL;
	init_gym_linked_list(read_count, gym_string_array, &all_gym_linked_list_head);
	for (int i = 0; i < read_count; i++)
	{
		free(gym_string_array[i]);
	}
	free(gym_string_array);

	// Init current_active_gyms linked list
	gym_node* current_active_gym_linked_list_head = NULL;
	raid_string_to_linked_list(&current_active_gym_linked_list_head, raid_json_string);

	int must_print_header = 1;
	int changes_to_user_gyms = 0;

	while (1) {
		if (must_print_header) {
			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			print_in_center(get_screen_dims('x'), 1, CHOOSE_GYM_TITLE);
			print_divider(get_screen_dims('x'), "full");

			custom_print('i', "%s", PROCESS_GYM_TITLE_0);
			SET_IN_FOR; custom_print('0', " [%s %s*]\n", PROCESS_GYM_ADD_ARG_0, PROCESS_GYM_ADD_ARG_2);

			custom_print('i', "%s", PROCESS_GYM_TITLE_1);
			SET_IN_FOR; custom_print('0', " [%s %s %s %s*]\n", PROCESS_GYM_ADD_ARG_0, PROCESS_GYM_ADD_ARG_1, PROCESS_GYM_NUM_ARG, PROCESS_GYM_ADD_ARG_2);

			custom_print('i', "*Optional argument that read current active gyms in specified radius instead of from masterlist.\n");

			custom_print('i', "*Use this command if you can't find your desired gym.\n");

			custom_print('i', "%s", PROCESS_GYM_TITLE_2);
			SET_IN_FOR; custom_print('0', " [%s %s]\n", PROCESS_GYM_DEL_ARG_0, PROCESS_GYM_NUM_ARG);

			custom_print('i', "%s", PROCESS_GYM_TITLE_3);
			SET_IN_FOR; custom_print('0', " [%s %s]\n", PROCESS_GYM_DEL_ARG_0, PROCESS_GYM_DEL_ARG_1);

			custom_print('i', "%s", PROCESS_GYM_TITLE_4);
			SET_IN_FOR; custom_print('0', " [%s %s %s]\n", PROCESS_GYM_SWAP_ARG_0, PROCESS_GYM_NUM_ARG, PROCESS_GYM_NUM_ARG);

			custom_print(':', "%s\n", BACK_TITLE);

			print_divider(get_screen_dims('x'), "full");
			gym_list_print_all(user_gym_linked_list_head, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);
			print_divider(get_screen_dims('x'), "full");

			must_print_header = 0;
		}

		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (confirmation_back(input_buffer)) {
			break;
		}

		// Extract up to 4 tokens
		char* token[4] = { strtok(input_buffer, " "), strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " ") };

		// There is some redundant checking here, for readability, idk improve whenever
		if (token[0] != NULL && !strcmp(token[0], PROCESS_GYM_ADD_ARG_0)) {
			must_print_header = 1;

			// add -pos <num> -curr
			if (token[1] != NULL && !strcmp(token[1], PROCESS_GYM_ADD_ARG_1) &&
				token[2] != NULL && verify_string_is_uint(token[2]) &&
				token[3] != NULL && !strcmp(token[3], PROCESS_GYM_ADD_ARG_2)) {
				if (process_gyms_add_curr(user_preferences, user_gym_linked_list_head, &current_active_gym_linked_list_head, atoi(token[2]))) {
					changes_to_user_gyms = 1;
				}
			}
			// add -pos <num>
			else if (token[1] != NULL && !strcmp(token[1], PROCESS_GYM_ADD_ARG_1) &&
				token[2] != NULL && verify_string_is_uint(token[2]) &&
				token[3] == NULL) {
				if (process_gyms_add(user_preferences, user_gym_linked_list_head, &all_gym_linked_list_head, atoi(token[2]))) {
					changes_to_user_gyms = 1;
				}
			}
			// add -curr
			else if (token[1] != NULL && !strcmp(token[1], PROCESS_GYM_ADD_ARG_2) &&
				token[2] == NULL &&
				token[3] == NULL) {
				if (process_gyms_add_curr(user_preferences, user_gym_linked_list_head, &current_active_gym_linked_list_head, 0)) {
					changes_to_user_gyms = 1;
				}
			}
			// add
			else if (token[1] == NULL &&
				token[2] == NULL &&
				token[3] == NULL) {
				if (process_gyms_add(user_preferences, user_gym_linked_list_head, &all_gym_linked_list_head, 0)) {
					changes_to_user_gyms = 1;
				}
			}
			// explosion.mp4
			else {
				custom_print('!', "No valid inputs found.\n");
				must_print_header = 0;
			}
		}
		else if (token[0] != NULL && !strcmp(token[0], PROCESS_GYM_DEL_ARG_0) &&
			token[1] != NULL && verify_string_is_uint(token[1])) {
			if (process_gyms_delete(user_gym_linked_list_head, atoi(token[1]))) {
				changes_to_user_gyms = 1;
			}

			must_print_header = 1;
		}
		else if (token[0] != NULL && !strcmp(token[0], PROCESS_GYM_DEL_ARG_0) &&
			token[1] != NULL && !strcmp(token[1], PROCESS_GYM_DEL_ARG_1)) {
			if (process_gyms_delete(user_gym_linked_list_head, 0)) {
				changes_to_user_gyms = 1;
			}

			must_print_header = 1;
		}
		else if (token[0] != NULL && !strcmp(token[0], PROCESS_GYM_SWAP_ARG_0) &&
			token[1] != NULL && verify_string_is_uint(token[1]) &&
			token[2] != NULL && verify_string_is_uint(token[2])) {
			if (process_gyms_swap(user_gym_linked_list_head, atoi(token[1]), atoi(token[2]))) {
				changes_to_user_gyms = 1;
			}

			must_print_header = 1;
		}
		else {
			custom_print('!', "No valid inputs found.\n");
		}
	}

	gym_list_destroy(&all_gym_linked_list_head);
	gym_list_destroy(&current_active_gym_linked_list_head);

	return changes_to_user_gyms;
}

// Handles add command (within gyms)
// Deals with both end of list insertion and index insertion
u_int process_gyms_add(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, gym_node** all_gym_linked_list_head, u_int insertion_index) {
	int must_print_header = 1;
	int return_code = 0;

	while (1) {
		if (must_print_header) {
			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			if (insertion_index == 0) {
				print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_0);
			}
			else {
				print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_1);
			}
			print_divider(get_screen_dims('x'), "full");
			custom_print(':', "Enter name of gym. (Full name)\n");
			custom_print(':', "%s\n", BACK_TITLE);
			print_divider(get_screen_dims('x'), "full");

			must_print_header = 0;
		}

		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (confirmation_back(input_buffer)) {
			break;
		}

		gym_node* searched_gyms_linked_list_head = gym_list_subset_by_name(all_gym_linked_list_head, input_buffer);
		if (searched_gyms_linked_list_head == NULL) {
			custom_print('!', "%s\n", NODE_MSG_ARR[NODE_NOT_FOUND]);
		}
		else {
			must_print_header = 1;

			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_0_A);
			print_divider(get_screen_dims('x'), "full");
			custom_print(':', "[Input Format] Index Number (Singular)\n");
			custom_print(':', "(There may be duplicates. In that event, choose based on distance offsets provided.)\n");
			custom_print(':', "%s\n", BACK_TITLE);
			print_divider(get_screen_dims('x'), "full");
			gym_list_print_all(&searched_gyms_linked_list_head, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);
			print_divider(get_screen_dims('x'), "full");

			while (1) {
				int chosen_index = 0;
				input_buffer[0] = '\0';
				receive_input(input_buffer);

				if (confirmation_back(input_buffer)) {
					break;
				}

				if (verify_string_is_uint(input_buffer)) {
					chosen_index = atoi(input_buffer);
					custom_print('>', "Chose %d.\n", chosen_index);

					if (insertion_index > 0) {
						custom_print('>', "This will place this gym after index %d.\n", insertion_index);
					}
				}
				else {
					custom_print('!', "Incorrect format. (Parameter format)\n");
					continue;
				}

				if (!confirmation_yes_no(1)) {
					continue;
				}

				gym_node* gym_node_chosen_index = gym_node_get_index(&searched_gyms_linked_list_head, chosen_index);

				if (gym_node_chosen_index != NULL) {
					if (insertion_index > 0) {
						u_int node_code = gym_node_insert_after_index(user_gym_linked_list_head, insertion_index, gym_node_chosen_index->lat, gym_node_chosen_index->lng, gym_node_chosen_index->gym_name);
						if (node_code > NODE_SUCCESS) {
							custom_print('!', "%s\n", NODE_MSG_ARR[node_code]);

							press_any();
							break;
						}
					}
					else {
						u_int node_code = gym_node_append(user_gym_linked_list_head, gym_node_chosen_index->lat, gym_node_chosen_index->lng, gym_node_chosen_index->gym_name);
						if (node_code > NODE_SUCCESS) {
							custom_print('!', "%s\n", NODE_MSG_ARR[node_code]);
							continue;
						}
					}

					custom_print('>', "Added: ");
					gym_node_print(0, gym_node_chosen_index, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);

					return_code = 1;

					press_any();
					break;
				}
				else {
					custom_print('!', "%s\n", NODE_MSG_ARR[NODE_INVALID_INDEX]);
					continue;
				}
			}

			gym_list_destroy(&searched_gyms_linked_list_head);
			// We don't want it to loop if it is insertion after index
			if (insertion_index > 0) {
				break;
			}
		}
	}

	return return_code;
}

// Handles add -curr command (within gyms)
// Deals with both end of list insertion and index insertion
u_int process_gyms_add_curr(user_preferences_struct* user_preferences, gym_node** user_gym_linked_list_head, gym_node** current_active_gym_linked_list_head, u_int insertion_index) {
	int must_print_header = 1;
	int return_code = 0;

	while (1) {
		if (must_print_header) {
			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			if (insertion_index == 0) {
				print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_0);
			}
			else {
				print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_1);
			}
			print_divider(get_screen_dims('x'), "full");
			custom_print(':', "Enter radius of active gyms to show.\n");
			custom_print(':', "[Input Format] Meters in Integer\n");
			custom_print(':', "%s\n", BACK_TITLE);
			print_divider(get_screen_dims('x'), "full");

			must_print_header = 0;
		}

		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (confirmation_back(input_buffer)) {
			break;
		}

		if (!verify_string_is_uint(input_buffer)) {
			custom_print('!', "Incorrect format. (Parameter format)\n");
			continue;
		}

		gym_node* in_range_gyms_linked_list_head = gym_list_subset_by_range(current_active_gym_linked_list_head, strtod(input_buffer, NULL), user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);
		if (in_range_gyms_linked_list_head == NULL) {
			custom_print('!', "%s\n", NODE_MSG_ARR[NODE_NOT_FOUND]);
		}
		else {
			must_print_header = 1;

			CLEAR_TERMINAL;

			print_divider(get_screen_dims('x'), "full");
			print_in_center(get_screen_dims('x'), 1, PROCESS_GYM_TITLE_0_A);
			print_divider(get_screen_dims('x'), "full");
			custom_print(':', "[Input Format] Index Number (Singular)\n");
			custom_print(':', "(There may be duplicates. In that event, choose based on distance offsets provided.)\n");
			custom_print(':', "%s\n", BACK_TITLE);
			print_divider(get_screen_dims('x'), "full");
			gym_list_print_all(&in_range_gyms_linked_list_head, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);
			print_divider(get_screen_dims('x'), "full");

			while (1) {
				int chosen_index = 0;
				input_buffer[0] = '\0';
				receive_input(input_buffer);

				if (confirmation_back(input_buffer)) {
					break;
				}

				if (verify_string_is_uint(input_buffer)) {
					chosen_index = atoi(input_buffer);
					custom_print('>', "Chose %d.\n", chosen_index);

					if (insertion_index > 0) {
						custom_print('>', "This will place this gym after index %d.\n", insertion_index);
					}
				}
				else {
					custom_print('!', "Incorrect format. (Parameter format)\n");
					continue;
				}

				if (!confirmation_yes_no(1)) {
					continue;
				}

				gym_node* gym_node_chosen_index = gym_node_get_index(&in_range_gyms_linked_list_head, chosen_index);

				if (gym_node_chosen_index != NULL) {
					if (insertion_index > 0) {
						u_int node_code = gym_node_insert_after_index(user_gym_linked_list_head, insertion_index, gym_node_chosen_index->lat, gym_node_chosen_index->lng, gym_node_chosen_index->gym_name);
						if (node_code > NODE_SUCCESS) {
							custom_print('!', "%s\n", NODE_MSG_ARR[node_code]);

							press_any();
							break;
						}
					}
					else {
						u_int node_code = gym_node_append(user_gym_linked_list_head, gym_node_chosen_index->lat, gym_node_chosen_index->lng, gym_node_chosen_index->gym_name);
						if (node_code > NODE_SUCCESS) {
							custom_print('!', "%s\n", NODE_MSG_ARR[node_code]);
							continue;
						}
					}

					custom_print('>', "Added: ");
					gym_node_print(0, gym_node_chosen_index, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1]);

					return_code = 1;

					press_any();
					break;
				}
				else {
					custom_print('!', "%s\n", NODE_MSG_ARR[NODE_INVALID_INDEX]);
					continue;
				}
			}

			gym_list_destroy(&in_range_gyms_linked_list_head);
			// We don't want it to loop if it is insertion after index
			if (insertion_index > 0) {
				break;
			}
		}
	}

	return return_code;
}

// Handles delete command (within gyms)
u_int process_gyms_delete(gym_node** user_gym_linked_list_head, u_int deletion_index) {
	// delete specific
	if (deletion_index > 0) {
		custom_print('>', "This will delete the gym at index %d.\n", deletion_index);
	}
	// delete all
	else {
		custom_print('>', "This will delete ALL gyms from your list.\n");
	}

	if (!confirmation_yes_no(0)) {
		return 0;
	}

	if (deletion_index > 0) {
		u_int node_code = gym_node_delete_index(user_gym_linked_list_head, deletion_index);
		if (node_code > NODE_SUCCESS) {
			custom_print('!', "%s\n", NODE_MSG_ARR[node_code]);
			return 0;
		}
	}
	else {
		gym_list_destroy(user_gym_linked_list_head);
	}

	return 1;
}

// Handles swap command (within gyms)
u_int process_gyms_swap(gym_node** user_gym_linked_list_head, u_int index_1, u_int index_2) {
	custom_print('>', "This will swap indexes %d and %d.\n", index_1, index_2);

	if (!confirmation_yes_no(0)) {
		return 0;
	}

	u_int node_code = gym_node_swap_index(user_gym_linked_list_head, index_1, index_2);
	if (node_code > NODE_SUCCESS) {
		custom_print('>', "%s\n", NODE_MSG_ARR[node_code]);
		return 0;
	}

	return 1;
}

// Handles updatemaster command
u_int process_update_master() {
	custom_print('>', "This will force a redownload of game master files on next refresh.\n");
	custom_print('i', "Includes: Pokemon, Moves, Forms.\n");

	if (!confirmation_yes_no(0)) {
		return 0;
	}

	return 1;
}

// Handles updatelist command
u_int process_update_gym_list() {
	custom_print('>', "This will force a redownload of the master gym list on next refresh.\n");
	custom_print('i', "[WARNING] This is a potentially destructive action if not conducted at the tail end of a raid hour!\n");
	custom_print('i', "[WARNING] Doing this may cause some gyms to no longer be present in the master file!\n");

	if (!confirmation_yes_no(0)) {
		return 0;
	}

	custom_print(':', "Are you really sure?\n");

	if (!confirmation_yes_no(0)) {
		return 0;
	}

	return 1;
}

// Helper function:
// Prompts the user to confirm their choice.
// If prompt_again is set, it will also prompt the user to enter again.
// Returns 0 if user selected 'n', 1 if user selected 'y'.
int confirmation_yes_no(int prompt_again) {
	custom_print(':', "Is this okay? (Enter y/n)\n");

	while (1) {
		char input_buffer[UNIVERSAL_LENGTH] = "";
		receive_input(input_buffer);

		if (strlen(input_buffer) != 1 || !(tolower(input_buffer[0]) == 'y' || tolower(input_buffer[0]) == 'n')) {
			custom_print('!', "Invalid format.\n");
			continue;
		}

		if (tolower(input_buffer[0]) == 'y') {
			return 1;
		}
		else {
			if (prompt_again) {
				custom_print(':', "Enter Again:\n");
			}

			return 0;
		}
	}
}

// Helper function:
// Checks if a given buffer value matches the "back" command.
// Returns 0 if no match, 1 if match.
int confirmation_back(char* buffer) {
	return !strcmp(buffer, BACK_COMMAND);
}

// Helper function:
// Stores a user's input to the specified buffer.
// Writes up to UNIVERSAL_LENGTH characters.
void receive_input(char* input_buffer) {
	custom_print('<', "");

	input_buffer[0] = '\0';
	fgets(input_buffer, UNIVERSAL_LENGTH, stdin);

	if (strchr(input_buffer, '\n')) {			// check exist newline
		*strchr(input_buffer, '\n') = '\0';	// replace newline with null terminator
	}
	else {
		scanf("%*[^\n]");	// clear up to newline
		scanf("%*c");		// clear newline
	}
}

// Helper function:
// Checks if a given buffer value contains only digits. (not even decimals or negatives)
// Not really uint but whatevs~
// Returns 0 if isnt uint, 1 if is.
int verify_string_is_uint(char* string) {
	if (strlen(string) == 0)
		return 0;

	for (int i = 0; i < strlen(string); i++) {
		if (!isdigit(string[i])) {
			return 0;
		}
	}

	return 1;
}
