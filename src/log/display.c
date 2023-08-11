//
// Created by iecas on 2023/8/11.
//

#include "log/display.h"

/**
 * @brief Internal routine to compute the bytes remaining in a buffer.
 *
 * @param[in] dspbuf The buffer.
 *
 * @return the number of bytes remaining in the buffer.
 */
static inline int _display_buffer_remain(struct display_buffer *dspbuf)
{
    /* Compute number of bytes remaining in buffer
     * (including space for null).
     */
    return dspbuf->b_size - (dspbuf->b_current - dspbuf->b_start);
}

/**
 * @brief Compute the bytes remaining in a buffer.
 *
 * @param[in,out] dspbuf The buffer.
 *
 * @retval -1 if there is some problem rendering the buffer unusable.
 * @retval 0 if the buffer has overflowed.
 * @retval >0 indicates the bytes remaining (including one byte for '\0').
 */
int display_buffer_remain(struct display_buffer *dspbuf)
{
    /* If no buffer, indicate problem. */
    if (dspbuf == NULL || dspbuf->b_start == NULL || dspbuf->b_size == 0) {
        errno = EFAULT;
        return -1;
    }

    /* If b_current is invalid, set it to b_start */
    if (dspbuf->b_current == NULL || dspbuf->b_current < dspbuf->b_start
                                                         || dspbuf->b_current > (dspbuf->b_start + dspbuf->b_size))
        dspbuf->b_current = dspbuf->b_start;

    /* Buffer is too small, just make it an empty
     * string and mark the buffer as overrun.
     */
    if (dspbuf->b_size < 4) {
        dspbuf->b_start[0] = '\0';
        dspbuf->b_current = dspbuf->b_start + dspbuf->b_size;
        return 0;
    }

    /* Compute number of bytes remaining in buffer
     * (including space for null).
     */
    return _display_buffer_remain(dspbuf);
}

/**
 * @brief Finish up a buffer after overflowing it.
 *
 * @param[in,out] dspbuf The buffer.
 * @param[in]     ptr    Proposed position in the buffer for the "..." string.
 *
 * This routine will validate the final character that will remain in the buffer
 * prior to the "..." to make sure it is not a partial UTF-8 character. If so,
 * it will place the "..." such that it replaces the partial UTF-8 character.
 * The result will be a proper UTF-8 string (assuming the rest of the string is
 * valid UTF-8...).
 *
 * Caller will make sure sufficient room is available in buffer for the "..."
 * string. This leaves it up to the caller whether prt must be backed up
 * from b_current, or if the caller knows the next item won't fit in the buffer
 * but that there is room for the "..." string.
 */
void _display_complete_overflow(struct display_buffer *dspbuf, char *ptr)
{
    int utf8len;
    char *end;

    /* ptr argument points after last byte that we will retain
     * set 'end' to that and 'ptr' to inspect previous byte if possible
     */
    end = ptr;
    if (ptr > dspbuf->b_start)
        ptr--;

    /* Now ptr points to last byte that will remain part of string.
     * Next we need to check if this byte is the end of a valid UTF-8
     * character.
     */
    while ((ptr > dspbuf->b_start) && ((*ptr & 0xc0) == 0x80))
        ptr--;

    /* Now ptr points to the start of a valid UTF-8 character or the string
     * was rather corrupt, there is no valid start of a UTF-8 character.
     */

    /* Compute the length of the last UTF-8 character */
    utf8len = end - ptr;

    /* Check if last character is valid UTF-8, for multibyte characters the
     * first byte is a string of 1 bits followed by a 0 bit. So for example,
     * a 2 byte character leads off with 110xxxxxxx, so we mask with
     * 11100000 (0xe0) and test for 11000000 (0xc0).
     */
    if ((((*ptr & 0x80) == 0x00) && (utf8len == 1))
        || (((*ptr & 0xe0) == 0xc0) && (utf8len == 2))
        || (((*ptr & 0xf0) == 0xe0) && (utf8len == 3))
        || (((*ptr & 0xf8) == 0xf0) && (utf8len == 4))
        || (((*ptr & 0xfc) == 0xf8) && (utf8len == 5))
        || (((*ptr & 0xfe) == 0xfc) && (utf8len == 6))) {
        /* Last character before end is valid, increment ptr past it. */
        ptr = end;
    }
    /* else last character is not valid, leave ptr to strip it. */

    /* Now we know where to place the ellipsis... */
    strcpy(ptr, "...");
}

/**
 * @brief Prepare to append to buffer.
 *
 * @param[in,out] dspbuf The buffer.
 *
 * @return the bytes remaining in the buffer.
 *
 * This routine validates the buffer, then checks if the buffer is already full
 * in which case it will mark the buffer as overflowed and finish up the buffer.
 *
 */
