#include "../headers/read_string.h"

// Supply this function with a int pointer, it will write number of lines to it
// And return a char** array with file strings
char** read_gym_string(char* gym_string, int* number_of_lines) {
	char* examiner = gym_string;

	// count \n
	while (1) {
		examiner = strstr(examiner + 1, "\n");

		if (examiner != NULL) {
			(*number_of_lines)++;
		}
		else {
			break;
		}		
	}

	// malloc based on number_of_lines
	char** gym_string_array = (char**)malloc(*number_of_lines * sizeof(char*));
	for (int i = 0; i < *number_of_lines; i++)
	{
		gym_string_array[i] = (char*)malloc(UNIVERSAL_LENGTH * sizeof(char));
	}

	// Reset reading pointer to front
	examiner = gym_string;
	int str_length = 0;

	for (int i = 0; i < *number_of_lines; i++) {
		str_length = (int)(strstr(examiner, "\n") - examiner);

		strncpy(gym_string_array[i], examiner, str_length);	// Copy
		gym_string_array[i][str_length] = '\0';				// Provide a terminator

		examiner = strstr(examiner, "\n");					// Move to next line
		examiner++;
	}

	return gym_string_array;
}

void init_gym_linked_list(int total, char** gym_string_array, gym_node** gym_linked_list_head) {
	for (int i = 0; i < total; i++) {
		char temp_lat[32] = "", temp_lng[32] = "", temp_gym_name[UNIVERSAL_LENGTH] = "";

		stringcopy_named_json_value(temp_lat, gym_string_array[i], "lat");
		stringcopy_named_json_value(temp_lng, gym_string_array[i], "lng");
		stringcopy_named_json_value(temp_gym_name, gym_string_array[i], "gym_name");

		gym_node_append(gym_linked_list_head, temp_lat, temp_lng, temp_gym_name);
	}
}

void init_raid_details_struct(gym_node** gym_linked_list_head, raid_details_struct* array_raid_details) {
	gym_node* iter_node = *gym_linked_list_head;
	int array_index = 0;
	while (iter_node != NULL) {
		strcpy(array_raid_details[array_index].lat, iter_node->lat);
		strcpy(array_raid_details[array_index].lng, iter_node->lng);
		strcpy(array_raid_details[array_index].gym_name, iter_node->gym_name);
		iter_node = iter_node->next;
		array_index++;
	}
}

// Pulls various values from raids.php, usually not human readable
void populate_basic_fields(int total, char* raid_json_string, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		char* examiner = NULL;
		// Formatting to "lat":%s,"lng":%s
		char temp_buffer[64] = "";
		snprintf(temp_buffer, 64, "\"lat\":%s,\"lng\":%s", array_raid_details[i].lat, array_raid_details[i].lng);

		examiner = strstr(raid_json_string, temp_buffer);

		// If lat lng not found in json, set value that signals all subsequent functions to not run checks
		if (examiner == NULL) {
			array_raid_details[i].has_active_raid = 0;
			continue;
		}

		array_raid_details[i].has_active_raid = 1;

		// The way the JSON works: unhatched gyms have one entry, hatched have two
		// The second one in line contains full info about hatched pokemon
		if (strstr(examiner + 1, temp_buffer) != NULL) {
			examiner = strstr(examiner + 1, temp_buffer);
		}

		// Seek to start of json object: {
		do {
			examiner--;
		} while (examiner[0] != '{');

		// Wall of Text has awoken!
		stringcopy_named_json_value(array_raid_details[i].cell_id, examiner, "cell_id");
		stringcopy_named_json_value(array_raid_details[i].ex_raid_eligible, examiner, "ex_raid_eligible");
		stringcopy_named_json_value(array_raid_details[i].sponsor, examiner, "sponsor");
		stringcopy_named_json_value(array_raid_details[i].raid_spawn, examiner, "raid_spawn");
		stringcopy_named_json_value(array_raid_details[i].raid_start, examiner, "raid_start");
		stringcopy_named_json_value(array_raid_details[i].raid_end, examiner, "raid_end");
		stringcopy_named_json_value(array_raid_details[i].pokemon_id, examiner, "pokemon_id");
		stringcopy_named_json_value(array_raid_details[i].level, examiner, "level");
		stringcopy_named_json_value(array_raid_details[i].cp, examiner, "cp");
		stringcopy_named_json_value(array_raid_details[i].team, examiner, "team");
		stringcopy_named_json_value(array_raid_details[i].move1, examiner, "move1");
		stringcopy_named_json_value(array_raid_details[i].move2, examiner, "move2");
		stringcopy_named_json_value(array_raid_details[i].is_exclusive, examiner, "is_exclusive");
		stringcopy_named_json_value(array_raid_details[i].form, examiner, "form");
		stringcopy_named_json_value(array_raid_details[i].gender, examiner, "gender");
	}
}

