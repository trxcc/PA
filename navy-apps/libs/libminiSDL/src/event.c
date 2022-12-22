#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t is_key_down[sizeof(keyname) / sizeof(char *)] = {0};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

char key_buf[64], *key, *act;

static int ana_key(uint8_t *type, uint8_t *sym) {
  if (!NDL_PollEvent(key_buf, sizeof(key_buf))) {
    return 0;
  }
  act = key_buf;
  int i;
  for (i = 0; key_buf[i] != ' '; i++);
  key_buf[i] = '\0';
  key = key_buf + i + 1;
  
  for (i = 0; key[i] != '\0' && key[i] != '\n'; i++);
  if (key[i] == '\n') key[i] = '\0';

  if (act[1] == 'd') *type = SDL_KEYDOWN;
  else *type = SDL_KEYUP;

  for (i = 0; i < sizeof(keyname) / sizeof(char *); i++) {
    if (strcmp(key, keyname[i]) == 0) {
      *sym = i;
      return 1;
    }
  }
  assert(0);
  return 1;
}

int SDL_PollEvent(SDL_Event *ev) {
  uint8_t tmp_type = 0, tmp_sym = 0;
  if (ana_key(&tmp_type, &tmp_sym)) {
    ev->type = tmp_type;
    ev->key.keysym.sym = tmp_sym;
    if (tmp_type == SDL_KEYDOWN) is_key_down[tmp_sym] = 1;
    else if (tmp_type == SDL_KEYUP) is_key_down[tmp_sym] = 0;
    return 1;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  uint8_t tmp_type = 0, tmp_sym = 0;
  while (!ana_key(&tmp_type, &tmp_sym));
  event->type = tmp_type;
  event->key.keysym.sym = tmp_sym;
  if (tmp_type == SDL_KEYDOWN) is_key_down[tmp_sym] = 1;
  else if (tmp_type == SDL_KEYUP) is_key_down[tmp_sym] = 0;
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if (numkeys) *numkeys = sizeof(is_key_down) / sizeof(is_key_down[0]); 
  return is_key_down;
}
