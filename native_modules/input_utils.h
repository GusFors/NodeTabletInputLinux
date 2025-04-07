#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

struct position_buffer {
  unsigned int next_index;
  unsigned int capacity;
  int *items;
};

#ifdef __cplusplus
extern "C" {
#endif

void parse_tablet_buffer_avg(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display);

#ifdef __cplusplus
}
#endif

#endif
