#ifndef AUDIO_SYNTHESIZER
#define AUDIO_SYNTHESIZER

#include "audio_utility.hpp"
#include "audio_waveform.hpp"
#include "audio_adsr.hpp"
#include <thread>

namespace audio
{
    struct Note
    {
        short id;
        double pressedTime, releaseTime = -1, ADSR_releaseTime, velocity;

        Note(short id, double pressedTime, double ADSR_releaseTime, double velocity);

        double getFrequency(short octave) const;
        bool isActive(double time) const;
    };

    class Synthesizer
    {
    private:
        static bool runThread;
        static void threadFunction(Source *src, double *time, short *octave, std::vector<Note> *note_arr, audio::Waveform *waveform, audio::ADSR *adsr, double *stereoPan, std::vector<int16_t> *bufferData);

        ALCdevice *device = nullptr;
        ALCcontext *context = nullptr;
        Source *src = nullptr;
        std::map<unsigned int, Buffer *> buffers;
        std::thread *audioThread = nullptr;

        double time;
        std::vector<Note> note_arr;

        audio::Waveform *waveform = nullptr;
        audio::ADSR *adsr = nullptr;

    public:
        static const unsigned short NUM_BUFFERS, BUFFER_SIZE, NUM_CHANNELS, SAMPLE_RATE;
        static const std::map<std::string, short> MIDI_NOTE_ID;

        short octave = 2, polyphony = 7;
        double stereoPan = 0;
        std::vector<int16_t> bufferData;

        Synthesizer(audio::Waveform *waveform, audio::ADSR *adsr);
        ~Synthesizer();

        void setVolume(float volume);
        float getVolume();

        double getTime();

        void addNote(short id, double velocity = 1);
        void releaseNote(short id);
        void releaseAll();
    };
}

#endif