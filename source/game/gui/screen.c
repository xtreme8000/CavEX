#include <assert.h>

#include "../game_state.h"

void screen_set(struct screen* s) {
	assert(s);
	gstate.current_screen = s;
	if(s->reset)
		s->reset(s);
}
