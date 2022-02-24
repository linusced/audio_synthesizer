#include "../../header/audio/audio_utility.hpp"

// audio::Buffer

audio::Buffer::Buffer()
{
    alGenBuffers(1, &id);
}
audio::Buffer::~Buffer()
{
    alDeleteBuffers(1, &id);
}

void audio::Buffer::update(const std::vector<int16_t> &bufferData, unsigned char channels, unsigned int sampleRate)
{
    alBufferData(id, (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, bufferData.data(), bufferData.size() * sizeof(bufferData[0]), sampleRate);
}

unsigned int audio::Buffer::getID()
{
    return id;
}

// audio::Source

audio::Source::Source()
{
    alGenSources(1, &id);
    alSourcef(id, AL_GAIN, 1);
}
audio::Source::~Source()
{
    stop();
    alDeleteSources(1, &id);
}

void audio::Source::queueInit(std::map<unsigned int, Buffer *> *buffers)
{
    this->buffers = buffers;
    std::vector<unsigned int> bids;
    for (auto it = buffers->begin(); it != buffers->end(); it++)
        bids.push_back(it->first);

    alSourceQueueBuffers(id, bids.size(), bids.data());
}
void audio::Source::queueInit()
{
    std::vector<unsigned int> bids;
    for (auto it = buffers->begin(); it != buffers->end(); it++)
        bids.push_back(it->first);

    alSourceQueueBuffers(id, bids.size(), bids.data());
}
void audio::Source::queueReset()
{
    std::vector<unsigned int> bids;
    for (auto it = buffers->begin(); it != buffers->end(); it++)
        bids.push_back(it->first);

    alSourceUnqueueBuffers(id, bids.size(), bids.data());
}
void audio::Source::queueBuffer(Buffer *b)
{
    unsigned int bid = b->getID();
    if (!buffers->contains(bid))
        buffers->operator[](bid) = b;

    alSourceQueueBuffers(id, 1, &bid);
}

audio::Buffer *audio::Source::unqueueProcessedBuffer()
{
    unsigned int result;
    alSourceUnqueueBuffers(id, 1, &result);
    return buffers->at(result);
}
bool audio::Source::isBufferProcessed()
{
    int result;
    alGetSourcei(id, AL_BUFFERS_PROCESSED, &result);
    return result > 0;
}

void audio::Source::play()
{
    alSourcePlay(id);
}
void audio::Source::stop()
{
    alSourceStop(id);
}
bool audio::Source::isPlaying()
{
    ALenum result;
    alGetSourcei(id, AL_SOURCE_STATE, &result);
    return result == AL_PLAYING;
}

void audio::Source::setGain(float gain)
{
    if (gain < 0)
        gain = 0;
    else if (gain > 1)
        gain = 1;
    this->gain = gain;
    alSourcef(id, AL_GAIN, volume * gain);
}
void audio::Source::setVolume(float volume)
{
    if (volume < 0)
        volume = 0;
    else if (volume > 1)
        volume = 1;
    this->volume = volume;
    alSourcef(id, AL_GAIN, volume * gain);
}
float audio::Source::getVolume()
{
    return volume;
}

unsigned int audio::Source::getID()
{
    return id;
}

// audio::File

audio::File::File(std::string filePath)
{
    alutLoadWAVFile((char *)filePath.c_str(), &format, (void **)&data, &size, &sampleRate);
    size /= sizeof(data[0]);
}
audio::File::~File()
{
    delete data;
}

unsigned int audio::File::getChannels()
{
    return (format == AL_FORMAT_STEREO8 || format == AL_FORMAT_STEREO16) ? 2 : 1;
}
unsigned int audio::File::getSampleRate()
{
    return sampleRate;
}
unsigned int audio::File::getSize()
{
    return size;
}
double audio::File::getDuration()
{
    return (size / getChannels()) / (double)sampleRate;
}

std::vector<int16_t> &audio::File::getData(unsigned int index, unsigned int size, bool *endOfFile)
{
    if (endOfFile)
        *endOfFile = false;

    unsigned int audioFileSize = getSize(), index_size = index + size;
    if (dataSlice.size() != size)
        dataSlice.resize(size);

    for (int i = index; i < index_size; i++)
        if (i < audioFileSize)
        {
            dataSlice[i - index] = data[i];
        }
        else
        {
            dataSlice[i - index] = 0;
            if (endOfFile)
                *endOfFile = true;
        }

    return dataSlice;
}