#include "wav_file_reader.hpp"

#include <string.h>

WavFileReader::WavFileReader(const char *file_path)
{
    m_file_path = file_path;
    m_file = fopen(m_file_path, "rb");

    if (m_file == NULL)
    {
        return;
    }

    read_header();

    m_data_size = m_header.data_chunk.subchunk_size / sizeof(uint16_t);
    m_data = new uint16_t[m_data_size];

    read_data();

    fclose(m_file);
}

WavFileReader::WavFileReader(const uint8_t *data, size_t size)
{
    memcpy(&m_header, data, sizeof(wav_header_t));

    m_data_size = size - sizeof(wav_header_t);
    m_data = new uint16_t[m_data_size];
    memcpy(m_data, data + sizeof(wav_header_t), m_data_size);
}

void WavFileReader::read_header()
{
    fread(&m_header, sizeof(wav_header_t), 1, m_file);
}

size_t WavFileReader::read_data()
{
    fseek(m_file, sizeof(wav_header_t), SEEK_SET);

    size_t read_size = fread(m_data, sizeof(uint16_t), m_header.data_chunk.subchunk_size / sizeof(uint16_t), m_file);

    return read_size;
}
