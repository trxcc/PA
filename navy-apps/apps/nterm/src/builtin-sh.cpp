#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <stdio.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  //char *tmp = (char *)cmd;
#ifndef MAX_ARGC 
#define MAX_ARGC 32
#endif

  char *tmp = (char *)"pal --skip";
  int i;
  for (i = 0; tmp[i] != ' ' && tmp[i] != '\n' && tmp[i] != '\0'; i++);
  if (tmp[i] == '\n' || tmp[i] == '\0') {
    tmp[i] = '\0';
    execvp(tmp, NULL);
    return;
  } 
  char *argv[MAX_ARGC];
  int argc = 0;

  char *argv0 = tmp;
  tmp += (i + 1);
  printf("tmmmp: %s\n", tmp);
  argv0[i] = '\0';
  printf("argv000: %s\n", argv0);
  argv[argc++] = argv0;
  printf("argv0: %s, tmp: %s\n", argv0, tmp);
  i = 0;
  while (tmp[i] != '\n' && tmp[i] != '\0') {
    if (*tmp == ' ') ++tmp;
    else if (tmp[i] != ' ') {
      i++; 
      continue;
    }
    argv0 = tmp;
    tmp += (i + 1);
    argv0[i] = '\0';
    argv[argc++] = argv0;
    i = 0;
  }
  tmp[i] = '\0';
  argv[argc++] = tmp;
  argv[argc] = NULL;
  for (int i = 0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  //execve(tmp, NULL, NULL);
  execvp(argv[0], argv);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
