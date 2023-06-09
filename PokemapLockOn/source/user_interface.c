#include "../headers/user_interface.h"
#include "../headers/time_operations.h"
#include "../headers/coordinates.h"

// =====
// Ended up not implementing weather, but if wanna do:
// Store weather data (raids.php)
// Match pokemon id and form to type (types_array)
// Match cell id to weather value
// Check if type is in corresponding cell id's weather array index (weather_affin_array)
// =====

// Main function that prints raid details.
// Examines array_raid_details and cross-references with user_preferences,
// instructing print_raid_info to actually perform the printing if the user has selected that tier to print.
// Also generates an alert if a new raid that matches a tier that the user has opted in to be alerted.
// Also changes console colour to match gym allegience.
void print_raid_details(int total, raid_details_struct* array_raid_details, user_preferences_struct* user_preferences) {
	u_int count_active = 0, count_inactive = 0, count_filtered_out = 0;
	static time_t latest_alert_time = 0;
	u_int must_alert = 0;

	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			count_inactive++;
			continue;
		}

		// Check if raid level is in display tiers
		char temp_buf[8] = "";
		snprintf(temp_buf, 8, "\"%s\"", array_raid_details[i].level);
		if (strstr(user_preferences->display_tiers, temp_buf) == NULL) {
			count_filtered_out++;
			continue;
		}

		// Add to active count
		count_active++;

		// Check if raid level is in alert tiers
		// If it is, check if its end time is greater than latest_alert_time, signifying a newer raid
		// of interest, and set must_alert
		if (strstr(user_preferences->alert_tiers, temp_buf) != NULL && strtol(array_raid_details[i].raid_end, NULL, 10) > latest_alert_time) {
			must_alert = 1;
			latest_alert_time = strtol(array_raid_details[i].raid_end, NULL, 10);
		}

		// if pokemon_id > 0, in progress
		switch (atoi(array_raid_details[i].team)) {
		case 0:
			SET_NORMAL;
			break;
		case 1:
			SET_MYSTIC_BLUE;
			break;
		case 2:
			SET_VALOR_RED;
			break;
		case 3:
			SET_INSTINCT_YELLOW;
			break;
		default:
			SET_NORMAL;
			break;
		}

		print_raid_info(array_raid_details[i], user_preferences->home_coordinates[0], user_preferences->home_coordinates[1], user_preferences->display_mode);

		SET_NORMAL;
	}
	print_divider(get_screen_dims('x'), "full");
	custom_print('i', "Active: %d, No raids: %d, Filtered out: %d\n", count_active, count_inactive, count_filtered_out);

	if (must_alert) {
		flash_alert();
	}
}

// Helper function:
// Called by print_raid_details, prints raid info at the provided index of array_raid_details.
// Prints in either Verbose or Minimal mode depending on display_mode's value.
void print_raid_info(raid_details_struct array_raid_details, double user_lat, double user_lng, u_int display_mode) {
	// forming time left string
	char time_left_string[UNIVERSAL_LENGTH] = "-";
	if (atoi(array_raid_details.pokemon_id)) {	// hatched
		stringcopy_formatted_time_difference(time_left_string, atol(array_raid_details.raid_end), get_current_time());
		strcat(time_left_string, " to END.");
	}
	else {	// hatching
		stringcopy_formatted_time_difference(time_left_string, atol(array_raid_details.raid_start), get_current_time());
		strcat(time_left_string, " to HATCH.");
	}

	// verbose mode
	if (display_mode == DISPLAY_MODE_VERBOSE) {
		// forming gym string
		char gym_string[UNIVERSAL_LENGTH] = "-";
		snprintf(gym_string, UNIVERSAL_LENGTH, "[%s] (%.1fm, %.1f%c)", array_raid_details.gym_name,
			get_lat_lon_dist_meters(user_lat, user_lng, strtod(array_raid_details.lat, NULL), strtod(array_raid_details.lng, NULL)),
			get_lat_lon_bearing_degrees(user_lat, user_lng, strtod(array_raid_details.lat, NULL), strtod(array_raid_details.lng, NULL)), DEG_CHAR);

		// forming time period string
		char time_start_end_string[UNIVERSAL_LENGTH] = "-";
		snprintf(time_start_end_string, UNIVERSAL_LENGTH, "%s - %s", array_raid_details.readable_raid_start, array_raid_details.readable_raid_end);

		// forming verbose pokemon details string
		char details_string[UNIVERSAL_LENGTH] = "-";
		if (atoi(array_raid_details.pokemon_id)) {	// hatched
			snprintf(details_string, UNIVERSAL_LENGTH, "CP: %s, Gender: %s, Form: %s", array_raid_details.cp, array_raid_details.readable_gender,
				atoi(array_raid_details.form) > 0 ? array_raid_details.readable_form : "-");
		}

		// newlines manually printed so console colour does not get reset
		print_divider(WIDTH_RAID_BOX, "full");
		print_in_center(WIDTH_RAID_BOX, 0, array_raid_details.readable_level);
		custom_print('0', "%s", gym_string); fprintf(stdout, "\n");
		print_in_center(WIDTH_RAID_BOX, 0, array_raid_details.readable_pokemon);
		custom_print('0', "%s", time_start_end_string); fprintf(stdout, "\n");
		print_in_center(WIDTH_RAID_BOX, 0, array_raid_details.readable_move1);
		custom_print('0', "%s", time_left_string); fprintf(stdout, "\n");
		print_in_center(WIDTH_RAID_BOX, 0, array_raid_details.readable_move2);
		custom_print('0', "%s", details_string); fprintf(stdout, "\n");
		print_divider(WIDTH_RAID_BOX, "full");
	}
	// minimal mode
	else {
		custom_print('0', "[%s] <<<%s>>> | %s | %s (%s, %s)\n",
			array_raid_details.gym_name, array_raid_details.readable_level, time_left_string,
			array_raid_details.readable_pokemon, array_raid_details.readable_move1, array_raid_details.readable_move2);
	}
}

