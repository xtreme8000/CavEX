#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "parson/parson.h"

struct config {
	JSON_Value* root;
};

bool config_create(struct config* c, const char* filename);
const char* config_read_string(struct config* c, const char* key,
							   const char* fallback);
bool config_read_int_array(struct config* c, const char* key, int* dest,
						   size_t* length);
void config_destroy(struct config* c);

#endif
