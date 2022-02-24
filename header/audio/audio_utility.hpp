#ifndef AUDIO_UTILITY
#define AUDIO_UTILITY

#include "audio_common.hpp"

namespace audio
{
    class Buffer
    {
    private:
        unsigned int id;

    public:
        Buffer();
        ~Buffer();

        void update(const std::vector<int16_t> &bufferData, unsigned char channels, unsigned int sampleRate);

        unsigned int getID();
    };

    class Source
    {
    private:
        unsigned int id;
        std::map<unsigned int, Buffer *> *buffers = nullptr;
        float volume = 1, gain = 1;

    public:
        Source();
        ~Source();

        void queueInit(std::map<unsigned int, Buffer *> *buffers);
        void queueInit();
        void queueReset();
        void queueBuffer(Buffer *b);

        Buffer *unqueueProcessedBuffer();
        bool isBufferProcessed();

        void play();
        void stop();
        bool isPlaying();
        void setGain(float gain);
        void setVolume(float volume);
        float getVolume();

        unsigned int getID();
    };

    class File
    {
    private:
        int16_t *data = nullptr;
        ALenum format;
        int size, sampleRate;
        std::vector<int16_t> dataSlice;

    public:
        File(std::string filePath);
        ~File();

        unsigned int getChannels();
        unsigned int getSampleRate();
        unsigned int getSize();
        double getDuration();

        std::vector<int16_t> &getData(unsigned int index, unsigned int size, bool *endOfFile);
    };
}

#endif