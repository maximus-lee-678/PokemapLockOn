#pragma once

#include "main.h"

time_t get_current_time();
void stringcopy_formatted_time_string(char* destination, time_t t);
void populate_time_name(int total, raid_details_struct* array_raid_details);
void stringcopy_formatted_time_difference(char* destination, time_t epoch_time_minuend, time_t epoch_time_subtrahend);