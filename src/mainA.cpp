#include "player.hpp"

int main() {

    init_player("leib");

    std::ofstream outFile("out.wav", std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Couldnt open file.\n";
        exit(1);
    }
    int num_channels = 1;
    int bits_per_sample = 16;
    int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    int block_align = num_channels * bits_per_sample / 8;
    int d_size = static_cast<int>(ceil(len) * sample_rate * num_channels * bits_per_sample / 8);
    int f_size = 36 + d_size;
    outFile.write("RIFF", 4);
    outFile.write(reinterpret_cast<const char*>(&f_size), 4);
    outFile.write("WAVE", 4);
    outFile.write("fmt ", 4);
    int fmt_chunk_size = 16;
    outFile.write(reinterpret_cast<const char*>(&fmt_chunk_size), 4);
    short audio_format = 1;
    outFile.write(reinterpret_cast<const char*>(&audio_format), 2);
    short num_channels_s = num_channels;
    outFile.write(reinterpret_cast<const char*>(&num_channels_s), 2);
    outFile.write(reinterpret_cast<const char*>(&sample_rate), 4);
    outFile.write(reinterpret_cast<const char*>(&byte_rate), 4);
    short block_align_s = block_align;
    outFile.write(reinterpret_cast<const char*>(&block_align_s), 2);
    short bits_per_sample_s = bits_per_sample;
    outFile.write(reinterpret_cast<const char*>(&bits_per_sample_s), 2);
    outFile.write("data", 4);
    outFile.write(reinterpret_cast<const char*>(&d_size), 4);

    write_wav(outFile);

    outFile.close();
    return 0;
}