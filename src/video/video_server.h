#ifndef VIDEO_SERVER_H
#define VIDEO_SERVER_H

#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

typedef struct {
    uint8_t signature[4]; /* "PaVE" - used to identify the start of frame */

    uint8_t version; /* Version code */

    uint8_t video_codec; /* Codec of the following frame */

    uint16_t header_size; /* Size of the parrot_video_encapsulation_t */

    uint32_t payload_size; /* Amount of data following this PaVE */

    uint16_t encoded_stream_width;

    uint16_t encoded_stream_height;

    uint16_t display_width;

    uint16_t display_height;

    uint32_t frame_number; /* Frame position inside the current stream */

    uint32_t timestamp; /* In milliseconds */

    uint8_t total_chuncks; /* Number of UDP packets containing the current decodable payload - currently unused */

    uint8_t chunk_index ; /* Position of the packet - first chunk is #0 - currenty unused */

    uint8_t frame_type; /* I-frame, P-frame - parrot_video_encapsulation_frametypes_t */

    uint8_t control; /* Special commands like end-of-stream or advertised frames */

    uint32_t stream_byte_position_lw; /* Byte position of the current payload in
                                         the encoded stream - lower 32-bit word */
    uint32_t stream_byte_position_uw; /* Byte position of the current payload in
                                         the encoded stream - upper 32-bit word */

    uint16_t stream_id; /* This ID indentifies packets that should be recorded together */

    uint8_t total_slices; /* number of slices composing the current frame */

    uint8_t slice_index; /* position of the current slice in the frame */

    uint8_t header1_size; /* H.264 only : size of SPS inside payload - no SPS present if value is zero */

    uint8_t header2_size; /* H.264 only : size of PPS inside payload - no PPS present if value is zero */

    uint8_t reserved2[2]; /* Padding to align on 48 bytes */

    uint32_t advertised_size; /* Size of frames announced as advertised frames */

    uint8_t reserved3[12];

    /* Padding to align on 64 bytes */
} __attribute__ ((packed)) parrot_video_encapsulation_t;

struct input_stream
{
    AVStream *ist;
    AVFormatContext *ifcx;
    AVCodecContext *iccx;
    int video_stream_index;
};

typedef enum {
    FRAME_TYPE_UNKNNOWN=0,
    FRAME_TYPE_IDR_FRAME, /* headers followed by I-frame */
    FRAME_TYPE_I_FRAME,
    FRAME_TYPE_P_FRAME,
    FRAME_TYPE_HEADERS
}parrot_video_encapsulation_frametypes_t;

void *video_listen(void*);

#endif