int display_start(struct display_buffer *dspbuf)
{
    int b_left = display_buffer_remain(dspbuf);

    /* If buffer has already overflowed, or is invalid, return that. */
    if (b_left <= 0)
        return b_left;

    /* If buffer is already full, indicate overflow now, and indicate
     * no space is left (so caller doesn't bother to do anything.
     */
    if (b_left == 1) {
        /* Increment past end and finish buffer. */
        dspbuf->b_current++;
        b_left--;

        /* Back up 4 bytes before last byte (note that b_current
         * points PAST the last byte of the buffer since the
         * buffer has overflowed).
         */
        _display_complete_overflow(dspbuf, dspbuf->b_current - 4);
    } else {
        /* Some display functions might not put anything in the
         * buffer...
         */
        *dspbuf->b_current = '\0';
    }

    /* Indicate buffer is ok by returning b_left. */
    return b_left;
}

/**
 * @brief Finish up a buffer after appending to it.
 *
 * @param[in,out] dspbuf The buffer.
 *
 * @return the bytes remaining in the buffer.
 *
 * After a buffer has been appended to, check for overflow.
 *
 * This should be called by every routine that actually copies bytes into a
 * display_buffer. It must not be called by routines that use other display
 * routines to build a buffer (since the last such routine executed will
 * have called this routine).
 *
 */
int display_finish(struct display_buffer *dspbuf)
{
    /* display_buffer_remain will return the current number of bytes left in
     * the buffer. If this is 0, and we just appended to the buffer (i.e.
     * display_buffer_remain was NOT 0 before appending), then the last
     * append just overflowed the buffer (note that if it exactly filled the
     * buffer, display_buffer_remain would have returned 1). Since the
     * buffer just overflowed, the overflow will be indicated by truncating
     * the string to allow space for a three character "..." sequence.
     */
    int b_left = display_buffer_remain(dspbuf);

    if (b_left != 0)
        return b_left;

    /* We validated above that buffer is at least 4 bytes... */

    /* Back up 4 bytes before last byte (note that b_current points
     * PAST the last byte of the buffer since the buffer has overflowed).
     */
    _display_complete_overflow(dspbuf, dspbuf->b_current - 4);

    return 0;
}

/**
 * @brief Format a string into the buffer.
 *
 * @param[in,out] dspbuf The buffer.
 * @param[in]     fmt    The format string
 * @param[in]     args   The va_list args
 *
 * @return the bytes remaining in the buffer.
 *
 */
int display_vprintf(struct display_buffer *dspbuf, const char *fmt,
                    va_list args)
{
    int len;
    int b_left = display_start(dspbuf);

    if (b_left <= 0)
        return b_left;

    /* snprintf into the buffer no more than b_left bytes. snprintf assures
     * the buffer is null terminated (so will copy at most b_left
     * characters).
     */
    len = vsnprintf(dspbuf->b_current, b_left, fmt, args);

    if (len >= b_left) {
        /* snprintf indicated that if the full string was printed, it
         * would have overflowed. By incrementing b_current by b_left,
         * b_current now points beyond the buffer and clearly marks the
         * buffer as full.
         */
        dspbuf->b_current += b_left;
    } else {
        /* No overflow, move b_current to the end of the printf. */
        dspbuf->b_current += len;
    }

    /* Finish up */
    return display_finish(dspbuf);
}

/**
 * @brief Append a length delimited string to the buffer.
 *
 * @param[in,out] dspbuf The buffer.
 * @param[in]     str    The string
 * @param[in]     len    The length of the string
 *
 * @return the bytes remaining in the buffer.
 *
 */
int display_len_cat(struct display_buffer *dspbuf, const char *str, int len)
{
    int b_left = display_start(dspbuf);
    int cpy;

    if (b_left <= 0)
        return b_left;

    /* Check if string would overflow dspbuf. */
    if (len >= b_left) {
        /* Don't copy more bytes than will fit. */
        cpy = b_left - 1;
    } else {
        /* Copy the entire string including null. */
        cpy = len;
    }

    /* Copy characters and null terminate. */
    memcpy(dspbuf->b_current, str, cpy);
    dspbuf->b_current[cpy] = '\0';

    if (len >= b_left) {
        /* Overflow, indicate by moving b_current past end of buffer. */
        dspbuf->b_current += b_left;
    } else {
        /* Didn't overflow, just increment b_current. */
        dspbuf->b_current += len;
    }

    return display_finish(dspbuf);
}

/**
 * @brief Append a null delimited string to the buffer.
 *
 * @param[in,out] dspbuf The buffer.
 * @param[in]     str    The string
 *
 * @return the bytes remaining in the buffer.
 *
 */
inline int display_cat(struct display_buffer *dspbuf, const char *str)
{
    return display_len_cat(dspbuf, str, strlen(str));
}

/**
 * @brief Compute the string length of the buffer.
 *
 * @param[in] dspbuf The buffer to finish up.
 *
 * @return the length.
 *
 * This function is more efficient than strlen if the buffer hasn't overflowed.
 *
 */
inline size_t display_buffer_len(struct display_buffer *dspbuf)
{
    size_t len = dspbuf->b_current - dspbuf->b_start;

    if (len == dspbuf->b_size) {
        /* Buffer has overflowed, due to forced overflow or partial
         * UTF-8 fixup, the actual string length might actually be less
         * than the full length of the buffer. Just use strlen.
         */
        return strlen(dspbuf->b_start);
    } else {
        return len;
    }
}