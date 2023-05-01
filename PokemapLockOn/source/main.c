#include "../headers/main.h"
#include "../headers/curl_operations.h"
#include "../headers/read_string.h"
#include "../headers/read_file.h"
#include "../headers/time_operations.h"
#include "../headers/user_interface.h"
#include "../headers/coordinates.h"
#include "../headers/user_prefs.h"
#include "../headers/thread_operations.h"

int main(void)
{
	// Trying to make all folders needed
	_mkdir(DIRECTORY_RESOURCE);
	_mkdir(DIRECTORY_CONFIG);

	// The declaration of independence
	char* raid_json_string = (char*)malloc_safe(TWO_MEGABYTES * sizeof(char));
	raid_json_string[0] = '\0';
	char* pokemon_json_string = (char*)malloc_safe(ONETWOEIGHT_KILOBYTES * sizeof(char));
	pokemon_json_string[0] = '\0';
	char* moves_json_string = (char*)malloc_safe(ONETWOEIGHT_KILOBYTES * sizeof(char));
	moves_json_string[0] = '\0';
	char* forms_json_string = (char*)malloc_safe(ONETWOEIGHT_KILOBYTES * sizeof(char));
	forms_json_string[0] = '\0';
	user_preferences_struct user_preferences = (user_preferences_struct){
		.home_coordinates = {0.0, 0.0},
		.refresh_interval = 0,
		.display_tiers = "NA",
		.alert_tiers = "NA",
		.display_mode = 0
	};
	gym_node* user_gym_linked_list_head = NULL;
	raid_details_struct* array_raid_details = (raid_details_struct*)malloc_safe(0);

	// Indicates which files needed to be updated
	u_int changes = 0b00000000;

	// Loading user prefs
	char* user_prefs_string = (char*)malloc_safe(EIGHT_KILOBYTES * sizeof(char));
	if (read_file_to_buffer_generic(user_prefs_string, FILENAME_PREFS) != -1) {
		load_user_prefs(&user_preferences, user_prefs_string);
	}
	else {
		create_user_prefs_from_scratch(&user_preferences);
	}
	free(user_prefs_string);
	// End Loading user prefs

	// Loading user gym file
	char* gym_string = (char*)malloc_safe(ONETWOEIGHT_KILOBYTES * sizeof(char));
	int read_count = 0;
	if (read_file_to_buffer_generic(gym_string, FILENAME_GYMS) != -1) {
		char** gym_string_array = read_gym_string(gym_string, &read_count);
		init_gym_linked_list(read_count, gym_string_array, &user_gym_linked_list_head);

		for (int i = 0; i < read_count; i++)
		{
			free(gym_string_array[i]);
		}
		free(gym_string_array);
	}
	else {
		raid_json_string[0] = '\0';
		get_webpage(PAGE_HOME, raid_json_string);

		create_user_gyms_from_scratch(&user_preferences, &user_gym_linked_list_head, raid_json_string);
	}
	free(gym_string);
	// End Loading user gym file

	// Loading master files
	if (read_file_to_buffer_generic(pokemon_json_string, FILENAME_POKEMON) == -1) {
		set_bit(&changes, BIT_POKEMON, 1);
	}
	if (read_file_to_buffer_generic(moves_json_string, FILENAME_MOVES) == -1) {
		set_bit(&changes, BIT_MOVES, 1);
	}
	if (read_file_to_buffer_generic(forms_json_string, FILENAME_FORMS) == -1) {
		set_bit(&changes, BIT_FORMS, 1);
	}

	while (1) {
		CLEAR_TERMINAL;

		// Must always load raid information
		raid_json_string[0] = '\0';
		get_webpage(PAGE_HOME, raid_json_string);

		// Prompted to redownload files, either user specified or file not found.
		if (get_bit(&changes, BIT_POKEMON)) {
			pokemon_json_string[0] = '\0';
			get_webpage(PAGE_POKEMON, pokemon_json_string);
			extract_pokemon_json(pokemon_json_string);
			write_file_generic(pokemon_json_string, FILENAME_POKEMON);

			set_bit(&changes, BIT_POKEMON, 0);
		}
		if (get_bit(&changes, BIT_MOVES)) {
			moves_json_string[0] = '\0';
			get_webpage(PAGE_MOVES, moves_json_string);
			write_file_generic(moves_json_string, FILENAME_MOVES);

			set_bit(&changes, BIT_MOVES, 0);
		}
		if (get_bit(&changes, BIT_FORMS)) {
			forms_json_string[0] = '\0';
			get_webpage(PAGE_FORMS, forms_json_string);
			write_file_generic(forms_json_string, FILENAME_FORMS);

			set_bit(&changes, BIT_FORMS, 0);
		}
		if (get_bit(&changes, BIT_GYMS)) {
			gym_node* temp_gym_linked_list_head = NULL;
			raid_string_to_linked_list(&temp_gym_linked_list_head, raid_json_string);
			write_to_file_gyms(&temp_gym_linked_list_head, FILENAME_ALLGYMS);
			gym_list_destroy(&temp_gym_linked_list_head);

			set_bit(&changes, BIT_FORMS, 0);
		}

		u_int gym_count = gym_list_count(&user_gym_linked_list_head);
		array_raid_details = (raid_details_struct*)realloc_safe(array_raid_details, gym_count * sizeof(raid_details_struct));
		init_raid_details_struct(&user_gym_linked_list_head, array_raid_details);

		populate_basic_fields(gym_count, raid_json_string, array_raid_details);
		populate_from_local(gym_count, array_raid_details);
		populate_pokemon_name(gym_count, pokemon_json_string, array_raid_details);
		populate_move_name(gym_count, moves_json_string, array_raid_details);
		populate_form_name(gym_count, forms_json_string, array_raid_details);
		populate_time_name(gym_count, array_raid_details);
		print_raid_details(gym_count, array_raid_details, &user_preferences);

		print_divider(get_screen_dims('x'), "full");
		char time_buf[UNIVERSAL_LENGTH] = "";
		stringcopy_formatted_time_string(time_buf, get_current_time());
		print_in_center(get_screen_dims('x'), 1, MSG_AUTO_REFRESH, user_preferences.refresh_interval, time_buf);
		print_divider(get_screen_dims('x'), "full");
		int sleep_return = handle_sleep_thread(user_preferences.refresh_interval * 60 * 1000);

		if (sleep_return == 1) {
			if (open_user_prefs_menu(&user_preferences, &user_gym_linked_list_head, &changes, raid_json_string)) {
				break;
			}
		}
	}

	// The uprooting of independence
	free(array_raid_details);
	gym_list_destroy(&user_gym_linked_list_head);
	free(raid_json_string);
	free(pokemon_json_string);
	free(moves_json_string);
	free(forms_json_string);

	press_any();

	return 0;
}
