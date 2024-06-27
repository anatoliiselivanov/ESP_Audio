#ifndef WAV_FILE_READER_HPP
#define WAV_FILE_READER_HPP

#include <stdint.h>
#include <stdio.h>

#include "format_wav.h"

class WavFileReader
{
public:
    WavFileReader(const char *file_path);
    WavFileReader(const uint8_t *data, size_t size);
    ~WavFileReader();

    uint16_t *data() { return m_data; }
    size_t data_size() { return m_data_size; }

    uint16_t audio_format() { return m_header.fmt_chunk.audio_format; }
    uint16_t num_of_channels() { return m_header.fmt_chunk.num_of_channels; }
    uint32_t sample_rate() { return m_header.fmt_chunk.sample_rate; }
    uint32_t byte_rate() { return m_header.fmt_chunk.byte_rate; }
    uint16_t bits_per_sample() { return m_header.fmt_chunk.bits_per_sample; }

private:
    const char *m_file_path;
    FILE *m_file;
    wav_header_t m_header;
    uint16_t *m_data;
    size_t m_data_size;

    void read_header();
    size_t read_data();
};

#endif // WAV_FILE_READER_HPP