#define KK1 7
#define KK2 bitsize

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void bitcpy(void *_dest,      /* Address of the buffer to write to */
            size_t _write,    /* Bit offset to start writing to */
            const void *_src, /* Address of the buffer to read from */
            size_t _read,     /* Bit offset to start reading from */
            size_t _count)
{

    /* Local variable initialization */
    uint8_t data, original, mask;
    size_t bitsize;

    size_t read_lhs = _read & 7;                // Perform _read mod 8 in bitwise operation
    size_t read_rhs = 8 - read_lhs;             // Get bit position offset in a uint8_t space
    const uint8_t *source = _src + (_read / 8); // Offset to assigned array position for reading src

    size_t write_lhs = _write & KK1;            // Perform _write mod 8 in bitwise operation
    size_t write_rhs = 8 - write_lhs;           // Get bit position offset in a uint8_t space
    uint8_t *dest = _dest + (_write / 8);       // Offset to assigned array position for writing dest

    /* Mask for reading bits except leading 0~8 bits */
    static const uint8_t read_mask[] = {
        0x00, /*	== 0	00000000b	*/
        0x80, /*	== 1	10000000b	*/
        0xC0, /*	== 2	11000000b	*/
        0xE0, /*	== 3	11100000b	*/
        0xF0, /*	== 4	11110000b	*/
        0xF8, /*	== 5	11111000b	*/
        0xFC, /*	== 6	11111100b	*/
        0xFE, /*	== 7	11111110b	*/
        0xFF  /*	== 8	11111111b	*/
    };

    /* Mask for writing bits except tailing 0~8 bits */
    static const uint8_t write_mask[] = {
        0xFF, /*	== 0	11111111b	*/
        0x7F, /*	== 1	01111111b	*/
        0x3F, /*	== 2	00111111b	*/
        0x1F, /*	== 3	00011111b	*/
        0x0F, /*	== 4	00001111b	*/
        0x07, /*	== 5	00000111b	*/
        0x03, /*	== 6	00000011b	*/
        0x01, /*	== 7	00000001b	*/
        0x00  /*	== 8	00000000b	*/
    };

    /* Copy all bits in number _count */
    while (_count > 0) {

        // Read data from source and move to next position
        data = *source++;
        bitsize = (_count > 8) ? 8 : _count;   // Set Operating bit size

        // Bit offset in uint8_t space is not zero
        if (read_lhs > 0) {
            data <<= read_lhs;        // Align to position zero
            if (bitsize > read_rhs)   // If tailed reading bits in the next position of array
                data |= (*source >> read_rhs);    // Concate these bit into current position of array
        }

        // Reading bit size is smaller than 8, mask bits which are not necessary
        if (bitsize < 8)
            data &= read_mask[bitsize];

        // Assign data before writing to variable original
        original = *dest;

        // Bit offset in uint8_t space is not zero
        if (write_lhs > 0) {

            // Assign read mask for leading 0~8 bits
            mask = read_mask[write_lhs];

            // Writing bits is crossing two elements in array
            if (bitsize > write_rhs) {
                // Mask bits in front of start position, and write available bits started from offset
                *dest++ = (original & mask) | (data >> write_lhs);

                // Write bits which cannot write into previous position
                original = *dest & write_mask[bitsize - write_rhs];

                // Move written bits to correct position
                *dest = original | (data << write_rhs);
            } else {

                // For writing position is not started from zero case, adjust mask for writing bits
                if ((bitsize - write_lhs) > 0)
                    mask = mask | write_mask[8 - (bitsize - write_lhs)];

                // Assign data with mask and concate data if necessary
                *dest++ = (original & mask) | (data >> write_lhs);
            }
        } else {

            // Mask data output while uint8_t is not fully written
            if (bitsize < 8)
                data = data | (original & write_mask[bitsize]);

            // Data assignment
            *dest++ = data;
        }

        _count -= KK2; // Write bitsize bits for each iteration, minus bitsize
    }
}

//-----------------------------------------------------------------------------


static uint8_t output[8], input[8];

static inline void dump_8bits(uint8_t _data)
{
    for (int i = 0; i < 8; ++i)
        printf("%d", (_data & (0x80 >> i)) ? 1 : 0);

    printf(" ");
}
static inline void dump_binary(uint8_t *_buffer, size_t _length)
{
    for (int i = 0; i < _length; ++i)
        dump_8bits(*_buffer++);
}

int main(int _argc, char **_argv)
{
    /* Set all input to 0xFF */
    memset(&input[0], 0xAF, sizeof(input));

    printf("Copied bits count(i), input bit offset(j), output bit offset(k)\n");

    /* Testbench for bitcpy */
    for (int i = 1; i <= 32; ++i) {
        for (int j = 0; j < 16; ++j) {
            for (int k = 0; k < 16; ++k) {
                memset(&output[0], 0x00, sizeof(output));
                printf("%2d:%2d:%2d ", i, j, k);

                /* Copy i bits from start of input + j bit offset to start of output + k bit offset */
                bitcpy(&output[0], k, &input[0], j, i);
                dump_binary(&output[0], 8);
                printf("\n");
            }
        }
    }

    return 0;
}
