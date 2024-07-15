#pragma once

#include "format_wav.h"
#include "sound_file.hpp"

class WavFile : public SoundFile {
public:
    WavFile(const uint8_t *data, size_t size);
    
    SampleChunk getNextSampleChunk() override {
        SampleChunk ret {
            .samples = data_,
            .length = data_size_,
            .sample_rate = header_.fmt_chunk.sample_rate
        };
        return ret;
    }

    eSoundFiles soundFileType() const override {
        return eSoundFiles::SOUND_FILE_WAV;
    }

    void reset() {

    }
private:
    wav_header_t header_{};
    const SampleT *data_{};
    size_t data_size_{};
};
