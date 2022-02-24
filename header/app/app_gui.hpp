#ifndef APP_GUI
#define APP_GUI

#include "../gl/gui.hpp"
#include "../custom_gui/custom_gui.hpp"
#include "../files.hpp"
#include "../audio/audio.hpp"

#include <tinyexpr/tinyexpr.h>

class AppGUI : public gui::Master
{
private:
    static const unsigned short NUM_HARMONICS;

    GLFWwindow *window = nullptr;
    int *windowWidth = nullptr, *windowHeight = nullptr;

    audio::Waveform *waveform = nullptr;
    audio::ADSR *adsr = nullptr;

    gui::Font *font = nullptr;

    customGUI::Waveform *waveformElement = nullptr;
    customGUI::ADSR *adsrElement = nullptr;
    customGUI::WaveformTime *timeElement = nullptr;
    customGUI::WaveformFrequency *frequencyElement = nullptr;
    customGUI::KeyboardMIDI *keyboardMIDI_Element = nullptr;
    customGUI::KeyboardComputer *keyboardComputerElement = nullptr;

    bool frequencyLinearMode = false;

    std::map<std::string, gui::native::Button *> buttons;
    std::map<std::string, customGUI::Slider *> sliders;
    std::map<std::string, gui::native::TextInput *> textInputs;

    std::map<std::string, std::function<void()>> clickCallbacks;
    std::map<std::string, std::function<void(double &)>> sliderCallbacks;
    std::map<std::string, std::function<void(std::string &)>> inputCallbacks;
    std::map<std::string, gui::native::Text *> variableText;

    void createInputWithSlider(std::string name, std::string labelText, double minValue, double maxValue, double defaultValue, std::string defaultText, gui::Constraints *container_con, gui::Element *parent);

    void mainGUI(), outputGUI(const std::vector<std::vector<int>> &frequencyDistribution), harmonicsGUI(), ADSR_GUI();

    void updateWaveform();
    double evaluateUserInput(std::string input, double h);

    int activePortMIDI = -1;
    std::function<void(int)> selectPortMIDI_Callback = nullptr;

    enum PRESET_TYPE
    {
        SYNTH,
        HARMONICS,
        ADSR
    };
    enum PRESET_ACTION_TYPE
    {
        SELECT,
        LOAD,
        SAVE,
        DELETE,
        DELETE_REFRESH
    };

    PRESET_TYPE presetsType = SYNTH;
    PRESET_ACTION_TYPE presetsActionType = SELECT;
    std::vector<std::string> presetsFiles;
    std::string savePresetName;

    std::function<void(int)> presetsPopupCallback = nullptr, savePresetActionCallback = nullptr;
    std::function<void(std::string &)> savePresetTextInputCallback = nullptr;
    void createPresetsCallbacks();

    void openPopup(std::string title, std::string closeBtnText, std::vector<std::string> items, std::function<void(int)> *callback);
    void openTextInputPopup(std::string title, std::string defaultText, std::string cancelBtnText, std::string continueBtnText, std::function<void(std::string &)> *textInputCallback, std::function<void(int)> *actionCallback);

    void loadPreset(std::string absolutePresetPath);
    void savePreset(std::string absolutePresetPath, PRESET_TYPE type);

public:
    AppGUI(audio::Waveform *waveform, audio::ADSR *adsr, const std::vector<std::vector<int>> &frequencyDistribution, GLFWwindow *window, int *windowWidth, int *windowHeight);
    ~AppGUI();

    void prepareResize();

    bool volumeChange = false, stereoPanChange = false, octaveChange = false, polyphonyChange = false, MIDI_PortChange = false;
    double getVolume(), getStereoPan();
    short getOctave(), getPolyphony();
    bool getFrequencyLinearMode();
    int getMIDI_Port();

    void setVolume(double volume), setStereoPan(double stereoPan);
    void setOctave(short octave), setPolyphony(short polyphony);
    void toggleFrequencyLinearMode();
    void setMIDI_Port(int port);

    bool isOutputVisible();
    void updateTimeElement(const std::vector<double> &timeDomain);
    void updateFrequencyElement(const std::vector<double> &frequencyDomain, const std::vector<std::vector<int>> &frequencyDistribution, unsigned int sampleRate);
    void setupMIDI();

    std::vector<short> keysPressedMIDI;
    std::vector<std::string> portsMIDI;

    gui::Master *popupGUI = nullptr;
    bool closePopup = false;
    std::function<void()> newPopupCallback = nullptr;
};

#endif