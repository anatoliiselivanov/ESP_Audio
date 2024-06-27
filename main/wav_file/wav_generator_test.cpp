#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>

const int SAMPLE_RATE = 44100;
const int BITS_PER_SAMPLE = 16;
const int NUM_CHANNELS = 1;
const double PI = 3.14159265358979323846;

struct Note
{
    double frequency;
    double duration;
};

const Note notes[] = {
    {261.63, 0.5}, {261.63, 0.5}, {392.00, 0.5}, {392.00, 0.5}, {440.00, 0.5}, {440.00, 0.5}, {392.00, 1.0}, {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5}, {293.66, 0.5}, {293.66, 0.5}, {261.63, 1.0}, {392.00, 0.5}, {392.00, 0.5}, {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5}, {293.66, 1.0}, {392.00, 0.5}, {392.00, 0.5}, {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5}, {293.66, 1.0}, {261.63, 0.5}, {261.63, 0.5}, {392.00, 0.5}, {392.00, 0.5}, {440.00, 0.5}, {440.00, 0.5}, {392.00, 1.0}, {349.23, 0.5}, {349.23, 0.5}, {329.63, 0.5}, {329.63, 0.5}, {293.66, 0.5}, {293.66, 0.5}, {261.63, 1.0}};

void write_wav_header(std::vector<char> &buffer, int sample_rate, int bits_per_sample, int num_channels, int num_samples)
{
    int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    int block_align = num_channels * bits_per_sample / 8;
    int data_size = num_samples * num_channels * bits_per_sample / 8;

    buffer.insert(buffer.end(), {'R', 'I', 'F', 'F'});
    int chunk_size = 36 + data_size;
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&chunk_size), reinterpret_cast<const char *>(&chunk_size) + 4);
    buffer.insert(buffer.end(), {'W', 'A', 'V', 'E'});
    buffer.insert(buffer.end(), {'f', 'm', 't', ' '});
    int subchunk1_size = 16;
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&subchunk1_size), reinterpret_cast<const char *>(&subchunk1_size) + 4);
    short audio_format = 1;
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&audio_format), reinterpret_cast<const char *>(&audio_format) + 2);
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&num_channels), reinterpret_cast<const char *>(&num_channels) + 2);
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&sample_rate), reinterpret_cast<const char *>(&sample_rate) + 4);
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&byte_rate), reinterpret_cast<const char *>(&byte_rate) + 4);
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&block_align), reinterpret_cast<const char *>(&block_align) + 2);
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&bits_per_sample), reinterpret_cast<const char *>(&bits_per_sample) + 2);
    buffer.insert(buffer.end(), {'d', 'a', 't', 'a'});
    buffer.insert(buffer.end(), reinterpret_cast<const char *>(&data_size), reinterpret_cast<const char *>(&data_size) + 4);
}

void write_note_to_buffer(std::vector<char> &buffer, double frequency, double duration, int sample_rate, int bits_per_sample)
{
    int num_samples = static_cast<int>(sample_rate * duration);
    double amplitude = 32760; // Max amplitude for 16-bit audio
    for (int i = 0; i < num_samples; ++i)
    {
        double sample = amplitude * sin(2 * PI * frequency * i / sample_rate);
        short int sample_int = static_cast<short int>(sample);
        buffer.insert(buffer.end(), reinterpret_cast<const char *>(&sample_int), reinterpret_cast<const char *>(&sample_int) + sizeof(short int));
    }
}

std::vector<char> generate_wav_file()
{
    std::vector<char> wav_buffer;

    int total_samples = 0;
    for (const auto &note : notes)
    {
        total_samples += static_cast<int>(SAMPLE_RATE * note.duration);
    }

    write_wav_header(wav_buffer, SAMPLE_RATE, BITS_PER_SAMPLE, NUM_CHANNELS, total_samples);

    for (const auto &note : notes)
    {
        write_note_to_buffer(wav_buffer, note.frequency, note.duration, SAMPLE_RATE, BITS_PER_SAMPLE);
    }

    return wav_buffer;
}
