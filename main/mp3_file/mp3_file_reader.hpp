#pragma once

#include <vector>
#include "sound_file.hpp"
#include "minimp3.h"

class Mp3File : public SoundFile
{
public:
    Mp3File(const uint8_t *data, size_t size);

    eSoundFiles soundFileType() const override {
        return eSoundFiles::SOUND_FILE_MP3;
    }

    SampleChunk getNextSampleChunk() override;

    void reset() {
        data_offset_ = 0;
    }

private:
    static constexpr const char* const TAG{"Mp3File"};
    SampleT* samples_{nullptr};  
    mp3dec_t mp3d_{};
    mp3dec_frame_info_t frame_info_{};
    const uint8_t *data_{nullptr};
    size_t data_offset_{};
    SampleChunk curr_chunk_{};
};
