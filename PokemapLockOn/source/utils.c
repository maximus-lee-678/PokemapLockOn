#include "../headers/utils.h"
#include "../headers/user_interface.h"
#include "../headers/coordinates.h"

//[MEMORY]

// Ensures malloc success. Always call on malloced pointer.
// Since there is no need for casting, we can directly return the void pointer.
// If the pointer leads to NULL, catastrophic failure has occurred, the program will ~off itself~ exit.
void* malloc_safe(size_t size)
{
	void* ptr = malloc(size);

	if (ptr == NULL) {
		custom_print('!', "[FATAL ERROR] malloc failed!\n");

		press_any();
		exit(1);
	}

	return ptr;
}

// Ensures realloc success. Always call on realloced pointer.
// Since there is no need for casting, we can directly return the void pointer.
// If the pointer leads to NULL, catastrophic failure has occurred, the program will ~off itself~ exit.
void* realloc_safe(void* ptr, size_t size)
{
	ptr = realloc(ptr, size);

	// if realloc(0) is called, it frees previous and assigns NULL, so must check for size == 0
	if (ptr == NULL && size != 0) {
		custom_print('!', "[FATAL ERROR] realloc failed!\n");

		press_any();
		exit(1);
	}

	return ptr;
}

//[BIT MANIP]

// Sets a bit of a u_int to 0 or 1.
// position: 31 - 0
void set_bit(u_int* target, int position, int set_to) {
	if (set_to) {
		*target |= 1 << position;
	}
	else {
		*target &= ~(1 << position);
	}
}

// Gets a bit of a u_int.
// position: 31 - 0
int get_bit(u_int* target, int position) {
	return (*target >> position) & 1;
}

//[GYM_NODE MANIP]
// For return code definitions, check utils.h.

// This function adds a new node to the end of the linked list,
// IF IT DOES NOT ALREADY EXIST IN THE PROVIDED LINKED LIST.
u_int gym_node_append(gym_node** head, char* lat, char* lng, char* gym_name)
{
	//create a new node
	gym_node* new_node = malloc_safe(sizeof(gym_node));
	*new_node = (gym_node){ .next = NULL };
	strcpy(new_node->lat, lat);
	strcpy(new_node->lng, lng);
	strcpy(new_node->gym_name, gym_name);

	//if head is NULL, it is an empty list
	if (*head == NULL) {
		*head = new_node;
		return NODE_SUCCESS;
	}

	//Otherwise, find the last node and add the new_node
	gym_node* iter_node = *head;

	while (1) {
		if (!strcmp(iter_node->gym_name, gym_name) && !strcmp(iter_node->lat, lat) && !strcmp(iter_node->lng, lng)) {
			free(new_node);
			return NODE_ALREADY_EXISTS;
		}
		if (iter_node->next != NULL) {
			iter_node = iter_node->next;
		}
		else {
			iter_node->next = new_node;
			return NODE_SUCCESS;
		}
	}
}

// This function adds a new node after a specified index of the linked list,
// IF IT DOES NOT ALREADY EXIST IN THE PROVIDED LINKED LIST.
// This method will also fail with a NULL head, use gym_node_append to initialise.
// This index is 1-based to reduce confusion when user selects.
u_int gym_node_insert_after_index(gym_node** head, u_int index, char* lat, char* lng, char* gym_name) {
	// Invalid index
	if (index < 1) {
		return NODE_INVALID_INDEX;
	}

	gym_node* iter_node = *head;
	// Empty linked list given
	if (iter_node == NULL) {
		return NODE_NULL_HEAD;
	}

	// Search for dupes
	while (iter_node != NULL) {
		if (!strcmp(iter_node->gym_name, gym_name) && !strcmp(iter_node->lat, lat) && !strcmp(iter_node->lng, lng)) {
			return NODE_ALREADY_EXISTS;
		}
		iter_node = iter_node->next;
	}
	iter_node = *head;

	//create a new node
	gym_node* new_node = malloc_safe(sizeof(gym_node));
	*new_node = (gym_node){ .next = NULL };
	strcpy(new_node->lat, lat);
	strcpy(new_node->lng, lng);
	strcpy(new_node->gym_name, gym_name);


	for (u_int iter_node_index = 1; iter_node_index < index; iter_node_index++) {
		iter_node = iter_node->next;

		// Linked list ended before finding desired index
		if (iter_node == NULL) {
			return NODE_INVALID_INDEX;
		}
	}

	// Change next_node of index to new_node
	// If !null, make new_node's next index+1's node
	gym_node* og_next_node = iter_node->next;
	iter_node->next = new_node;
	if (og_next_node != NULL) {
		new_node->next = og_next_node;
	}

	return NODE_SUCCESS;
}

