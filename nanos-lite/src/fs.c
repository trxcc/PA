#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

extern size_t ramdisk_read(void *, size_t, size_t);
extern size_t ramdisk_write(const void *, size_t, size_t);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

struct fileState{
  bool is_open;
  size_t open_offset;
}file_state[50];

static bool check_open_overflow(int fd, size_t len) {
  if (!file_state[fd].is_open) { return false; }
  //Log("Open_offset: %d, len: %d, size: %d", file_state[fd].open_offset, len, file_table[fd].size);
  if (len + file_state[fd].open_offset > file_table[fd].size) {
    return false;
  }
  return true;
}

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 3; i < 23; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) 
      break;
  }
  if (i == 23) {
    Log("File %s not found!\n", pathname);
    assert(0);
  } 
  file_state[i].is_open = true;
  file_state[i].open_offset = 0;
  //Log("OK with %d", i);
  return i;
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (!check_open_overflow(fd, len)) {
    panic("file not open or overflow!");
  }
  size_t fd_read_offset = file_table[fd].disk_offset + file_state[fd].open_offset; 
  printf("file_off: %d, read_off: %d\n", file_state[fd].open_offset, fd_read_offset);
  file_state[fd].open_offset += len;
  return ramdisk_read(buf, fd_read_offset, len);
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (!check_open_overflow(fd, len)) {
    panic("file not open or overflow!");
  }
  printf("file_off: %d\n", file_state[fd].open_offset);
  size_t fd_write_offset = file_table[fd].disk_offset + file_state[fd].open_offset; 
  file_state[fd].open_offset += len;
  return ramdisk_write(buf, fd_write_offset, len);
}


size_t fs_lseek(int fd, size_t offset, int whence) { 
  size_t now_offset = offset, tmp = file_state[fd].open_offset;
  //printf("file_off: %d\n", file_state[fd].open_offset);
  //printf("now_file_off: %d\n", file_state[fd].open_offset);
  if (whence == SEEK_SET) {
    file_state[fd].open_offset = now_offset;
  }
  else if (whence == SEEK_CUR) {
    //printf("file_off: %d\n", file_state[fd].open_offset);
    file_state[fd].open_offset += offset;
    now_offset = file_state[fd].open_offset;
  }
  else if (whence == SEEK_END) {
    now_offset = file_table[fd].size + offset;
    file_state[fd].open_offset = now_offset;
  }
  if (!file_state[fd].is_open || file_state[fd].open_offset > file_table[fd].size) { 
    file_state[fd].open_offset = tmp;
    panic("file not open or overflow!");
  } 
  return now_offset;
}

size_t fs_get_file_size(int fd) {
  return file_table[fd].size;
}

size_t fs_get_file_off(int fd) {
  return file_table[fd].disk_offset;
}

int fs_close(int fd) {
  return 0;
}
