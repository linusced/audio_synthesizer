#include "../../header/app/app_gui.hpp"

const unsigned short AppGUI::NUM_HARMONICS = 51;

AppGUI::AppGUI(audio::Waveform *waveform, audio::ADSR *adsr, const std::vector<std::vector<int>> &frequencyDistribution, GLFWwindow *window, int *windowWidth, int *windowHeight)
{
    this->waveform = waveform;
    this->adsr = adsr;

    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    this->window = window;

    font = new gui::Font(filePath("fonts/Arial.ttf"));

    saveConstraints("btnText", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .85, gui::RELATIVE_CONSTRAINT, .9));
    saveConstraints("fill", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, 1));

    saveConstraints("defaultContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .4, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .6, gui::RELATIVE_CONSTRAINT, 1));
    saveConstraints("defaultTitle", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::PIXEL_CONSTRAINT, -2, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .05));
    saveConstraints("defaultContentContainer", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .05, gui::RELATIVE_CONSTRAINT, .99, gui::RELATIVE_CONSTRAINT, .95));

    outputGUI(frequencyDistribution);
    harmonicsGUI();
    ADSR_GUI();
    mainGUI();

    clickCallbacks["outputDisplay"]();
    resize(*windowWidth, *windowHeight);

    loadPreset(applicationSupportPath("presets/autosave.dat"));
}
AppGUI::~AppGUI()
{
    savePreset(applicationSupportPath("presets/autosave.dat"), SYNTH);
}

void AppGUI::createInputWithSlider(std::string name, std::string labelText, double minValue, double maxValue, double defaultValue, std::string defaultText, gui::Constraints *container_con, gui::Element *parent)
{
    saveConstraints(name + "Container", container_con);
    saveConstraints(name + "Label", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .02, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .36, gui::RELATIVE_CONSTRAINT, 1));
    saveConstraints(name + "InputContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .39, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .09, gui::RELATIVE_CONSTRAINT, .6));
    saveConstraints(name + "Slider", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .52, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .46, gui::RELATIVE_CONSTRAINT, .6));

    saveNamedChild(name + "Container", new gui::native::Container(glm::vec4(0), 0, getConstraints(name + "Container")));
    parent->addChild(getNamedChild(name + "Container"));

    getNamedChild(name + "Container")->addChild(new gui::native::Text(font, labelText, 45, glm::vec3(1), false, gui::ALIGNMENT_RIGHT_H, gui::ALIGNMENT_CENTER_V, getConstraints(name + "Label")));
    saveNamedChild(name + "InputContainer", new gui::native::Container(glm::vec4(1), 0, getConstraints(name + "InputContainer")));
    getNamedChild(name + "Container")->addChild(getNamedChild(name + "InputContainer"));

    textInputs[name] = new gui::native::TextInput(font, defaultText, 20, glm::vec3(0), inputCallbacks[name], inputCallbacks[name], getConstraints("fill"));
    getNamedChild(name + "InputContainer")->addChild(textInputs[name]);

    sliders[name] = new customGUI::Slider(minValue, maxValue, defaultValue, sliderCallbacks[name], getConstraints(name + "Slider"));
    getNamedChild(name + "Container")->addChild(sliders[name]);
}

void AppGUI::updateWaveform()
{
    int activeHarmonics = 1;
    for (int h = NUM_HARMONICS; h >= 1; h--)
        if (textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->getText() != "0")
        {
            activeHarmonics = h;
            break;
        }

    if (waveform->harmonics.size() != activeHarmonics)
        waveform->harmonics.resize(activeHarmonics);

    for (int h = 1; h <= activeHarmonics; h++)
    {
        waveform->harmonics[h - 1].amplitude = evaluateUserInput(textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->getText(), h);
        waveform->harmonics[h - 1].phase = evaluateUserInput(textInputs["harmonic_" + std::to_string(h) + "_phase"]->getText(), h);
    }

    waveformElement->update(waveform);
}

double AppGUI::evaluateUserInput(std::string input, double h)
{
    if (input.size() == 0)
        return 0;

    te_variable vars[] = {{"h", &h}};
    te_expr *expr = te_compile(input.c_str(), vars, 1, 0);

    if (!expr)
    {
        std::cout << "Invalid mathematical expression (h_" << (int)h << ")\n";
        return 0;
    }

    double result = te_eval(expr);
    te_free(expr);
    return result;
}

