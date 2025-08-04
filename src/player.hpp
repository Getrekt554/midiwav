#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

double len = 10.0;
const int sample_rate = 44100;
const double PI = 3.14159265358979323846;

using std::vector;

struct song{
    vector<int> notes;
    vector<double> len;
};

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
song leib;

void init_player() {
    clar_wavef.freq = {220, 660, 1100, 1540, 1980, 2420, 2860};
    clar_wavef.volume = {1.0, 0.6, 0.3, 0.15, 0.07, 0.03, 0.015};

    leib.notes = {0, 1, 2, 3, 4, 5};
    leib.len = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    for (int i = 0; i < leib.notes.size(); i++) {
        len += leib.len.at(i);
    }
}

void write_sample(std::ofstream& OF, short sample) {
    OF.write(reinterpret_cast<const char*>(&sample), sizeof(short));
}

void write_wav(std::ofstream& outFile) {
    for (int i = 0; i < leib.notes.size(); i++) {
        for (int j = 0; j < sample_rate * leib.len.at(i); ++j) {
            double t = j / static_cast<double>(sample_rate);

            double wave = play_instrument(clar_wavef, leib.notes.at(i), 0.11, t);
            
            short sample = static_cast<short>(32767 * wave);
            write_sample(outFile, sample);
        }
    }
}