// This function swaps 2 specified nodes of a linked list.
// There are many ways to do it, but this is mine (i.e shit 5 pointers)
// This index is 1-based to reduce confusion when user selects.
u_int gym_node_swap_index(gym_node** head, u_int index_1, u_int index_2) {
	// Invalid index
	if (index_1 < 1 || index_2 < 1 || index_1 == index_2) {
		return NODE_INVALID_INDEX;
	}
	u_int max_index = index_1 > index_2 ? index_1 : index_2;

	gym_node* iter_node = *head;
	gym_node* iter_node_prev = NULL;
	// Empty linked list given
	if (iter_node == NULL) {
		return NODE_NULL_HEAD;
	}

	// [LOCATE SECTION]
	// Cycles thru linked list
	// If it finds a node with matching index, it captures the address of that node and its previous node
	gym_node* og_index1_node = NULL, * og_index2_node = NULL, * og_index1_node_prev = NULL, * og_index2_node_prev = NULL;
	for (u_int iter_node_index = 1; iter_node_index <= max_index; iter_node_index++) {
		if (iter_node_index == index_1) {
			og_index1_node = iter_node;
			if (iter_node_prev != NULL) {
				og_index1_node_prev = iter_node_prev;
			}
		}

		if (iter_node_index == index_2) {
			og_index2_node = iter_node;
			if (iter_node_prev != NULL) {
				og_index2_node_prev = iter_node_prev;
			}
		}

		if (og_index1_node != NULL && og_index2_node != NULL) {
			break;
		}

		iter_node_prev = iter_node;
		iter_node = iter_node->next;

		// Linked list ended before finding desired index
		if (iter_node == NULL) {
			return NODE_INVALID_INDEX;
		}
	}

	// [SWAP SECTION]
	// Swap nodes (it works cause you swapping the addresses of both, not the content)
	gym_node* temp_node = og_index1_node;
	og_index1_node = og_index2_node;
	og_index2_node = temp_node;

	// Swaps nodes->next
	temp_node = og_index1_node->next;
	og_index1_node->next = og_index2_node->next;
	og_index2_node->next = temp_node;

	// Swaps previous nodes'->next
	// If any previous nodes remained NULL, promote actual node to new head instead
	if (og_index1_node_prev == NULL) {
		*head = og_index1_node;
		og_index2_node_prev->next = og_index2_node;
	}
	else if (og_index2_node_prev == NULL) {
		*head = og_index2_node;
		og_index1_node_prev->next = og_index1_node;
	}
	else {
		temp_node = og_index1_node_prev->next;
		og_index1_node_prev->next = og_index2_node_prev->next;
		og_index2_node_prev->next = temp_node;
	}

	return NODE_SUCCESS;
}

// This function deletes a specified node of a linked list.
// This index is 1-based to reduce confusion when user selects.
// 
// Return values:
// 0: Success | 1: Invalid Index | 2: Invalid head
u_int gym_node_delete_index(gym_node** head, u_int index) {
	// Invalid index
	if (index < 1) {
		return NODE_INVALID_INDEX;
	}

	gym_node* iter_node = *head;
	gym_node* iter_node_prev = NULL;
	// Empty linked list given
	if (iter_node == NULL) {
		return NODE_NULL_HEAD;
	}

	// Cycles thru linked list until index
	for (u_int iter_node_index = 1; iter_node_index < index; iter_node_index++) {
		iter_node_prev = iter_node;
		iter_node = iter_node->next;

		// Linked list ended before finding desired index
		if (iter_node == NULL) {
			return NODE_INVALID_INDEX;
		}
	}

	// If node is not head, cut out middleman
	// Else promote head->next to head (iter_node will be pointing to head)
	if (iter_node_prev != NULL) {
		iter_node_prev->next = iter_node->next;
		free(iter_node);
	}
	else {
		*head = iter_node->next;
		free(iter_node);
	}

	return NODE_SUCCESS;
}

