//
// Created by iecas on 2023/8/11.
//

#ifndef DNFS_DISPLAY_H
#define DNFS_DISPLAY_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

struct display_buffer {
    size_t b_size;		/*< Size of the buffer, will hold b_size
				    - 1 chars plus a '\0' */
    char *b_current;	/*< Current position in the buffer, where the
				    next string will be appended */
    char *b_start;		/*< Start of the buffer */
};

int display_start(struct display_buffer *dspbuf);
int display_vprintf(struct display_buffer *dspbuf, const char *fmt,
                    va_list args);
int display_cat(struct display_buffer *dspbuf, const char *str);
size_t display_buffer_len(struct display_buffer *dspbuf);

#endif //DNFS_DISPLAY_H
