#ifndef WAV_FILE_READER_HPP
#define WAV_FILE_READER_HPP

#include "format_wav.h"

#include "sound_file.hpp"

class WavFileReader : public SoundFile
{
    using SampleT = int16_t;

public:
    WavFileReader(const uint8_t *data, size_t size); // TODO Replace pointer with a shared pointer

    const SampleT *samples() const
    {
        return m_data;
    }

    size_t samples_number() const
    {
        return m_data_size;
    }

    uint16_t audio_format() const
    {
        return m_header.fmt_chunk.audio_format;
    }

    uint16_t num_of_channels() const
    {
        return m_header.fmt_chunk.num_of_channels;
    }

    uint32_t sample_rate() const
    {
        return m_header.fmt_chunk.sample_rate;
    }

    uint32_t byte_rate() const
    {
        return m_header.fmt_chunk.byte_rate;
    }

    uint16_t bits_per_sample() const
    {
        return m_header.fmt_chunk.bits_per_sample;
    }

    // TODO Implement read method
    size_t read(SampleT *samples, size_t number) override
    {
        return 0;
    }

private:
    wav_header_t m_header;
    const SampleT *m_data;
    size_t m_data_size;
};

#endif // WAV_FILE_READER_HPP