// Populates level, team and gender values
void populate_from_local(int total, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			continue;
		}

		// If outta range, set to "Unknown"
		int level_id = atoi(array_raid_details[i].level);
		if (level_id >= 0 && level_id < LEVEL_ARRAY_SIZE) {
			strcpy(array_raid_details[i].readable_level, level_array[level_id]);
		}
		else {
			strcpy(array_raid_details[i].readable_level, "Unknown");
		}

		// If outta range, set to "Unknown"
		int team_id = atoi(array_raid_details[i].team);
		if (team_id >= 0 && team_id < TEAMS_ARRAY_SIZE) {
			strcpy(array_raid_details[i].readable_team, teams_array[team_id]);
		}
		else {
			strcpy(array_raid_details[i].readable_team, "Unknown");
		}

		// If outta range, set to "Unknown"
		int gender_id = atoi(array_raid_details[i].gender);
		if (gender_id >= 0 && gender_id < GENDER_ARRAY_SIZE) {
			strcpy(array_raid_details[i].readable_gender, gender_array[gender_id]);
		}
		else {
			strcpy(array_raid_details[i].readable_gender, "Unknown");
		}
	}
}

// Populates pokemon name
void populate_pokemon_name(int total, char* pokemon_json_string, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			continue;
		}

		// Returns pokemon id
		int pokemon_id = atoi(array_raid_details[i].pokemon_id);

		// Formatting to "i":"<id>"
		char temp_buffer[16] = "";
		snprintf(temp_buffer, 16, "\"i\":\"%d\"", pokemon_id);

		// If outta range, set to "Unknown"
		char* examiner = NULL;
		examiner = strstr(pokemon_json_string, temp_buffer);
		if (examiner != NULL) {
			stringcopy_named_json_value(array_raid_details[i].readable_pokemon, examiner, "n");
		}
		else {
			strcpy(array_raid_details[i].readable_pokemon, "Unknown (Not Unown)");
		}
	}
}

// Populates fast and charge moves
void populate_move_name(int total, char* moves_json_string, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			continue;
		}

		// Returns move id, -1 means unhatched, moves unknown
		// Value here used only to check if unhatched, original string used below
		int move1_id = atoi(array_raid_details[i].move1);
		if (move1_id <= -1) {
			strcpy(array_raid_details[i].readable_move1, "?");
			strcpy(array_raid_details[i].readable_move2, "?");
			continue;
		}

		// Formatting to "<id>"
		char temp_buffer[16] = "";
		snprintf(temp_buffer, 16, "\"%s\"", array_raid_details[i].move1);

		// If outta range, set to "Unknown"
		char* examiner = NULL;
		examiner = strstr(moves_json_string, temp_buffer);
		if (examiner != NULL) {
			stringcopy_adjacent_json_value(array_raid_details[i].readable_move1, examiner);
		}
		else {
			strcpy(array_raid_details[i].readable_move1, "?!");
		}

		/////////////////////////

		// Formatting to "<id>"
		snprintf(temp_buffer, 16, "\"%s\"", array_raid_details[i].move2);

		// If outta range, set to "Unknown"
		examiner = strstr(moves_json_string, temp_buffer);
		if (examiner != NULL) {
			stringcopy_adjacent_json_value(array_raid_details[i].readable_move2, examiner);
		}
		else {
			strcpy(array_raid_details[i].readable_move2, "?!");
		}
	}
}

