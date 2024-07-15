#pragma once

#include <stdint.h>
#include <stddef.h>

using SampleT = int16_t;

enum eSoundFiles {
    SOUND_FILE_MP3,
    SOUND_FILE_WAV
};

struct SampleChunk {
    const SampleT* samples;
    size_t length;
    uint32_t sample_rate;
};

class SoundFile
{
public:
    virtual SampleChunk getNextSampleChunk() = 0;
    virtual eSoundFiles soundFileType() const = 0;
    virtual ~SoundFile() = default;
    virtual void reset() = 0;
};
