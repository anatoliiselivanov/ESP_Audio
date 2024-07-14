#ifndef SOUND_FILE_HPP
#define SOUND_FILE_HPP

#include <stdint.h>
#include <stddef.h>

class SoundFile
{
public:
    using SampleT = int16_t;

    virtual ~SoundFile() = default;
    virtual uint16_t num_of_channels() const = 0;
    virtual uint32_t sample_rate() const = 0;
    virtual uint32_t byte_rate() const = 0;
    virtual uint16_t bits_per_sample() const = 0;
    virtual size_t read(SampleT *samples, size_t number) = 0;
    virtual void reset() = 0;
};
#endif // SOUND_FILE_HPP