#ifndef WAV_FILE_READER_HPP
#define WAV_FILE_READER_HPP

#include "format_wav.h"

#include <stdint.h>
#include <stdio.h>

class WavFileReader
{
public:
    WavFileReader(const uint8_t *data, size_t size); // TODO Replace pointer with a shared pointer

    const uint8_t *data() { return m_data; }
    size_t data_size() { return m_data_size; }

    uint16_t audio_format() { return m_header.fmt_chunk.audio_format; }
    uint16_t num_of_channels() { return m_header.fmt_chunk.num_of_channels; }
    uint32_t sample_rate() { return m_header.fmt_chunk.sample_rate; }
    uint32_t byte_rate() { return m_header.fmt_chunk.byte_rate; }
    uint16_t bits_per_sample() { return m_header.fmt_chunk.bits_per_sample; }

private:
    wav_header_t m_header;
    const uint8_t *m_data;
    size_t m_data_size;
};

#endif // WAV_FILE_READER_HPP