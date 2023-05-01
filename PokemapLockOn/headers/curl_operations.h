#pragma once

#include "main.h"
#include "../headers/user_interface.h"

#define USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/111.0"

#define PAGE_HOME "https://sgpokemap.com/raids.php"
#define PAGE_FORMS "https://sgpokemap.com/json/forms.json"
#define PAGE_MOVES "https://sgpokemap.com/json/moves.json"
#define PAGE_POKEMON "https://sgpokemap.com/js/script_gym.js"

typedef struct {
	int bytes;
	char* output_to_string;
}webpage_callback;

// base
void get_webpage(char* webpage_link, char* output_to_string);
int store_webpage(char* buffer, int itemsize, int n_items, void* userp);