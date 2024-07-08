#include "wav_file_reader.hpp"

#include <string.h>

WavFileReader::WavFileReader(const uint8_t *data, size_t size)
{
    memcpy(&m_header, data, sizeof(wav_header_t));
    m_data_size = (size - sizeof(wav_header_t)) / sizeof(SampleT);
    m_data = reinterpret_cast<const SampleT*>(data) + sizeof(wav_header_t);
}