// Prints a message in different colors based on symbol specified.
// Prints to stderr for '!', stdout otherwise.
// If no newline, color does not change back to normal.
// 'i'	info
// '>'	system operation
// '!'	error
// ':'	user input format
// '<'	user input
// '0'	dont print this symbol, maintain old colour
// '~'	only for 'press any key to continue'
void custom_print(char msg_type, const char* format, ...)
{
	switch (msg_type) {
	case 'i':
		SET_INFO;
		break;
	case '!':
		SET_ERR;
		break;
	case '>':
		SET_OUT;
		break;
	case '<':
		SET_IN;
		break;
	case ':':
		SET_IN;
		break;
	case '0':
		break;
	case '~':
		SET_NORMAL;
		break;
	default:
		SET_NORMAL;
		break;
	}

	if (msg_type != '0') {
		fprintf(stdout, "[%c] ", msg_type);
	}

	va_list args;
	va_start(args, format);
	if (msg_type == '!') {
		vfprintf(stderr, format, args);
	}
	else {
		vfprintf(stdout, format, args);
	}
	va_end(args);

	// if no newline, keep color
	// (usually used for user input)
	if (strchr(format, '\n') != NULL) {
		SET_NORMAL;
	}
}

// Clears the screen.
// (All that for clearing?)
void cls()
{
	// https://learn.microsoft.com/en-US/windows/console/clearing-the-screen
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);

	SET_NORMAL;
}

// Returns screen dimensions.
// Returns horizontal axis when 'x' is given, vertical axis when 'y' is given.
// (In this particular implementation, returned 'x' is smaller by 1 compared to reality.
// This is because \n does not work properly beyond this 'x' value, so we -1 to circumvent this.)
int get_screen_dims(char axis) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return -1;
	}

	//return axis == 'x' ? csbi.srWindow.Right - csbi.srWindow.Left + 1 : csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return axis == 'x' ? csbi.srWindow.Right - csbi.srWindow.Left : csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// Prints a = divider spanning "full", "half", or "quarter" of the screen.
void print_divider(int width, char* scale) {
	if (!strcmp(scale, "full")) {
		fprintf(stdout, "%.*s\n", width, DIVIDER);
	}
	else if (!strcmp(scale, "half")) {
		fprintf(stdout, "%.*s\n", width / 2, DIVIDER);
	}
	else if (!strcmp(scale, "quarter")) {
		fprintf(stdout, "%.*s\n", width / 4, DIVIDER);
	}
}

// Prints a box (|| ... ||) containing centered text. Spans "width" of the screen.
// Set "new_line" to leave a new line after the box. (This is to provide support to strings on right side of box)
void print_in_center(int width, u_int new_line, char* format, ...) {
	char string[UNIVERSAL_LENGTH] = "";

	va_list args;
	va_start(args, format);
	vsnprintf(string, UNIVERSAL_LENGTH, format, args);
	va_end(args);

	// Space is limited
	if (strlen(string) > width - 2) {
		fprintf(stdout, "%c%.*s%c", BORDER_CHAR, width - 2, string, BORDER_CHAR);
	}
	else {
		int padding_total = width - strlen(string) - 2;
		int padding_left = 0;
		int padding_right = 0;

		// if there is a remainder, skew a bit to the left
		if (padding_total % 2) {
			padding_left = padding_total / 2;
			padding_right = (padding_total / 2) + 1;
		}
		else {
			padding_left = padding_total / 2;
			padding_right = padding_total / 2;
		}

		fprintf(stdout, "%c%.*s%s%.*s%c", BORDER_CHAR, padding_left, SPACES, string, padding_right, SPACES, BORDER_CHAR);
	}

	// if no newline dont print a newline and dont reset colour
	// (usually used for raid verbose)
	if (new_line) {
		fprintf(stdout, "\n");
		SET_NORMAL;
	}
}

// Helper function:
// Simply prints a prompt and requests the user "Press any key to continue" to continue.
void press_any() {
	custom_print('~', "Press any key to continue . . .\n");

	_getch();
}

// Helper function:
// Simply prints a prompt and requests the user "Press any key to continue" to continue.
void erase_previous_line() {
	fprintf(stdout, "\033[A\r%.*s\r", get_screen_dims('x') + 1, SPACES);
}

// Helper function:
// Flashes the application window in the taskbar and plays an alert sound.
// (Legacy version uses the "bell" character, but the sound is a bit jarring.)
void flash_alert() {
	HWND hWnd = GetConsoleWindow();
	FlashWindow(hWnd, TRUE);

	//fprintf(stdout, "\a");
	MessageBeep(MB_ICONEXCLAMATION);
}