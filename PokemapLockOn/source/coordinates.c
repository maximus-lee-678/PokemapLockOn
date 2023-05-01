#include "../headers/coordinates.h"

// Converts degrees format to radians format.
double deg_to_rad(double deg) {
	return deg * (M_PI / 180);
}

// Returns a double value stating distance between lat1lon1 and lat2lon2.
// Formula adapted from: https://www.movable-type.co.uk/scripts/latlong.html
double get_lat_lon_dist_meters(double lat1, double lon1, double lat2, double lon2) {
	// Using Haverzine Formula

	int R = 6371000; // Radius of the earth in m
	double rad_lat1 = deg_to_rad(lat1);
	double rad_lat2 = deg_to_rad(lat2);
	double delta_lat = deg_to_rad(lat2 - lat1);
	double delta_lon = deg_to_rad(lon2 - lon1);

	double a = sin(delta_lat / 2) * sin(delta_lat / 2) + cos(rad_lat1) * cos(rad_lat2) * sin(delta_lon / 2) * sin(delta_lon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	double d = R * c; // Distance in m

	return d;
}

// Returns a double value stating degrees bearing from lat1lon1 to lat2lon2.
// Formula adapted from: https://www.movable-type.co.uk/scripts/latlong.html
double get_lat_lon_bearing_degrees(double lat1, double lon1, double lat2, double lon2) {
	double rad_lat1 = deg_to_rad(lat1);
	double rad_lat2 = deg_to_rad(lat2);
	double rad_lon1 = deg_to_rad(lon1);
	double rad_lon2 = deg_to_rad(lon2);

	double y = sin(rad_lon2 - rad_lon1) * cos(rad_lat2);
	double x = cos(rad_lat1) * sin(rad_lat2) - sin(rad_lat1) * cos(rad_lat2) * cos(rad_lon2 - rad_lon1);
	double rad_bear = atan2(y, x);
	double deg_bear = (rad_bear * 180 / M_PI + 360);

	// can't % a double, this should be good enough
	if (deg_bear > 360.0) {
		deg_bear -= 360.0;
	}

	return deg_bear;
}