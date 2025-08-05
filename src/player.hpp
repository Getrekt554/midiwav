#include <iostream>
#include <filesystem>
#include <vector>
#include <variant>
#include <fstream>
#include <cmath>
#include <sstream>
#include <algorithm>

using NoteOrChord = std::variant<double, std::vector<double>>;

struct song {
    std::vector<NoteOrChord> notes;
    std::vector<double> len;
};

typedef enum {
    SINE,
    SQUARE,
    TRIANGLE,
    SAW,
    UNKNOWN
} wave_types;

struct waveform {
    std::vector<double> freq;
    std::vector<double> volume;
    wave_types type;
};

const int sample_rate = 44100;
const double PI = 3.14159265358979323846;

double len = 0.0;
song current_song;
waveform current_waveform;
double BPM = 120.0;
std::string selected_waveform;
std::string selected_song;

wave_types str_to_wavef(const std::string& str) {
    if (str == "SINE") return SINE;
    else if (str == "SQUARE") return SQUARE;
    else if (str == "TRIANGLE") return TRIANGLE;
    else if (str == "SAW") return SAW;
    return UNKNOWN;
}

std::vector<NoteOrChord> parse_notes_line(const std::string& line) {
    std::vector<NoteOrChord> result;
    std::string inner = line.substr(1, line.size() - 2);
    std::stringstream ss(inner);
    std::string token;

    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        std::vector<double> chord_notes;
        size_t pos = 0;
        size_t last_pos = 0;
        while (pos < token.size()) {
            if (pos != 0 && token[pos] == '-') {
                // Check if this '-' is a separator or negative sign
                char prev_char = token[pos - 1];
                char next_char = (pos + 1 < token.size()) ? token[pos + 1] : '\0';

                bool prev_is_digit = (prev_char >= '0' && prev_char <= '9') || prev_char == '.';
                bool next_is_digit_or_minus = (next_char >= '0' && next_char <= '9') || next_char == '-';

                if (prev_is_digit && next_is_digit_or_minus) {
                    // Separator found, parse substring from last_pos to pos
                    std::string part = token.substr(last_pos, pos - last_pos);
                    part.erase(0, part.find_first_not_of(" \t"));
                    part.erase(part.find_last_not_of(" \t") + 1);
                    chord_notes.push_back(std::stod(part));
                    last_pos = pos + 1;
                }
            }
            pos++;
        }
        // push last number in token
        std::string last_part = token.substr(last_pos);
        last_part.erase(0, last_part.find_first_not_of(" \t"));
        last_part.erase(last_part.find_last_not_of(" \t") + 1);
        chord_notes.push_back(std::stod(last_part));

        if (chord_notes.size() == 1) {
            result.push_back(chord_notes[0]);
        } else {
            result.push_back(chord_notes);
        }
    }

    return result;
}

std::vector<double> parse_len_line(const std::string& line) {
    std::vector<double> result;
    std::string inner = line.substr(1, line.size() - 2);
    std::stringstream ss(inner);
    std::string value;

    while (std::getline(ss, value, ',')) {
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        result.push_back(std::stod(value));
    }

    return result;
}

double set_wave(const waveform& wavef, double volume, double t, double note) {
    if (wavef.freq.size() != wavef.volume.size()) exit(1);

    double temp_wave = 0.0;
    for (size_t i = 0; i < wavef.freq.size(); i++) {
        double freq = wavef.freq[i] * std::pow(2.0, note / 12.0);
        switch (wavef.type) {
            case SINE:
                temp_wave += volume * (wavef.volume[i] * sin(2.0 * PI * freq * t));
                break;
            case SQUARE: {
                double s = sin(2.0 * PI * freq * t);
                temp_wave += volume * (wavef.volume[i] * (s >= 0.0 ? 1.0 : -1.0));
                break;
            }
            case TRIANGLE: {
                double phase = freq * t;
                double tri = 2.0 * fabs(2.0 * (phase - floor(phase + 0.5))) - 1.0;
                temp_wave += volume * (wavef.volume[i] * tri);
                break;
            }
            case SAW: {
                double phase = fmod(t * freq, 1.0);
                double saw = 2.0 * phase - 1.0;
                temp_wave += volume * (wavef.volume[i] * saw);
                break;
            }
            default:
                break;
        }
    }
    return temp_wave;
}

void write_sample(std::ofstream& OF, short sample) {
    OF.write(reinterpret_cast<const char*>(&sample), sizeof(short));
}

void init_player(const std::string& song) {
    selected_song = song;

    std::string filename = selected_song + ".txt";
    std::filesystem::path dir = "../src/song_data/songs/";
    std::filesystem::path songfile = dir / filename;

    if (!std::filesystem::exists(songfile)) exit(1);

    std::ifstream file(songfile);
    if (!file) exit(1);

    int line_count = 1;
    std::string line;
    while (std::getline(file, line)) {
        switch (line_count) {
            case 1:
                if (line.rfind("BPM ", 0) == 0) BPM = std::stod(line.substr(4));
                else exit(1);
                break;
            case 2:
                if (line.rfind("USE ", 0) == 0) selected_waveform = line.substr(4);
                else exit(1);
                break;
            case 3:
                current_song.notes = parse_notes_line(line);
                break;
            case 4:
                current_song.len = parse_len_line(line);
                break;
        }
        line_count++;
    }
    file.close();

    std::string wavef_filename = selected_waveform + ".txt";
    std::filesystem::path wavef_dir = "../src/song_data/waveforms/";
    std::filesystem::path wavef_songfile = wavef_dir / wavef_filename;

    if (!std::filesystem::exists(wavef_songfile)) exit(1);

    std::ifstream wavef_file(wavef_songfile);
    if (!wavef_file) exit(1);

    int wavef_line_count = 1;
    std::string wavef_line;
    while (std::getline(wavef_file, wavef_line)) {
        switch (wavef_line_count) {
            case 1:
                if (wavef_line.rfind("TYPE", 0) == 0)
                    current_waveform.type = str_to_wavef(wavef_line.substr(5));
                else
                    exit(1);
                break;
            case 2:
                current_waveform.freq = parse_len_line(wavef_line);
                break;
            case 3:
                current_waveform.volume = parse_len_line(wavef_line);
                break;
        }
        wavef_line_count++;
    }
    wavef_file.close();

    size_t min_size = std::min(current_song.len.size(), current_song.notes.size());
    len = 0.0;
    for (size_t i = 0; i < min_size; ++i) {
        current_song.len[i] = (60.0 / BPM) * current_song.len[i];
        len += current_song.len[i];
    }
}

void write_wav(std::ofstream& outFile) {
    for (size_t i = 0; i < current_song.notes.size(); i++) {
        double duration = current_song.len[i];
        size_t samples = static_cast<size_t>(sample_rate * duration);

        for (size_t j = 0; j < samples; j++) {
            double t = j / static_cast<double>(sample_rate);
            double final_wave = 0.0;

            const auto& note_or_chord = current_song.notes[i];
            if (std::holds_alternative<double>(note_or_chord)) {
                double note = std::get<double>(note_or_chord);
                final_wave += set_wave(current_waveform, 0.11, t, note);
            } else {
                const auto& chord_notes = std::get<std::vector<double>>(note_or_chord);
                for (double note : chord_notes) {
                    final_wave += set_wave(current_waveform, 0.11, t, note);
                }
                final_wave /= chord_notes.size();
            }
            
            short sample = static_cast<short>(32767 * final_wave);
            write_sample(outFile, sample);
        }
    }
}
