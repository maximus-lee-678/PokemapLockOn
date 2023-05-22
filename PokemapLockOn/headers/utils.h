#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <direct.h>
#include <conio.h>
#include <errno.h>
#include <windows.h>

#define UNIVERSAL_LENGTH 512
#define TWO_MEGABYTES 2097152
#define ONETWOEIGHT_KILOBYTES 131072
#define TWOFIVESIX_KILOBYTES 262144
#define EIGHT_KILOBYTES 8192

typedef struct {
	char gym_name[UNIVERSAL_LENGTH];
	char cell_id[32];					// 19 digits
	char ex_raid_eligible[4];			// 0 or 1
	char sponsor[8];					// 0 or null
	char lat[16];							// max 10: <1-3>.<6>
	char lng[16];							// max 10: <1-3>.<6>
	char raid_spawn[16];				// epoch value 10 long
	char raid_start[16];				// epoch value 10 long
	char raid_end[16];					// epoch value 10 long
	char pokemon_id[8];					// up to 4 digits
	char level[4];						// max 9??
	char cp[8];							// up to 5
	char team[4];						// 0 - 3
	char move1[4];						// 1 - 381
	char move2[4];						// 1 - 381
	char is_exclusive[16];				// ??: appears to always be 0
	char form[8];						// 1 - 2822
	char gender[4];						// 0 - 2

	int has_active_raid;
	char readable_raid_spawn[UNIVERSAL_LENGTH];
	char readable_raid_start[UNIVERSAL_LENGTH];
	char readable_raid_end[UNIVERSAL_LENGTH];
	char readable_pokemon[UNIVERSAL_LENGTH];
	char readable_level[UNIVERSAL_LENGTH];
	char readable_team[UNIVERSAL_LENGTH];
	char readable_move1[UNIVERSAL_LENGTH];
	char readable_move2[UNIVERSAL_LENGTH];
	char readable_form[UNIVERSAL_LENGTH];
	char readable_gender[UNIVERSAL_LENGTH];
} raid_details_struct;

typedef struct {
	double home_coordinates[2];
	int refresh_interval;
	char display_tiers[64];
	char alert_tiers[64];
	int display_mode;
} user_preferences_struct;

#define DISPLAY_MODE_MINIMAL 0
#define DISPLAY_MODE_VERBOSE 1

typedef struct {
	char lat[16];
	char lng[16];
	char gym_name[UNIVERSAL_LENGTH];
	struct gym_node* next;
} gym_node;

#define NODE_SUCCESS 0
#define NODE_ALREADY_EXISTS 1
#define NODE_INVALID_INDEX 2
#define NODE_NULL_HEAD 3
#define NODE_NOT_FOUND 4
#define NODE_MSG_ARR (char[5][64]){"Node action successfully performed!", "Entry already exists!", "Invalid index provided!", "Operation invalid for NULL list!", "No results found."}

// Info below from: https://sgpokemap.com/js/script_gym.js
#define TEAMS_ARRAY_SIZE 4
static const char teams_array[TEAMS_ARRAY_SIZE][16] = {
	"No Team",
	"Mystic",
	"Valor",
	"Instinct"
};

#define GENDER_ARRAY_SIZE 4
static const char gender_array[GENDER_ARRAY_SIZE][8] = {
	"?",
	"M",
	"F",
	"-"
};

#define LEVEL_ARRAY_SIZE 16
static const char level_array[LEVEL_ARRAY_SIZE][16] = {
	"-",
	"Tier 1",
	"-",
	"Tier 3",
	"Community Day",
	"Tier 5",
	"Mega",
	"-",
	"-",
	"Elite",
	"-",
	"Tier 1 (Shadow)",
	"-",
	"Tier 3 (Shadow)",
	"-",
	"Tier 5 (Shadow)"
};

#define TYPES_ARRAY_SIZE 19
static const char types_array[TYPES_ARRAY_SIZE][16] = {
	"None",
	"Normal",
	"Fighting",
	"Flying",
	"Poison",
	"Ground",
	"Rock",
	"Bug",
	"Ghost",
	"Steel",
	"Fire",
	"Water",
	"Grass",
	"Electric",
	"Psychic",
	"Ice",
	"Dragon",
	"Dark",
	"Fairy"
};

#define WEATHER_AFFIN_ARRAY_SIZE 8
static const int weather_affin_array[WEATHER_AFFIN_ARRAY_SIZE][3] = {
	{0, 0, 0},
	{12, 5, 10},
	{11, 13, 7},
	{1, 6, 0},
	{18, 2, 4},
	{16, 3, 14},
	{15, 9, 0},
	{17, 8, 0}
};

void* malloc_safe(size_t size);
void* realloc_safe(void* ptr, size_t size);

void set_bit(u_int* target, int position, int set_to);
int get_bit(u_int* target, int position);

u_int gym_node_append(gym_node** head, char* lat, char* lng, char* gym_name);
u_int gym_node_insert_after_index(gym_node** head, u_int index, char* lat, char* lng, char* gym_name);
u_int gym_node_swap_index(gym_node** head, u_int index_1, u_int index_2);
u_int gym_node_delete_index(gym_node** head, u_int index);
gym_node* gym_node_get_index(gym_node** head, u_int index);
void gym_node_print(u_int index, gym_node* node, double user_lat, double user_long);
void gym_list_print_all(gym_node** head, double user_lat, double user_long);
gym_node* gym_list_subset_by_name(gym_node** head, char* gym_name);
gym_node* gym_list_subset_by_range(gym_node** head, double specified_range, double user_lat, double user_long);
u_int gym_list_count(gym_node** head);
void gym_list_destroy(gym_node** head);