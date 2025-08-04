#include <iostream>
#include <fstream>
#include <cmath>

const double len = 1.0;
const int sample_rate = 44100;
const double PI = 3.14159265358979323846;

void write_sample(std::ofstream& OF, short sample) {
    OF.write(reinterpret_cast<const char*>(&sample), sizeof(short));
}

int main() {
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

    double freq = 440.0;
    double volume = 0.5;

    for (int i = 0; i < sample_rate * len; ++i) {
        double t = i / static_cast<double>(sample_rate);
        double wave = sin(2 * PI * freq * t);
        short sample = static_cast<short>(volume * 32767 * wave);
        write_sample(outFile, sample);
    }

    outFile.close();
    return 0;
}