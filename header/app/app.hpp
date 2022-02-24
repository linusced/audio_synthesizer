#ifndef APP
#define APP

#include "../audio/audio.hpp"
#include "../gl/draw_loop.hpp"
#include "app_gui.hpp"
#include <fftw3.h>
#include <rtmidi/RtMidi.h>

class App : public DrawLoop
{
private:
    audio::Waveform waveform;
    audio::ADSR adsr;
    audio::Synthesizer *synth = nullptr;
    AppGUI *_gui = nullptr;

    std::map<GLenum, short> keysMIDI;
    std::map<GLenum, bool> keyDown;

    std::vector<double> timeDomain, frequencyDomain;
    static const std::vector<std::vector<int>> frequencyDistribution;
    fftw_complex *frequencyComplex = nullptr;
    fftw_plan frequencyPlan;

    RtMidiIn *inputMIDI = nullptr;
    std::vector<uint8_t> inputDataMIDI;
    std::thread *threadMIDI = nullptr;
    bool setupMIDI_GUI = false;

    static bool threadMIDI_Complete;
    static void createMIDI(RtMidiIn *&inputMIDI, std::vector<std::string> *ports);

    void init() override;
    void resize() override;
    void update() override;
    void inputs() override;
    void draw() override;

public:
    App(GLFWwindow *window, int *windowWidth, int *windowHeight);
    ~App();
};

#endif