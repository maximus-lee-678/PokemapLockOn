#include "../headers/curl_operations.h"

int get_webpage(char* webpage_link, char* output_to_string) {
	CURL* curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "[!] Failed Initializing Curl. (get_webpage)\n");
		curl_easy_cleanup(curl);
		return 1;
	}

	// Zeroise string in case i forgor
	output_to_string[0] = '\0';

	webpage_callback store_callback = { 0 , output_to_string };

	CURLcode result;
	curl_easy_setopt(curl, CURLOPT_URL, webpage_link);	// Set webpage URL
	curl_easy_setopt(curl, CURLOPT_PRIVATE, webpage_link);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, store_webpage);			// Callback function for storing webpage
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&store_callback);		// Callback argument
	curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);						// Follow Redirects
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);						// >= 400, failed

	print_divider(get_screen_dims('x'), "full");
	fprintf(stdout, "[i] Downloading webpage from <%s>\n", webpage_link);

	result = curl_easy_perform(curl);

	if (result != CURLE_OK) {
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		fprintf(stderr, "[!] Could Not Fetch Webpage %s\n[!] Error : %s (Code %d)\n", webpage_link, curl_easy_strerror(result), http_code);
		curl_easy_cleanup(curl);
		return 3;
	}
	else {
		fprintf(stdout, "[>] Webpage Size: %.2f KB\n", (float)store_callback.bytes / 1024);
	}
	print_divider(get_screen_dims('x'), "full");

	curl_easy_cleanup(curl);

	return 0;
}

int store_webpage(char* buffer, int itemsize, int n_items, void* userp) {
	webpage_callback* callback = (webpage_callback*)userp;

	int bytes = itemsize * n_items;

	callback->bytes += bytes;
	strncat(callback->output_to_string, buffer, bytes);

	return bytes;
}
