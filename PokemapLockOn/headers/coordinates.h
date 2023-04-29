#pragma once

#define _USE_MATH_DEFINES

#include "main.h"
#include <math.h>

double deg_to_rad(double deg);
double get_lat_lon_dist_meters(double lat1, double lon1, double lat2, double lon2);
double get_lat_lon_bearing_degrees(double lat1, double lon1, double lat2, double lon2);