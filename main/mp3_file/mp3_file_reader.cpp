#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_STDIO

#include "mp3_file_reader.hpp"

#include <stdio.h>

const int BUFFER_SIZE = 1024;

Mp3FileReader::Mp3FileReader(const uint8_t *data, size_t size)
{
    m_data = data;
    m_data_size = size;

    // Get info from the mp3 file
    mp3dec_decode_frame(&mp3d, m_data, BUFFER_SIZE, nullptr, &info);

    // mp3 decoder state
    mp3dec_init(&mp3d);
}

size_t Mp3FileReader::read(SampleT *samples, size_t number)
{
    size_t read_size = number * sizeof(SampleT);

    if (m_data_offset + read_size > m_data_size)
    {
        read_size = m_data_size - m_data_offset;
    }

    mp3dec_decode_frame(&mp3d, m_data + m_data_offset, number * sizeof(SampleT), samples, &info);
    m_data_offset += read_size;

    return read_size / sizeof(SampleT);
}

size_t Mp3FileReader::read()
{
}

// void play_task(void *param)
// {
//     Output *output = new I2SOutput(I2S_NUM_0, i2s_speaker_pins);

//     // setup for the mp3 decoded
//     short *pcm = (short *)malloc(sizeof(short) * MINIMP3_MAX_SAMPLES_PER_FRAME);
//     uint8_t *input_buf = (uint8_t *)malloc(BUFFER_SIZE);

//     // mp3 decoder state
//     mp3dec_t mp3d = {};
//     mp3dec_init(&mp3d);
//     mp3dec_frame_info_t info = {};

//     // keep track of how much data we have buffered, need to read and decoded
//     int to_read = BUFFER_SIZE;
//     int buffered = 0;
//     int decoded = 0;
//     bool is_output_started = false;

//     // this assumes that you have uploaded the mp3 file to the SPIFFS
//     FILE *fp = fopen("/fs/test.mp3", "r");

//     while (1)
//     {

//         // read in the data that is needed to top up the buffer
//         size_t n = fread(input_buf + buffered, 1, to_read, fp);

//         buffered += n;
//         if (buffered == 0)
//         {
//             // we've reached the end of the file and processed all the buffered data
//             output->stop();
//             is_output_started = false;
//             break;
//         }

//         // decode the next frame
//         int samples = mp3dec_decode_frame(&mp3d, input_buf, buffered, pcm, &info);

//         // we've processed this may bytes from teh buffered data
//         buffered -= info.frame_bytes;

//         // shift the remaining data to the front of the buffer
//         memmove(input_buf, input_buf + info.frame_bytes, buffered);

//         // we need to top up the buffer from the file
//         to_read = info.frame_bytes;
//         if (samples > 0)
//         {
//             // if we haven't started the output yet we can do it now as we now know the sample rate and number of channels
//             if (!is_output_started)
//             {
//                 output->start(info.hz);
//                 is_output_started = true;
//             }

//             // if we've decoded a frame of mono samples convert it to stereo by duplicating the left channel
//             // we can do this in place as our samples buffer has enough space
//             if (info.channels == 1)
//             {
//                 for (int i = samples - 1; i >= 0; i--)
//                 {
//                     pcm[i * 2] = pcm[i];
//                     pcm[i * 2 - 1] = pcm[i];
//                 }
//             }

//             // write the decoded samples to the I2S output
//             output->write(pcm, samples);

//             // keep track of how many samples we've decoded
//             decoded += samples;
//         }
//     }

//     fclose(fp);
// }