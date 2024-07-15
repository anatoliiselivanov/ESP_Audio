#include "wav_file_reader.hpp"

#include <string.h>

WavFileReader::WavFileReader(const uint8_t *data, size_t size)
{
    memcpy(&m_header, data, sizeof(wav_header_t));
    m_data_size = (size - sizeof(wav_header_t)) / sizeof(SampleT);
    m_data = reinterpret_cast<const SampleT *>(data) + sizeof(wav_header_t);
}

size_t WavFileReader::read(SampleT *samples, size_t number)
{
    size_t read_size = number * sizeof(SampleT);

    if (m_data_offset + read_size > m_data_size * sizeof(SampleT))
    {
        read_size = m_data_size * sizeof(SampleT) - m_data_offset;
    }

    memcpy(samples, m_data + m_data_offset / sizeof(SampleT), read_size);
    m_data_offset += read_size;

    return read_size / sizeof(SampleT);
}