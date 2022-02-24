#include "../../header/audio/audio_synthesizer.hpp"

const unsigned short audio::Synthesizer::NUM_CHANNELS = 2, audio::Synthesizer::SAMPLE_RATE = 48000,
                     audio::Synthesizer::NUM_BUFFERS = 3, audio::Synthesizer::BUFFER_SIZE = 1024 * NUM_CHANNELS;

const std::map<std::string, short> audio::Synthesizer::MIDI_NOTE_ID{{"A", 0}, {"A#", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"D", 5}, {"D#", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"G", 10}, {"G#", 11}};

audio::Note::Note(short id, double pressedTime, double ADSR_releaseTime, double velocity)
{
    this->id = id;
    this->pressedTime = pressedTime;
    this->ADSR_releaseTime = ADSR_releaseTime;
    this->velocity = velocity;
}

double audio::Note::getFrequency(short octave) const
{
    return (27.5 * pow(2, octave)) * pow(1.0594630943592953, id);
}
bool audio::Note::isActive(double time) const
{
    return (releaseTime == -1) || (time < releaseTime + ADSR_releaseTime);
}

audio::Synthesizer::Synthesizer(audio::Waveform *waveform, audio::ADSR *adsr)
{
    this->waveform = waveform;
    this->adsr = adsr;

    device = alcOpenDevice(nullptr);
    context = alcCreateContext(device, nullptr);

    alcMakeContextCurrent(context);
    alDistanceModel(AL_NONE);

    std::vector<int16_t> initialData(1);
    src = new Source();
    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        Buffer *b = new Buffer();
        b->update(initialData, NUM_CHANNELS, SAMPLE_RATE);
        buffers[b->getID()] = b;
    }

    src->queueInit(&buffers);
    src->play();
    runThread = true;
    audioThread = new std::thread(threadFunction, this->src, &this->time, &this->octave, &this->note_arr, this->waveform, this->adsr, &this->stereoPan, &this->bufferData);
}
audio::Synthesizer::~Synthesizer()
{
    if (audioThread)
    {
        runThread = false;
        audioThread->join();
        delete audioThread;
    }

    for (auto b : buffers)
        delete b.second;

    delete src;

    alcDestroyContext(context);
    alcCloseDevice(device);
}

void audio::Synthesizer::setVolume(float volume)
{
    src->setVolume(volume);
}
float audio::Synthesizer::getVolume()
{
    return src->getVolume();
}

double audio::Synthesizer::getTime()
{
    return time;
}

void audio::Synthesizer::addNote(short id, double velocity)
{
    double minPressedTime = time;
    int activeNotes = 0;

    for (auto &n : note_arr)
        if (n.releaseTime == -1)
        {
            if (n.id == id)
            {
                n.releaseTime = time;
                activeNotes = 0;
                break;
            }
            else if (polyphony == 1)
                n.releaseTime = time;

            activeNotes++;
            if (n.pressedTime < minPressedTime)
                minPressedTime = n.pressedTime;
        }

    if (activeNotes >= polyphony)
        for (auto &n : note_arr)
            if (activeNotes >= polyphony && n.pressedTime == minPressedTime)
            {
                n.releaseTime = time;
                activeNotes--;
            }

    note_arr.push_back({id, time, adsr->release_time, velocity});
}
void audio::Synthesizer::releaseNote(short id)
{
    for (auto &n : note_arr)
        if (n.id == id && n.releaseTime == -1)
        {
            n.releaseTime = time;
            return;
        }
}
void audio::Synthesizer::releaseAll()
{
    for (auto &n : note_arr)
        if (n.releaseTime == -1)
            n.releaseTime = time;
}

bool audio::Synthesizer::runThread = false;

void audio::Synthesizer::threadFunction(Source *src, double *time, short *octave, std::vector<Note> *note_arr, audio::Waveform *waveform, audio::ADSR *adsr, double *stereoPan, std::vector<int16_t> *bufferData)
{
    bufferData->resize((BUFFER_SIZE / NUM_CHANNELS) * 4);
    std::vector<int16_t> monoData(BUFFER_SIZE / NUM_CHANNELS), stereoData(NUM_CHANNELS == 2 ? BUFFER_SIZE : 0),
        noteData(monoData.size());
    *time = 0;
    size_t i, j;
    Buffer *b = nullptr;

    while (runThread)
    {
        if (src->isBufferProcessed())
        {
            std::fill(monoData.begin(), monoData.end(), 0);

            for (i = 0; i < note_arr->size(); i++)
            {
                std::fill(noteData.begin(), noteData.end(), 0);
                waveform->fillBuffer(noteData, note_arr->at(i).getFrequency(*octave), *time, SAMPLE_RATE);
                adsr->modifyBuffer(noteData, *time, note_arr->at(i).pressedTime, note_arr->at(i).releaseTime, SAMPLE_RATE);

                for (j = 0; j < monoData.size(); j++)
                    monoData[j] += noteData[j] * note_arr->at(i).velocity;
            }

            if (NUM_CHANNELS == 2)
                for (i = 0; i < stereoData.size(); i += 2)
                {
                    stereoData[i] = monoData[i / 2] * (*stereoPan > 0 ? 1 - *stereoPan : 1);
                    stereoData[i + 1] = monoData[i / 2] * (*stereoPan < 0 ? 1 - abs(*stereoPan) : 1);
                }

            bufferData->erase(bufferData->begin(), bufferData->begin() + monoData.size());
            bufferData->insert(bufferData->end(), monoData.begin(), monoData.end());

            b = src->unqueueProcessedBuffer();
            b->update(NUM_CHANNELS == 2 ? stereoData : monoData, NUM_CHANNELS, SAMPLE_RATE);
            src->queueBuffer(b);

            *time += (BUFFER_SIZE / NUM_CHANNELS) / (double)SAMPLE_RATE;
        }

        for (i = 0; i < note_arr->size(); i++)
            if (!note_arr->at(i).isActive(*time))
                note_arr->erase(note_arr->begin() + i);
    }
}