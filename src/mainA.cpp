#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

const double len = 10.0;
const int sample_rate = 44100;
const double PI = 3.14159265358979323846;

using std::vector;
typedef struct {
    vector<double> freq;
    vector<double> volume;
}waveform;

double set_wave(waveform wavef, double v, double t) {
    if (wavef.freq.size() != wavef.volume.size()) {std::cerr << "vectors don't match!\n"; exit(1);}

    double temp_wave;
    for (int i=0; i < wavef.freq.size(); i++) {
        temp_wave += v * (wavef.volume[i] * sin(2 * PI * (wavef.freq[i]) * (t)));
    }

    //(sin(2 * PI * (f) * (t)) > 0)
    return temp_wave;
}

double play_instrument(waveform wavef, double note, double volume, double t) {
    if (wavef.freq.size() != wavef.volume.size()) {std::cerr << "vectors don't match!\n"; exit(1);}

    for (int i=0; i < wavef.freq.size(); i++) {
        wavef.freq[i] *= pow(2.0, (note/12.0));
    }

    double temp_wave = set_wave(wavef, volume, t);
    double decay = exp(2.0 * t);
    return temp_wave/decay;
}

waveform clar_wavef;
void init_waveforms() {
    clar_wavef.freq = {220, 660, 1100, 1540, 1980, 2420, 2860};
    clar_wavef.volume = {1.0, 0.6, 0.3, 0.15, 0.07, 0.03, 0.015};
}

void write_sample(std::ofstream& OF, short sample) {
    OF.write(reinterpret_cast<const char*>(&sample), sizeof(short));
}

int main() {

    init_waveforms();

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


    for (int i = 0; i < sample_rate * len; ++i) {
        double t = i / static_cast<double>(sample_rate);

        double wave = play_instrument(clar_wavef, 0, 0.11, t);
        
        short sample = static_cast<short>(32767 * wave);
        write_sample(outFile, sample);
    }

    outFile.close();
    return 0;
}