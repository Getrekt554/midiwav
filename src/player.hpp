#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <cmath>

std::vector<double> parse_array_line(const std::string& line) {
    std::vector<double> result;
    std::string inner = line.substr(1, line.size() - 2); // remove '{' and '}'
    std::stringstream ss(inner);
    std::string value;

    while (std::getline(ss, value, ',')) {
        result.push_back(std::stod(value));
    }

    return result;
}

double len = 10.0;
const int sample_rate = 44100;
const double PI = 3.14159265358979323846;

using std::vector;

struct song{
    vector<double> notes;
    vector<double> len;
};

typedef enum {
    SINE,
    SQUARE,
    TRIANGLE,
    UNKNOWN
}wave_types;

typedef struct {
    vector<double> freq;
    vector<double> volume;
    wave_types type;
}waveform;

wave_types str_to_wavef(const std::string& str) {
    if (str == "SINE") {return SINE;}
    else if (str == "SQUARE") {return SQUARE;}
    else if (str == "TRIANGLE") {return TRIANGLE;}

    return UNKNOWN;
}

double set_wave(waveform wavef, double v, double t) {
    if (wavef.freq.size() != wavef.volume.size()) {std::cerr << "vectors don't match!\n"; exit(1);}

    double temp_wave = 0.0;
    for (int i=0; i < wavef.freq.size(); i++) {
        switch (wavef.type)
        {
        case SINE:
            temp_wave += v * (wavef.volume[i] * sin(2.0 * PI * (wavef.freq[i]) * t));
            break;
        case SQUARE:
            {
                double s = sin(2.0 * PI * (wavef.freq[i]) * t);
                temp_wave += v * (wavef.volume[i] * (s >= 0.0 ? 1.0 : -1.0));
            }
            break;
        case TRIANGLE:
            {
                double phase = wavef.freq[i] * t;
                double tri = 2.0 * fabs(2.0 * (phase - floor(phase + 0.5))) - 1.0;
                temp_wave += v * (wavef.volume[i] * tri);
            }
            break;
        default:
            break;
        }
    }
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

song current_song;
std::string selected_song;
double BPM = 120.0;

waveform current_waveform;
std::string selected_waveform;

void init_player(std::string song) {
    selected_song = song;

    //song
    std::string filename = std::string(selected_song) + ".txt";
    std::filesystem::path dir = "../src/song_data/songs/";
    std::filesystem::path songfile = dir / filename;

    if (std::filesystem::exists(songfile)) {
        std::ifstream file(songfile);

        if (!file) {std::cerr << "Failed to open file\n";exit(1);}
        
        int line_count = 1;
        std::string line;
        std::string wavef_name;
        while (std::getline(file, line)) {
            switch (line_count) {
                case 1:
                    if (line.rfind("USE ", 0) == 0) {
                        selected_waveform = line.substr(4);
                    } else {std::cerr << "File notation incorrect\n"; exit(1);}

                    break;
                case 2:
                    if (line.rfind("BPM ", 0) == 0) {
                        BPM = std::stoi(line.substr(4));
                    } else {std::cerr << "File notation incorrect\n"; exit(1);}

                    break;
                case 3:
                    current_song.notes = parse_array_line(line);
                    break;
                case 4:
                    current_song.len = parse_array_line(line);
                    break;
            }
            line_count++;
        }
        file.close();
    } else {std::cout << "File not found.\n";}

    //wavef
    std::string wavef_filename = std::string(selected_waveform) + ".txt";
    std::filesystem::path wavef_dir = "../src/song_data/waveforms/";
    std::filesystem::path wavef_songfile = wavef_dir / wavef_filename;

    if (std::filesystem::exists(wavef_songfile)) {
        std::ifstream wavef_file(wavef_songfile);

        if (!wavef_file) {std::cerr << "Failed to open wavef_file\n"; exit(1);}

        int wavef_line_count = 1;
        std::string wavef_line;
        while (std::getline(wavef_file, wavef_line)) {
            switch (wavef_line_count) {
                case 1:
                    if (wavef_line.rfind("TYPE", 0) == 0) {
                        current_waveform.type = str_to_wavef(wavef_line.substr(5));
                    } else {std::cerr << "File notation incorrect\n"; exit(1);}
                    break;
                case 2:
                    current_waveform.freq = parse_array_line(wavef_line);
                    break;
                case 3:
                    current_waveform.volume = parse_array_line(wavef_line);
                    break;
            }
            wavef_line_count++;
        }
    }

    for (int i = 0; i < current_song.notes.size(); i++) {
        double temp = current_song.len.at(i);
        current_song.len.at(i) = (60.0/BPM) * temp;

        len += current_song.len.at(i);
    }
}

void write_sample(std::ofstream& OF, short sample) {
    OF.write(reinterpret_cast<const char*>(&sample), sizeof(short));
}

void write_wav(std::ofstream& outFile) {
    for (int i = 0; i < current_song.notes.size(); i++) {
        for (int j = 0; j < sample_rate * current_song.len.at(i); ++j) {
            double t = j / static_cast<double>(sample_rate);

            double wave = play_instrument(current_waveform, current_song.notes.at(i) - 12, 0.11, t);
            
            short sample = static_cast<short>(32767 * wave);
            write_sample(outFile, sample);
        }
    }
}