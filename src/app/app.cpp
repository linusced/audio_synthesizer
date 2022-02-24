#include "../../header/app/app.hpp"

const std::vector<std::vector<int>> App::frequencyDistribution{{20, 30}, {250, 200}, {500, 400}, {1000, 550}, {2000, 700}, {4000, 850}, {10000, 990}, {24000, 1000}};

App::App(GLFWwindow *window, int *windowWidth, int *windowHeight)
    : DrawLoop(window, windowWidth, windowHeight)
{
    waveform.amplitude = .14;

    _gui = new AppGUI(&waveform, &adsr, frequencyDistribution, window, windowWidth, windowHeight);

    threadMIDI = new std::thread(createMIDI, std::ref(inputMIDI), &_gui->portsMIDI);

    keysMIDI[GLFW_KEY_A] = synth->MIDI_NOTE_ID.at("C");
    keysMIDI[GLFW_KEY_W] = synth->MIDI_NOTE_ID.at("C#");

    keysMIDI[GLFW_KEY_S] = synth->MIDI_NOTE_ID.at("D");
    keysMIDI[GLFW_KEY_E] = synth->MIDI_NOTE_ID.at("D#");

    keysMIDI[GLFW_KEY_D] = synth->MIDI_NOTE_ID.at("E");

    keysMIDI[GLFW_KEY_F] = synth->MIDI_NOTE_ID.at("F");
    keysMIDI[GLFW_KEY_T] = synth->MIDI_NOTE_ID.at("F#");

    keysMIDI[GLFW_KEY_G] = synth->MIDI_NOTE_ID.at("G");
    keysMIDI[GLFW_KEY_Y] = synth->MIDI_NOTE_ID.at("G#");

    keysMIDI[GLFW_KEY_H] = synth->MIDI_NOTE_ID.at("A") + 12;
    keysMIDI[GLFW_KEY_U] = synth->MIDI_NOTE_ID.at("A#") + 12;

    keysMIDI[GLFW_KEY_J] = synth->MIDI_NOTE_ID.at("B") + 12;

    keysMIDI[GLFW_KEY_K] = synth->MIDI_NOTE_ID.at("C") + 12;
    keysMIDI[GLFW_KEY_O] = synth->MIDI_NOTE_ID.at("C#") + 12;

    keysMIDI[GLFW_KEY_L] = synth->MIDI_NOTE_ID.at("D") + 12;
    keysMIDI[GLFW_KEY_P] = synth->MIDI_NOTE_ID.at("D#") + 12;

    synth = new audio::Synthesizer(&waveform, &adsr);
    synth->polyphony = _gui->getPolyphony();
    synth->setVolume(_gui->getVolume());
}
App::~App()
{
    delete _gui;
    delete synth;

    if (frequencyComplex)
        fftw_free(frequencyComplex);

    if (inputMIDI)
    {
        inputMIDI->closePort();
        delete inputMIDI;
    }
    if (threadMIDI)
    {
        threadMIDI->join();
        delete threadMIDI;
    }
}

void App::init()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(.1, .1, .1, 1);
}

void App::resize()
{
    _gui->prepareResize();
    _gui->resize(*windowWidth, *windowHeight);
}

void App::update()
{
    if (_gui->closePopup)
    {
        delete _gui->popupGUI;
        _gui->popupGUI = nullptr;
        _gui->closePopup = false;

        if (_gui->newPopupCallback)
        {
            _gui->newPopupCallback();
            if (_gui->closePopup)
            {
                _gui->closePopup = false;
                _gui->newPopupCallback();
            }
        }
    }

    if (_gui->volumeChange)
    {
        synth->setVolume(_gui->getVolume());
        _gui->volumeChange = false;
    }
    if (_gui->stereoPanChange)
    {
        synth->stereoPan = _gui->getStereoPan();
        _gui->stereoPanChange = false;
    }
    if (_gui->octaveChange)
    {
        synth->octave = _gui->getOctave();
        _gui->octaveChange = false;
        synth->releaseAll();

        _gui->keysPressedMIDI.clear();
    }
    if (_gui->polyphonyChange)
    {
        synth->polyphony = _gui->getPolyphony();
        _gui->polyphonyChange = false;
        synth->releaseAll();
    }

    if (!setupMIDI_GUI && threadMIDI_Complete)
    {
        setupMIDI_GUI = true;
        _gui->setupMIDI();

        threadMIDI->join();
        delete threadMIDI;
        threadMIDI = nullptr;
    }
    else if (_gui->MIDI_PortChange)
    {
        inputMIDI->closePort();
        inputMIDI->openPort(_gui->getMIDI_Port());
        _gui->MIDI_PortChange = false;
    }

    if (_gui->isOutputVisible())
    {
        if (timeDomain.size() != synth->bufferData.size())
            timeDomain.resize(synth->bufferData.size());

        for (size_t i = 0; i < synth->bufferData.size(); i++)
            timeDomain[i] = synth->bufferData[i] / (double)((pow(2, 16) / 2) - 1);

        _gui->updateTimeElement(timeDomain);

        if (frequencyDomain.size() != timeDomain.size() / 4)
        {
            frequencyComplex = (fftw_complex *)calloc(timeDomain.size() / 2 + 1, sizeof(fftw_complex));
            frequencyDomain.resize(timeDomain.size() / 4);
        }

        frequencyPlan = fftw_plan_dft_r2c_1d(timeDomain.size(), timeDomain.data(), frequencyComplex, FFTW_ESTIMATE);
        fftw_execute(frequencyPlan);

        for (size_t i = 0; i < frequencyDomain.size(); i++)
            frequencyDomain[i] = 20.0 * log10(sqrt(pow(frequencyComplex[i][0], 2) + pow(frequencyComplex[i][1], 2)));

        fftw_destroy_plan(frequencyPlan);

        _gui->updateFrequencyElement(frequencyDomain, frequencyDistribution, synth->SAMPLE_RATE);
    }
}

void App::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    _gui->draw();

    if (_gui->popupGUI)
        _gui->popupGUI->draw();
}

bool App::threadMIDI_Complete = false;

void App::createMIDI(RtMidiIn *&inputMIDI, std::vector<std::string> *ports)
{
    threadMIDI_Complete = false;
    inputMIDI = new RtMidiIn();

    unsigned int portCount = inputMIDI->getPortCount();
    if (portCount > 0)
    {
        for (unsigned int i = 0; i < portCount; i++)
            ports->push_back(inputMIDI->getPortName(i));
    }
    else
    {
        delete inputMIDI;
        inputMIDI = nullptr;
    }

    threadMIDI_Complete = true;
}