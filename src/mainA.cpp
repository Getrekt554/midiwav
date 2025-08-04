#include "raylib.h"
#include <cmath>

int main() {
    InitAudioDevice();

    int sampleRate = 44100;
    int sampleCount = sampleRate;
    float frequency = 440.0f;

    short *samples = new short[sampleCount];
    for (int i = 0; i < sampleCount; i++)
        samples[i] = (short)(32000 * sin(2 * PI * frequency * i / sampleRate));

    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 16,
        .channels = 1,
        .data = samples
    };

    Sound sound = LoadSoundFromWave(wave);
    PlaySound(sound);

    while (IsSoundPlaying(sound)) {}

    UnloadSound(sound);
    CloseAudioDevice();
    delete[] samples;
}
