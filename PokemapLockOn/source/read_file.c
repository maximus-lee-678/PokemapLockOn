#include "../headers/read_file.h"
#include "../headers/user_prefs.h"

// Legacy function for counting new lines
int count_new_lines(FILE* fp) {
	int c;	// EOF could be outside the range of char
	int lines = 0;

	if (fp == NULL) {
		return -1;
	}

	while (!feof(fp))
	{
		c = fgetc(fp);
		if (c == '\n')
		{
			lines++;
		}
	}

	fseek(fp, 0, SEEK_SET);

	return lines;
}

// Returns 0 if file_path was not found, 1 if found.
u_int check_if_file_exists(char* source_file_path) {
	FILE* read_file;

	read_file = fopen(source_file_path, "r");
	if (read_file == NULL) {
		return 0;
	}
	else {
		fclose(read_file);
		return 1;
	}
}

// Supply this with a Char Array ptr and the DEFINE filepath. Filepath -> Char Array
// Returns bytes read
int read_file_to_buffer_generic(char* destination_buffer, char* source_file_path) {
	FILE* read_file;

	read_file = fopen(source_file_path, "r");
	if (read_file == NULL) {
		custom_print('!', "Could not open file %s.\n", source_file_path);
		return -1;
	}
	else {
		// Because of \r\n, we cant use good ol
		// fseek(read_file, 0, SEEK_END) and ftell and fread

		// Extract characters from file and store in character c
		long index = 0;
		for (char c = getc(read_file); c != EOF; c = getc(read_file)) {
			destination_buffer[index++] = c;
		}
		destination_buffer[index] = '\0';

		fclose(read_file);
		return (int) index;
	}
}

// Supply this with a Char Array ptr and the DEFINE filepath. Char Array -> Filepath
// Works for new file creation or updating a file (it blows up the old file)
// Returns bytes written
int write_file_generic(char* writable_string, char* destination_file_path) {
	FILE* file_open = fopen(destination_file_path, "w");
	if (file_open == NULL) {
		custom_print('!', "Could not create file %s.\n", destination_file_path);
		return -1;
	}
	else {
		fwrite(writable_string, 1, strlen(writable_string), file_open);

		fclose(file_open);
		return sizeof(writable_string);
	}
}

// Stringifys provided user_preferences_struct for writing to file,
// and writes to file
void write_to_file_user_preferences(user_preferences_struct* user_preferences) {
	char prepared_statement_buffer[UNIVERSAL_LENGTH] = "";

	snprintf(prepared_statement_buffer, UNIVERSAL_LENGTH, "%s=%f,%f\n%s=%d\n%s=%s\n%s=%s\n%s=%d\n",
		SET_HOME_COMMAND, user_preferences->home_coordinates[0], user_preferences->home_coordinates[1],
		SET_REFRESH_COMMAND, user_preferences->refresh_interval,
		CHOOSE_DISPLAY_TIER_COMMAND, user_preferences->display_tiers,
		CHOOSE_ALERT_TIER_COMMAND, user_preferences->alert_tiers,
		CHOOSE_DISPLAY_MODE_COMMAND, user_preferences->display_mode
	);

	write_file_generic(prepared_statement_buffer, FILENAME_PREFS);
}

// Stringifys provided gym_node linked list for writing to file,
// and writes to file
void write_to_file_gyms(gym_node** user_gym_linked_list_head, char* destination_file_path) {
	char* gym_string = (char*)malloc_safe(TWOFIVESIX_KILOBYTES * sizeof(char));
	gym_string[0] = '\0';
	char prepared_statement_buffer[UNIVERSAL_LENGTH] = "";
	gym_node* iter_node = *user_gym_linked_list_head;

	while (iter_node != NULL) {
		snprintf(prepared_statement_buffer, UNIVERSAL_LENGTH, "\"lat\":%s,\"lng\":%s,\"gym_name\":\"%s\"\n",
			iter_node->lat, iter_node->lng, iter_node->gym_name);
		strcat(gym_string, prepared_statement_buffer);
		iter_node = iter_node->next;
	}

	write_file_generic(gym_string, destination_file_path);
	free(gym_string);
}

int remove_file(char* remove_file_path) {
	int ret = remove(remove_file_path);
	int error_code = 0;
	if (ret != 0) {
		error_code = errno;
		errno = 0;
	}

	return error_code;
}