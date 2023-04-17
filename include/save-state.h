#pragma once

#include "shared.h"

typedef struct c8_saved_state c8_saved_state_t;

int c8_saveState(const c8_state_t *state, c8_saved_state_t *out);
void c8_loadState(c8_state_t *state, const c8_saved_state_t *save);