double AppGUI::getVolume()
{
    return sliders["volume"]->getValue();
}
double AppGUI::getStereoPan()
{
    return sliders["stereoPan"]->getValue();
}
short AppGUI::getOctave()
{
    return sliders["octave"]->getValue();
}
short AppGUI::getPolyphony()
{
    return sliders["polyphony"]->getValue();
}
bool AppGUI::getFrequencyLinearMode()
{
    return frequencyLinearMode;
}
int AppGUI::getMIDI_Port()
{
    return activePortMIDI;
}

void AppGUI::setVolume(double volume)
{
    sliders["volume"]->setValue(volume);
    volume = sliders["volume"]->getValue();
    textInputs["volume"]->setText(std::to_string(volume).substr(0, 5), textInputs["volume"]->getFontSize(), textInputs["volume"]->getColor(), *windowWidth, *windowHeight);
}
void AppGUI::setStereoPan(double stereoPan)
{
    sliders["stereoPan"]->setValue(stereoPan);
    stereoPan = sliders["stereoPan"]->getValue();
    textInputs["stereoPan"]->setText(std::to_string(stereoPan).substr(0, stereoPan < 0 ? 6 : 5), textInputs["stereoPan"]->getFontSize(), textInputs["stereoPan"]->getColor(), *windowWidth, *windowHeight);
}
void AppGUI::setOctave(short octave)
{
    sliders["octave"]->setValue(octave);
    octave = sliders["octave"]->getValue();
    textInputs["octave"]->setText(std::to_string(octave), textInputs["octave"]->getFontSize(), textInputs["octave"]->getColor(), *windowWidth, *windowHeight);
}
void AppGUI::setPolyphony(short polyphony)
{
    sliders["polyphony"]->setValue(polyphony);
    polyphony = sliders["polyphony"]->getValue();
    textInputs["polyphony"]->setText(std::to_string(polyphony), textInputs["polyphony"]->getFontSize(), textInputs["polyphony"]->getColor(), *windowWidth, *windowHeight);
}
void AppGUI::toggleFrequencyLinearMode()
{
    clickCallbacks["frequencyLinearMode"]();
}
void AppGUI::setMIDI_Port(int port)
{
    activePortMIDI = port;
    variableText["midiPortsActive"]->setText(portsMIDI[port], variableText["midiPortsActive"]->getFontSize(), variableText["midiPortsActive"]->getColor(), *windowWidth, *windowHeight);
}

bool AppGUI::isOutputVisible()
{
    return getNamedChild("outputContainer")->draw_b;
}
void AppGUI::updateTimeElement(const std::vector<double> &timeDomain)
{
    timeElement->update(timeDomain);
}
void AppGUI::updateFrequencyElement(const std::vector<double> &frequencyDomain, const std::vector<std::vector<int>> &frequencyDistribution, unsigned int sampleRate)
{
    frequencyElement->update(frequencyDomain, frequencyDistribution, sampleRate, frequencyLinearMode);
}
void AppGUI::setupMIDI()
{
    variableText["midiPortsCount"]->setText(std::to_string(portsMIDI.size()) + " MIDI-ports found!", variableText["midiPortsCount"]->getFontSize(), variableText["midiPortsCount"]->getColor(), *windowWidth, *windowHeight);
    if (portsMIDI.size() > 0)
    {
        buttons["midiPortsSelect"]->draw_b = true;
        setMIDI_Port(0);
        MIDI_PortChange = true;
    }
}

void AppGUI::prepareResize()
{
    if (*windowHeight / (double)*windowWidth < 0.507)
    {
        getConstraints("waveform")->setConstraintW(gui::ASPECT_CONSTRAINT);
        getConstraints("waveform")->setConstraintH(gui::RELATIVE_CONSTRAINT);
        getConstraints("waveform")->setW(2);
        getConstraints("waveform")->setH(1);
    }
    else
    {
        getConstraints("waveform")->setConstraintW(gui::RELATIVE_CONSTRAINT);
        getConstraints("waveform")->setConstraintH(gui::ASPECT_CONSTRAINT);
        getConstraints("waveform")->setW(1);
        getConstraints("waveform")->setH(.5);
    }
}