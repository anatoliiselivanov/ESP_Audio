#ifndef MP3_FILE_READER_HPP
#define MP3_FILE_READER_HPP

#include "sound_file.hpp"

#include "minimp3.h"

class Mp3FileReader : public SoundFile
{
public:
    Mp3FileReader(uint8_t *input_buf, size_t size);

    uint16_t num_of_channels() const override
    {
        return info.channels;
    }

    uint32_t sample_rate() const override
    {
        return info.hz;
    }

    uint32_t byte_rate() const override
    {
        return info.bitrate_kbps * 1000 / 8;
    }

    uint16_t bits_per_sample() const override
    {
        return info.frame_bytes;
    }

    size_t read(SampleT *samples, size_t number) override
    {
        size_t read_size = mp3dec_decode_frame(&mp3d, m_input_buf, number * sizeof(SampleT), samples, &info);
        m_input_buf += read_size;
        return read_size / sizeof(SampleT);
    }

private:
    mp3dec_t mp3d = {};
    mp3dec_frame_info_t info = {};
    uint8_t *m_input_buf;
};

#endif // MP3_FILE_READER_HPP