#include "../../header/app/app_gui.hpp"

void AppGUI::harmonicsGUI()
{
    inputCallbacks["updateWaveform"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
            text = "0";

        updateWaveform();
    };

    int defaultFontSize = 30;

    saveNamedChild("harmonicsContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("defaultContainer")), true);

    saveNamedChild("harmonicsTitle", new gui::native::Container({.2, .2, .2, 1}, 1, glm::vec4(1), getConstraints("defaultTitle")));
    getNamedChild("harmonicsContainer")->addChild(getNamedChild("harmonicsTitle"));
    getNamedChild("harmonicsTitle")->addChild(new gui::native::Text(font, "Waveform Harmonics", 45, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));

    saveConstraints("harmonicLabel", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .025, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .2 - .05, gui::RELATIVE_CONSTRAINT, 1));
    saveConstraints("harmonicAmplitude", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .2 + .025, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .45 - .05, gui::RELATIVE_CONSTRAINT, 1));
    saveConstraints("harmonicPhase", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .65 + .025, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .35 - .05, gui::RELATIVE_CONSTRAINT, 1));

    saveConstraints("harmonicsInfo_1", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .05, gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, .05));
    saveNamedChild("harmonicsInfo_1", new gui::native::Container(glm::vec4(0), 0, getConstraints("harmonicsInfo_1")));
    getNamedChild("harmonicsContainer")->addChild(getNamedChild("harmonicsInfo_1"));

    getNamedChild("harmonicsInfo_1")->addChild(new gui::native::Text(font, "h", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicLabel")));
    getNamedChild("harmonicsInfo_1")->addChild(new gui::native::Text(font, "Amplitude", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicAmplitude")));
    getNamedChild("harmonicsInfo_1")->addChild(new gui::native::Text(font, "Phase", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicPhase")));

    saveConstraints("harmonicsInfo_2", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, .05, gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, .05));
    saveNamedChild("harmonicsInfo_2", new gui::native::Container(glm::vec4(0), 0, getConstraints("harmonicsInfo_2")));
    getNamedChild("harmonicsContainer")->addChild(getNamedChild("harmonicsInfo_2"));

    getNamedChild("harmonicsInfo_2")->addChild(new gui::native::Text(font, "h", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicLabel")));
    getNamedChild("harmonicsInfo_2")->addChild(new gui::native::Text(font, "Amplitude", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicAmplitude")));
    getNamedChild("harmonicsInfo_2")->addChild(new gui::native::Text(font, "Phase", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicPhase")));

    saveConstraints("harmonicsInfo_3", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 2 / 3.0, gui::RELATIVE_CONSTRAINT, .05, gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, .05));
    saveNamedChild("harmonicsInfo_3", new gui::native::Container(glm::vec4(0), 0, getConstraints("harmonicsInfo_3")));
    getNamedChild("harmonicsContainer")->addChild(getNamedChild("harmonicsInfo_3"));

    getNamedChild("harmonicsInfo_3")->addChild(new gui::native::Text(font, "h", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicLabel")));
    getNamedChild("harmonicsInfo_3")->addChild(new gui::native::Text(font, "Amplitude", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicAmplitude")));
    getNamedChild("harmonicsInfo_3")->addChild(new gui::native::Text(font, "Phase", defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicPhase")));

    saveConstraints("harmonics", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .1, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .9));
    saveNamedChild("harmonics", new gui::native::Container(glm::vec4(0), 0, getConstraints("harmonics")));
    getNamedChild("harmonicsContainer")->addChild(getNamedChild("harmonics"));

    for (unsigned int h = 1; h <= NUM_HARMONICS; h++)
    {
        std::string str = "harmonic_" + std::to_string(h);
        saveConstraints(str, new gui::Constraints(gui::RELATIVE_CONSTRAINT, (h > NUM_HARMONICS / 3) ? (h > NUM_HARMONICS * (2 / 3.0) ? 2 / 3.0 : 1 / 3.0) : 0, gui::RELATIVE_CONSTRAINT, ((h > NUM_HARMONICS / 3) ? (h > NUM_HARMONICS * (2 / 3.0) ? h - floor(NUM_HARMONICS * (2 / 3.0)) - 1 : h - (NUM_HARMONICS / 3) - 1) : h - 1) / (double)(NUM_HARMONICS / 3), gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, 1 / (double)(NUM_HARMONICS / 3)));
        saveNamedChild(str, new gui::native::Container(glm::vec4(0), 0, getConstraints(str)));
        getNamedChild("harmonics")->addChild(getNamedChild(str));

        getNamedChild(str)->addChild(new gui::native::Text(font, std::to_string(h), defaultFontSize, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("harmonicLabel")));

        saveNamedChild(str + "_amplitudeContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("harmonicAmplitude")));
        getNamedChild(str)->addChild(getNamedChild(str + "_amplitudeContainer"));
        textInputs[str + "_amplitude"] = new gui::native::TextInput(font, h == 1 ? "1" : "0", defaultFontSize, glm::vec3(1), inputCallbacks["updateWaveform"], inputCallbacks["updateWaveform"], getConstraints("btnText"));
        getNamedChild(str + "_amplitudeContainer")->addChild(textInputs[str + "_amplitude"]);

        saveNamedChild(str + "_phaseContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("harmonicPhase")));
        getNamedChild(str)->addChild(getNamedChild(str + "_phaseContainer"));
        textInputs[str + "_phase"] = new gui::native::TextInput(font, "0", defaultFontSize, glm::vec3(1), inputCallbacks["updateWaveform"], inputCallbacks["updateWaveform"], getConstraints("btnText"));
        getNamedChild(str + "_phaseContainer")->addChild(textInputs[str + "_phase"]);
    }
}