#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "display.h"
#include "tablet.h"
#include "input_utils.h"

void print_position_buffer(struct position_buffer *pb) {
  printf("[");
  for (unsigned int i = 0; i < pb->capacity; i++) {
    printf("%d,", pb->items[i]);
  }
  printf("]\n");
}

void create_position_buffer(struct position_buffer *pb, unsigned int capacity) {
  pb->next_index = 0;
  pb->capacity = capacity;
  pb->items = calloc(capacity, sizeof(int));
}

void position_buffer_insert_val(struct position_buffer *pb, int val) {
  // printf("pb->next_index %% pb->capacity: %d, next_index: %d\n", pb->next_index % pb->capacity, pb->next_index);
  pb->items[pb->next_index % pb->capacity] = val;
  pb->next_index = pb->next_index % pb->capacity + 1;
}

void reset_position_buffer(struct position_buffer *pb) {
  for (unsigned int i = 0; i < pb->capacity; i++)
    pb->items[i] = -1;
}

void parse_tablet_buffer_avg(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;

  ssize_t r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  struct position_buffer x_position_buf;
  struct position_buffer y_position_buf;

  unsigned int position_buffer_capacity = 8;

  create_position_buffer(&x_position_buf, position_buffer_capacity);
  create_position_buffer(&y_position_buf, position_buffer_capacity);

  while (active) {
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    DEBUG_REPORT(buf, r);

    if (buf[1] == 0x80) {
      reset_position_buffer(&x_position_buf);
      reset_position_buffer(&y_position_buf);
      // printf("out of range, setting position buffer values to -1\n");
      continue;
    }

    if (buf[0] <= 0x10) {
      x = (buf[tablet.xindex]) | ((buf[tablet.xindex + 1]) << 8);
      y = (buf[tablet.yindex]) | ((buf[tablet.yindex + 1]) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      position_buffer_insert_val(&x_position_buf, (int)x_scaled);
      position_buffer_insert_val(&y_position_buf, (int)y_scaled);

      int x_avg_position = 0;
      int y_avg_position = 0;
      int x_div = 0;
      int y_div = 0;

      for (unsigned int i = 0; i < position_buffer_capacity; i++) {
        if (x_position_buf.items[i] > 0) {
          x_avg_position += x_position_buf.items[i];
          x_div++;
        }

        if (y_position_buf.items[i] > 0) {
          y_avg_position += y_position_buf.items[i];
          y_div++;
        }
      }

      if (x_div > 0)
        x_avg_position /= x_div;
      if (y_div > 0)
        y_avg_position /= y_div;

      if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled)) {
        tabletbtn_input_event(tablet_fd, x_avg_position + display.offset_x, y_avg_position + display.offset_y, buf[tablet.bindex]);
      }
    }
  }
}