// If form value cannot be found in file, its considered "Normal" form
// (That's gonna be a pain when determining what is out of date)
void populate_form_name(int total, char* forms_json_string, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			continue;
		}

		// Returns form id, 0 is "Normal" form
		int form_id = atoi(array_raid_details[i].form);
		if (form_id <= 0) {
			strcpy(array_raid_details[i].readable_form, "Normal");
			continue;
		}

		// Formatting to "<id>"
		char temp_buffer[16] = "";
		snprintf(temp_buffer, 16, "\"%s\"", array_raid_details[i].form);

		// If outta range, set to "Normal"
		char* examiner = NULL;
		examiner = strstr(forms_json_string, temp_buffer);
		if (examiner != NULL) {
			stringcopy_adjacent_json_value(array_raid_details[i].readable_form, examiner);
		}
		else {
			strcpy(array_raid_details[i].readable_form, "Normal");
		}
	}
}

// Finds the first occurence of specified json key from "source" and copies its value to "destination"
// (built for [: "])
void stringcopy_named_json_value(char* destination, char* source, char* json_key) {
	char* examiner = NULL;
	int str_length = 0;
	char temp_buffer[UNIVERSAL_LENGTH] = "";
	snprintf(temp_buffer, UNIVERSAL_LENGTH, "\"%s\"", json_key);	// surrounds key with "" (never forget P<lat>form)

	examiner = strstr(source, temp_buffer);
	examiner += (strlen(temp_buffer) + 1);			// moves pointer past key name and :

	if (examiner[0] == '"') {
		examiner++;														// skip a "
		str_length = (int)(strstr(examiner, "\"") - examiner);	// Measure length until before ",
	}
	else {
		str_length = (int)(strstr(examiner, ",") - examiner);		// Measure length until before ,
	}

	// prunes } at end, if any
	if (examiner[str_length - 1] == '}') {
		str_length--;
	}

	strncpy(destination, examiner, str_length);	// Copy
	destination[str_length] = '\0';						// Provide a terminator
}

u_int raid_string_to_linked_list(gym_node** head, char* raid_json_string) {
	char* examiner = raid_json_string;
	u_int count = 0;
	char temp_gym_name[512] = "", temp_lat[32] = "", temp_lng[32] = "";


	examiner = strstr(examiner + 1, "{\"gym_name\"");
	while (examiner != NULL) {
		stringcopy_named_json_value(temp_lat, examiner, "lat");
		stringcopy_named_json_value(temp_lng, examiner, "lng");
		stringcopy_named_json_value(temp_gym_name, examiner, "gym_name");

		gym_node_append(head, temp_lat, temp_lng, temp_gym_name);
		count++;

		examiner = strstr(examiner + 1, "{\"gym_name\"");
	}

	return count;
}

// Finds the first occurence of ANY json key from "source" and copies its value to "destination" 
// (built for [":])
void stringcopy_adjacent_json_value(char* destination, char* source) {
	char* examiner = NULL;
	int str_length = 0;

	examiner = strstr(source, "\": \"");
	examiner += strlen("\": \"");				// moves pointer past ": "

	str_length = (int)(strstr(examiner, "\",") - examiner);		// Measure length until before ",

	strncpy(destination, examiner, str_length);	// Copy
	destination[str_length] = '\0';						// Provide a terminator
}

// Finds occurence of a key from "source" and copies its value to "destination" 
// (built for config file string, [=])
void stringcopy_config_file(char* destination, char* source, char* key) {
	char* examiner = NULL;
	int str_length = 0;

	examiner = strstr(source, key);
	examiner += (strlen(key) + 1);				// moves pointer past key name and =

	str_length = (int)(strstr(examiner, "\n") - examiner);		// Measure length until before \n

	strncpy(destination, examiner, str_length);	// Copy
	destination[str_length] = '\0';						// Provide a terminator
}

// Intended for use after downloading script.js into buffer.
// Updates buffer to contain only pokemon information in json format.
void extract_pokemon_json(char* pokemon_json_string) {
	char* examiner = NULL;
	int str_length = 0;

	examiner = strstr(pokemon_json_string, "pokeArray = ");
	examiner += strlen("pokeArray = ");				// moves pointer past string

	str_length = (int)(strstr(examiner, ";") - examiner);		// Measure length until before ;

	strncpy(pokemon_json_string, examiner, str_length);	// Copy
	pokemon_json_string[str_length] = '\0';						// Provide a terminator
}