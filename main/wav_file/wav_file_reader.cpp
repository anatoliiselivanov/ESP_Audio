#include <string.h>
#include "wav_file_reader.hpp"

WavFile::WavFile(const uint8_t *data, size_t size) {
    memcpy(&header_, data, sizeof(wav_header_t));
    data_size_ = (size - sizeof(wav_header_t)) / sizeof(SampleT);
    data_ = reinterpret_cast<const SampleT *>(data) + sizeof(wav_header_t);
}
