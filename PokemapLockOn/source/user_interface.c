#include "../headers/user_interface.h"
#include "../headers/time_operations.h"
#include "../headers/coordinates.h"

void print_raid_details(int total, raid_details_struct* array_raid_details, user_preferences_struct* user_preferences) {
	u_int count_inactive = 0;

	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			count_inactive++;
			continue;
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
	fprintf(stdout, "[x] No raids: %d\n", count_inactive);
}

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

		print_divider(WIDTH_RAID_BOX, "full");
		print_in_center_trailing(WIDTH_RAID_BOX, array_raid_details.readable_level, gym_string);
		print_in_center_trailing(WIDTH_RAID_BOX, array_raid_details.readable_pokemon, time_start_end_string);
		print_in_center_trailing(WIDTH_RAID_BOX, array_raid_details.readable_move1, time_left_string);
		print_in_center_trailing(WIDTH_RAID_BOX, array_raid_details.readable_move2, details_string);
		print_divider(WIDTH_RAID_BOX, "full");
	}
	else {
		fprintf(stdout, "[%s] <<<%s>>> | %s | %s (%s, %s)\n", 
			array_raid_details.gym_name, array_raid_details.readable_level, time_left_string, 
			array_raid_details.readable_pokemon, array_raid_details.readable_move1, array_raid_details.readable_move2);
	}
}

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
}

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

void print_in_center(int width, char* string) {
	// Space is limited
	if (strlen(string) > width - 2) {
		fprintf(stdout, "%c%.*s%c\n", BORDER_CHAR, width - 2, string, BORDER_CHAR);

		return;
	}

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

	fprintf(stdout, "%c%.*s%s%.*s%c\n", BORDER_CHAR, padding_left, SPACES, string, padding_right, SPACES, BORDER_CHAR);
}

void print_in_center_trailing(int width, char* string, char* trailing_text) {
	// Space is limited
	if (strlen(string) > width - 2) {
		fprintf(stdout, "%c%.*s%c\n", BORDER_CHAR, width - 2, string, BORDER_CHAR);

		return;
	}

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

	fprintf(stdout, "%c%.*s%s%.*s%c %s\n", BORDER_CHAR, padding_left, SPACES, string, padding_right, SPACES, BORDER_CHAR, trailing_text);
}

void press_any() {
	fprintf(stdout, "[~] Press any key to continue . . .\n");

	_getch();
}

void erase_previous_line() {
	fprintf(stdout, "\033[A\r%.*s\r", get_screen_dims('x') + 1, SPACES);
}

void flash_alert() {
	HWND hWnd = GetConsoleWindow();
	FlashWindow(hWnd, TRUE);
	fprintf(stdout, "\a");

	//https://stackoverflow.com/questions/21034935/playsound-in-c
	//PlaySound("gottem.wav", NULL, SND_FILENAME);
}