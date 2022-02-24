#include "../../header/app/app_gui.hpp"

void AppGUI::outputGUI(const std::vector<std::vector<int>> &frequencyDistribution)
{
    clickCallbacks["frequencyLinearMode"] = [&]()
    {
        frequencyLinearMode = !frequencyLinearMode;
        buttons["frequencyLinearMode"]->setDefaultColor(frequencyLinearMode ? glm::vec4(1, .5, 0, 1) : glm::vec4(.2, .2, .2, 1));

        double prevX = -1, x, w = .06;
        int freq;
        for (auto &i : frequencyDistribution)
        {
            freq = i.at(0);
            getConstraints("frequencyLine" + std::to_string(freq))->setW(0);
            getConstraints("frequencyLabel" + std::to_string(freq))->setW(0);

            if (frequencyLinearMode)
            {
                if (freq == 20)
                {
                    getConstraints("frequencyLine" + std::to_string(freq))->setX(0);
                    getConstraints("frequencyLabel" + std::to_string(freq))->setX(0);

                    getConstraints("frequencyLine" + std::to_string(freq))->setW(.003);
                    getConstraints("frequencyLabel" + std::to_string(freq))->setW(w);
                }
                else if (freq == 24000)
                {
                    getConstraints("frequencyLine" + std::to_string(freq))->setX(1 - .003);
                    getConstraints("frequencyLabel" + std::to_string(freq))->setX(1 - w);

                    getConstraints("frequencyLine" + std::to_string(freq))->setW(.003);
                    getConstraints("frequencyLabel" + std::to_string(freq))->setW(w);
                }
            }
            else
            {
                x = (i.at(1) / 1000.0) - (w / 2.0);
                if (x - prevX > w)
                {
                    prevX = x;

                    getConstraints("frequencyLine" + std::to_string(freq))->setX(x + (w / 2.0));
                    getConstraints("frequencyLabel" + std::to_string(freq))->setX(x);

                    getConstraints("frequencyLine" + std::to_string(freq))->setW(.003);
                    getConstraints("frequencyLabel" + std::to_string(freq))->setW(w);
                }
            }
        }
        resize(*windowWidth, *windowHeight);
    };

    sliderCallbacks["octave"] = [this](double &value)
    {
        value = round(value);
        textInputs["octave"]->setText(std::to_string((short)value), textInputs["octave"]->getFontSize(), textInputs["octave"]->getColor(), *windowWidth, *windowHeight);
        octaveChange = true;
    };
    inputCallbacks["octave"] = [this](std::string &text)
    {
        if (text.size() == 0)
        {
            text = std::to_string((short)sliders["octave"]->getValue());
            return;
        }
        for (auto &c : text)
            if (c < '0' || c > '9')
            {
                text = std::to_string((short)sliders["octave"]->getValue());
                return;
            }

        short value = std::stoi(text);
        if (value < 0)
        {
            value = 0;
            text = "0";
        }
        else if (value > 7)
        {
            value = 7;
            text = "7";
        }

        sliders["octave"]->setValue(value);
        octaveChange = true;
    };

    selectPortMIDI_Callback = [this](int index)
    {
        if (index != -1)
        {
            setMIDI_Port(index);
            MIDI_PortChange = true;
        }
        closePopup = true;
    };
    clickCallbacks["midiPortsSelect"] = [this]()
    {
        openPopup("Select MIDI port:", "Cancel", portsMIDI, &selectPortMIDI_Callback);
    };

    saveNamedChild("outputContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("defaultContainer")), true);

    saveNamedChild("outputTitle", new gui::native::Container({.2, .2, .2, 1}, 1, glm::vec4(1), getConstraints("defaultTitle")));
    getNamedChild("outputContainer")->addChild(getNamedChild("outputTitle"));
    getNamedChild("outputTitle")->addChild(new gui::native::Text(font, "Audio Output", 45, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));

    saveNamedChild("outputContent", new gui::native::Container(glm::vec4(0), 0, getConstraints("defaultContentContainer")));
    getNamedChild("outputContainer")->addChild(getNamedChild("outputContent"));

    saveConstraints("time", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .3));
    timeElement = new customGUI::WaveformTime(1000, 500, getConstraints("time"));
    getNamedChild("outputContent")->addChild(timeElement);

    saveConstraints("frequency", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .3));
    frequencyElement = new customGUI::WaveformFrequency(1000, 500, getConstraints("frequency"));
    getNamedChild("outputContent")->addChild(frequencyElement);

    int freq;
    for (auto &i : frequencyDistribution)
    {
        freq = i.at(0);
        saveConstraints("frequencyLine" + std::to_string(freq), new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .1, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .9));
        frequencyElement->addChild(new gui::native::Container({1, .75, .5, .5}, 0, getConstraints("frequencyLine" + std::to_string(freq))));

        saveConstraints("frequencyLabel" + std::to_string(freq), new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .025, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .1));
        frequencyElement->addChild(new gui::native::Text(font, std::to_string(freq > 999 ? freq / 1000 : freq) + (freq > 999 ? " kHz" : " Hz"), 20, {1, .5, 0}, false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_TOP_V, getConstraints("frequencyLabel" + std::to_string(freq))));
    }

    saveConstraints("outputControls", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .4));
    saveNamedChild("outputControls", new gui::native::Container(glm::vec4(0), 0, getConstraints("outputControls")));
    getNamedChild("outputContent")->addChild(getNamedChild("outputControls"));

    saveConstraints("frequencyLinearMode", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .01, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .2));
    buttons["frequencyLinearMode"] = new gui::native::Button({.2, .2, .2, 1}, 1, glm::vec4(1), clickCallbacks["frequencyLinearMode"], getConstraints("frequencyLinearMode"));
    buttons["frequencyLinearMode"]->setClickColor({.4, .4, .4, 1});
    buttons["frequencyLinearMode"]->addChild(new gui::native::Text(font, "Linear Frequency Display", 40, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    getNamedChild("outputControls")->addChild(buttons["frequencyLinearMode"]);
    frequencyLinearMode = true;
    clickCallbacks["frequencyLinearMode"]();

    saveConstraints("keyboardMIDI", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .01, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .3));
    keyboardMIDI_Element = new customGUI::KeyboardMIDI(1000, 200, &keysPressedMIDI, getConstraints("keyboardMIDI"));
    getNamedChild("outputControls")->addChild(keyboardMIDI_Element);

    saveConstraints("keyboardComputer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .32, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .25));
    keyboardComputerElement = new customGUI::KeyboardComputer(400, 200, window, font, getConstraints("keyboardComputer"));
    getNamedChild("outputControls")->addChild(keyboardComputerElement);

    saveConstraints("keyboardComputerLabel", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .22, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .24));
    getNamedChild("outputControls")->addChild(new gui::native::Text(font, "Keyboard MIDI Control", 35, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("keyboardComputerLabel")));

    createInputWithSlider("octave", "Octave:", 0, 7, 2, "2",
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT_INVERSE, .32 + .15, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .15),
                          getNamedChild("outputControls"));

    saveConstraints("octaveControlLabel", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .44, gui::RELATIVE_CONSTRAINT_INVERSE, .43, gui::RELATIVE_CONSTRAINT, .16, gui::RELATIVE_CONSTRAINT, .06));
    getNamedChild("outputControls")->addChild(new gui::native::Text(font, "Decrement | Increment", 35, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("octaveControlLabel")));

    saveConstraints("octaveDecrementKey", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .46, gui::RELATIVE_CONSTRAINT_INVERSE, .33, gui::ASPECT_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .1));
    saveNamedChild("octaveDecrementKey", new gui::native::Container(glm::vec4(1), 2, {0, 0, 0, 1}, getConstraints("octaveDecrementKey")));
    getNamedChild("outputControls")->addChild(getNamedChild("octaveDecrementKey"));

    saveConstraints("octaveIncrementKey", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .55, gui::RELATIVE_CONSTRAINT_INVERSE, .33, gui::ASPECT_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .1));
    saveNamedChild("octaveIncrementKey", new gui::native::Container(glm::vec4(1), 2, {0, 0, 0, 1}, getConstraints("octaveIncrementKey")));
    getNamedChild("outputControls")->addChild(getNamedChild("octaveIncrementKey"));

    getNamedChild("octaveDecrementKey")->addChild(new gui::native::Text(font, ",", 40, glm::vec3(0), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    getNamedChild("octaveIncrementKey")->addChild(new gui::native::Text(font, ".", 40, glm::vec3(0), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));

    saveConstraints("midiPortsContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT, .01, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .6));
    saveNamedChild("midiPortsContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("midiPortsContainer")));
    getNamedChild("outputControls")->addChild(getNamedChild("midiPortsContainer"));

    saveConstraints("midiPortsCount", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, 1 / 3.0));
    variableText["midiPortsCount"] = new gui::native::Text(font, "Searching for MIDI-ports...", 40, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("midiPortsCount"));
    getNamedChild("midiPortsContainer")->addChild(variableText["midiPortsCount"]);

    saveConstraints("midiPortsSelect", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .95, gui::RELATIVE_CONSTRAINT, 1 / 3.0));
    buttons["midiPortsSelect"] = new gui::native::Button(glm::vec4(1), 0, clickCallbacks["midiPortsSelect"], getConstraints("midiPortsSelect"));
    buttons["midiPortsSelect"]->setClickColor({.8, .8, .8, 1});
    buttons["midiPortsSelect"]->addChild(new gui::native::Text(font, "Select MIDI-port", 40, glm::vec3(0), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    buttons["midiPortsSelect"]->draw_b = false;
    getNamedChild("midiPortsContainer")->addChild(buttons["midiPortsSelect"]);

    saveConstraints("midiPortsActiveLabel", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT, .5, gui::RELATIVE_CONSTRAINT, 1 / 3.0));
    getNamedChild("midiPortsContainer")->addChild(new gui::native::Text(font, "Active port: ", 30, glm::vec3(1), false, gui::ALIGNMENT_RIGHT_H, gui::ALIGNMENT_CENTER_V, getConstraints("midiPortsActiveLabel")));

    saveConstraints("midiPortsActive", new gui::Constraints(gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT, .5, gui::RELATIVE_CONSTRAINT, 1 / 3.0));
    variableText["midiPortsActive"] = new gui::native::Text(font, "N/A", 30, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("midiPortsActive"));
    getNamedChild("midiPortsContainer")->addChild(variableText["midiPortsActive"]);
}