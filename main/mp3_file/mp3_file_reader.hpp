#ifndef MP3_FILE_READER_HPP
#define MP3_FILE_READER_HPP

#include "sound_file.hpp"

#include "minimp3.h"

class Mp3FileReader : public SoundFile
{
public:
    Mp3FileReader(const uint8_t *data, size_t size);

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

    size_t read(SampleT *samples, size_t number) override;

    void reset()
    {
        m_data_offset = 0;
    }

private:
    mp3dec_t mp3d = {};
    mp3dec_frame_info_t info = {};
    const uint8_t *m_data;
    size_t m_data_size;
    size_t m_data_offset = 0;
};

#endif // MP3_FILE_READER_HPP