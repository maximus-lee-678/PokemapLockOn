#include "../headers/time_operations.h"

// Returns current time in time_t format.
time_t get_current_time() {
	return time(NULL);
}

// Converts a time_t value into a human-readable string, and writes it into a provided buffer. 
void stringcopy_formatted_time_string(char* destination, time_t t) {
	struct tm* time_struct;

	time_struct = localtime(&t);
	strftime(destination, UNIVERSAL_LENGTH, "%I:%M:%S%p", time_struct);
}

// Helper function:
// Used to convert an epoch time from raids.php to time string.
void populate_time_name(int total, raid_details_struct* array_raid_details) {
	for (int i = 0; i < total; i++) {
		// No raid in operation, skip check
		if (!array_raid_details[i].has_active_raid) {
			continue;
		}

		// Convert longs to formatted time
		stringcopy_formatted_time_string(array_raid_details[i].readable_raid_spawn, atol(array_raid_details[i].raid_spawn));
		stringcopy_formatted_time_string(array_raid_details[i].readable_raid_start, atol(array_raid_details[i].raid_start));
		stringcopy_formatted_time_string(array_raid_details[i].readable_raid_end, atol(array_raid_details[i].raid_end));
	}
}

// Copies a human-readable representation of the difference between 2 time_t values into the provided buffer.
void stringcopy_formatted_time_difference(char* destination, time_t epoch_time_minuend, time_t epoch_time_subtrahend) {
	time_t t  = epoch_time_minuend - epoch_time_subtrahend;
	time_t minutes = t / 60;
	time_t seconds = t % 60;

	snprintf(destination, UNIVERSAL_LENGTH, "%lldmin, %lldseconds", minutes, seconds);
}