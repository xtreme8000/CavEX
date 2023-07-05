#include <assert.h>

#include "config.h"

bool config_create(struct config* c, const char* filename) {
	assert(c && filename);
	c->root = json_parse_file(filename);

	if(!c->root)
		return false;

	if(json_value_get_type(c->root) != JSONObject) {
		config_destroy(c);
		return false;
	}

	return true;
}

const char* config_read_string(struct config* c, const char* key,
							   const char* fallback) {
	assert(c && key);
	// TODO: only give out copy
	const char* res = json_object_dotget_string(json_object(c->root), key);
	return res ? res : fallback;
}

bool config_read_int_array(struct config* c, const char* key, int* dest,
						   size_t* length) {
	assert(c && key && dest);

	JSON_Array* entry = json_object_dotget_array(json_object(c->root), key);

	if(!entry)
		return false;

	*length = *length < json_array_get_count(entry) ?
		*length :
		json_array_get_count(entry);

	for(size_t k = 0; k < *length; k++)
		dest[k] = json_array_get_number(entry, k);

	return true;
}

void config_destroy(struct config* c) {
	assert(c && c->root);
	json_value_free(c->root);
}