// This function returns a pointer to a specified node of a linked list based on index.
// Special return conditions:
// Success: pointer to node, Failure: NULL pointer
gym_node* gym_node_get_index(gym_node** head, u_int index) {
	gym_node* iter_node = *head;

	// Cycles thru linked list until index
	for (u_int iter_node_index = 1; iter_node_index < index; iter_node_index++) {
		iter_node = iter_node->next;

		// Linked list ended before finding desired index
		if (iter_node == NULL) {
			return NULL;
		}
	}

	return iter_node;
}

// This function prints a SINGLE NODE's info based on a given node's address.
void gym_node_print(u_int index, gym_node* node, double user_lat, double user_long) {
	if (index) {
		custom_print('0', " <%d>\t[%s] {%s, %s} (%.1fm, %.1f%c)\n", index, node->gym_name, node->lat, node->lng,
			get_lat_lon_dist_meters(user_lat, user_long, strtod(node->lat, NULL), strtod(node->lng, NULL)),
			get_lat_lon_bearing_degrees(user_lat, user_long, strtod(node->lat, NULL), strtod(node->lng, NULL)), DEG_CHAR);
	}
	else {
		custom_print('0', "[%s] {%s, %s} (%.1fm, %.1f%c)\n", node->gym_name, node->lat, node->lng,
			get_lat_lon_dist_meters(user_lat, user_long, strtod(node->lat, NULL), strtod(node->lng, NULL)),
			get_lat_lon_bearing_degrees(user_lat, user_long, strtod(node->lat, NULL), strtod(node->lng, NULL)), DEG_CHAR);
	}
}

// This function prints the ENTIRE LINKED LIST's info based on a given list's head pointer.
void gym_list_print_all(gym_node** head, double user_lat, double user_long) {
	gym_node* iter_node = *head;
	if (iter_node == NULL) {
		return;
	}
	u_int index = 1;

	do {
		gym_node_print(index, iter_node, user_lat, user_long);

		iter_node = iter_node->next;
		index++;
	} while (iter_node != NULL);
}

// This function creates a new subset linked list based on provided head containing nodes that matched the provided name.
// Returns pointer to head of subset linked list.
gym_node* gym_list_subset_by_name(gym_node** head, char* gym_name) {
	gym_node* iter_node = *head;
	gym_node* sub_linked_list_head = NULL;

	while (iter_node != NULL) {
		if (!strcmp(gym_name, iter_node->gym_name)) {
			gym_node_append(&sub_linked_list_head, iter_node->lat, iter_node->lng, iter_node->gym_name);
		}

		iter_node = iter_node->next;
	}

	return sub_linked_list_head;
}

// This function creates a new subset linked list based on provided head containing nodes that are in a specified range.
// Returns pointer to head of subset linked list.
gym_node* gym_list_subset_by_range(gym_node** head, double specified_range, double user_lat, double user_long) {
	gym_node* iter_node = *head;
	gym_node* sub_linked_list_head = NULL;

	while (iter_node != NULL) {
		if (get_lat_lon_dist_meters(strtod(iter_node->lat, NULL), strtod(iter_node->lng, NULL), user_lat, user_long) < specified_range) {
			gym_node_append(&sub_linked_list_head, iter_node->lat, iter_node->lng, iter_node->gym_name);
		}

		iter_node = iter_node->next;
	}

	return sub_linked_list_head;
}


// This function returns number of nodes in linked list.
u_int gym_list_count(gym_node** head) {
	gym_node* iter_node = *head;
	u_int count = 0;

	while (iter_node != NULL) {
		count++;
		iter_node = iter_node->next;
	}

	return count;
}

// This function blows up an entire linked list based on a head's pointer.
// The head will then be set to NULL.
void gym_list_destroy(gym_node** head) {
	gym_node* iter_node = *head;
	gym_node* free_node = NULL;

	while (iter_node != NULL) {
		free_node = iter_node;
		iter_node = iter_node->next;
		free(free_node);
	}

	*head = NULL;
}