#pragma once

#include "main.h"

#define DIVIDER "================================================================================================================================================================================================================================================================"
#define SPACES "                                                                                                                                                                                                                                                                "
#define BORDER_CHAR 186
#define DEG_CHAR 248

#define WIDTH_RAID_BOX 29	// Menacing Moonraze Maelstrom has the longest English name of all moves, with 27 characters.

#define MSG_AUTO_REFRESH "Auto-refreshing %d minute(s) from %s, or press any key to enter [OPTIONS]."

void print_raid_details(int total, raid_details_struct* array_raid_details, user_preferences_struct* user_preferences);
void print_raid_info(raid_details_struct array_raid_details, double user_lat, double user_lng, u_int display_mode);

void cls();
int get_screen_dims(char axis);
void print_divider(int width, char* scale);
void print_in_center(int width, char* string);
void print_in_center_trailing(int width, char* string, char* trailing_text);
void press_any();
void erase_previous_line();
void flash_alert();

#define CLEAR_TERMINAL cls();
#define SET_INSTINCT_YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#define SET_VALOR_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
#define SET_MYSTIC_BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define SET_CYAN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#define SET_NORMAL SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);