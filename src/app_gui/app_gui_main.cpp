#include "../../header/app/app_gui.hpp"

void AppGUI::mainGUI()
{
    sliderCallbacks["volume"] = [this](double &value)
    {
        textInputs["volume"]->setText(std::to_string(value).substr(0, 5), textInputs["volume"]->getFontSize(), textInputs["volume"]->getColor(), *windowWidth, *windowHeight);
        volumeChange = true;
    };
    inputCallbacks["volume"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
        {
            text = std::to_string(sliders["volume"]->getValue());
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.')
            {
                text = std::to_string(sliders["volume"]->getValue());
                text = text.substr(0, text.find('.') + 4);
                return;
            }

        double value = std::stod(text);
        if (value < 0)
        {
            value = 0;
            text = "0.000";
        }
        else if (value > 1)
        {
            value = 1;
            text = "1.000";
        }

        sliders["volume"]->setValue(value);
        volumeChange = true;
    };

    sliderCallbacks["stereoPan"] = [this](double &value)
    {
        textInputs["stereoPan"]->setText(std::to_string(value).substr(0, value < 0 ? 6 : 5), textInputs["stereoPan"]->getFontSize(), textInputs["stereoPan"]->getColor(), *windowWidth, *windowHeight);
        stereoPanChange = true;
    };
    inputCallbacks["stereoPan"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1 || std::count(text.begin(), text.end(), '-') > 1)
        {
            text = std::to_string(sliders["stereoPan"]->getValue());
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.' && c != '-')
            {
                text = std::to_string(sliders["stereoPan"]->getValue());
                text = text.substr(0, text.find('.') + 4);
                return;
            }

        double value = std::stod(text);
        if (value < -1)
        {
            value = -1;
            text = "-1.000";
        }
        else if (value > 1)
        {
            value = 1;
            text = "1.000";
        }

        sliders["stereoPan"]->setValue(value);
        stereoPanChange = true;
    };

    sliderCallbacks["polyphony"] = [this](double &value)
    {
        value = round(value);
        textInputs["polyphony"]->setText(std::to_string((short)value), textInputs["polyphony"]->getFontSize(), textInputs["polyphony"]->getColor(), *windowWidth, *windowHeight);
        polyphonyChange = true;
    };
    inputCallbacks["polyphony"] = [this](std::string &text)
    {
        if (text.size() == 0)
        {
            text = std::to_string((short)sliders["polyphony"]->getValue());
            return;
        }
        for (auto &c : text)
            if (c < '0' || c > '9')
            {
                text = std::to_string((short)sliders["polyphony"]->getValue());
                return;
            }

        short value = std::stoi(text);
        if (value < 1)
        {
            value = 1;
            text = "1";
        }
        else if (value > (short)(1 / waveform->amplitude))
        {
            value = 1 / waveform->amplitude;
            text = std::to_string(value);
        }

        sliders["polyphony"]->setValue(value);
        polyphonyChange = true;
    };

    clickCallbacks["outputDisplay"] = [this]()
    {
        getNamedChild("outputContainer")->draw_b = true;
        getNamedChild("harmonicsContainer")->draw_b = getNamedChild("adsrContainer")->draw_b = false;

        buttons["outputDisplay"]->setDefaultColor({0, .5, 1, 1});

        buttons["harmonicsDisplay"]->setDefaultColor({.2, .2, .2, 1});
        buttons["adsrDisplay"]->setDefaultColor({.2, .2, .2, 1});
    };

    clickCallbacks["harmonicsDisplay"] = [this]()
    {
        getNamedChild("harmonicsContainer")->draw_b = true;
        getNamedChild("outputContainer")->draw_b = getNamedChild("adsrContainer")->draw_b = false;

        buttons["harmonicsDisplay"]->setDefaultColor({0, .5, 1, 1});

        buttons["outputDisplay"]->setDefaultColor({.2, .2, .2, 1});
        buttons["adsrDisplay"]->setDefaultColor({.2, .2, .2, 1});
    };
    clickCallbacks["adsrDisplay"] = [this]()
    {
        getNamedChild("adsrContainer")->draw_b = true;

        getNamedChild("outputContainer")->draw_b = getNamedChild("harmonicsContainer")->draw_b = false;

        buttons["adsrDisplay"]->setDefaultColor({0, .5, 1, 1});

        buttons["outputDisplay"]->setDefaultColor({.2, .2, .2, 1});
        buttons["harmonicsDisplay"]->setDefaultColor({.2, .2, .2, 1});
    };

    clickCallbacks["synthPresets"] = [this]()
    {
        presetsType = SYNTH;
        presetsActionType = SELECT;
        clickCallbacks["presets"]();
    };
    clickCallbacks["harmonicsPresets"] = [this]()
    {
        presetsType = HARMONICS;
        presetsActionType = SELECT;
        clickCallbacks["presets"]();
    };
    clickCallbacks["adsrPresets"] = [this]()
    {
        presetsType = ADSR;
        presetsActionType = SELECT;
        clickCallbacks["presets"]();
    };
    createPresetsCallbacks();

    saveConstraints("mainContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .4, gui::RELATIVE_CONSTRAINT, 1));
    saveNamedChild("mainContainer", new gui::native::Container(glm::vec4(0), 0, getConstraints("mainContainer")), true);

    saveConstraints("waveformContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .4));
    saveNamedChild("waveformContainer", new gui::native::Container({0, 0, 0, 1}, 0, getConstraints("waveformContainer")));
    getNamedChild("mainContainer")->addChild(getNamedChild("waveformContainer"));

    saveConstraints("waveform", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::ASPECT_CONSTRAINT, .5));
    waveformElement = new customGUI::Waveform(1000, 500, getConstraints("waveform"));
    waveformElement->update(waveform);
    getNamedChild("waveformContainer")->addChild(waveformElement);

    saveConstraints("presetsContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .7, gui::RELATIVE_CONSTRAINT, .4, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .6));
    saveNamedChild("presetsContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("presetsContainer")));
    getNamedChild("mainContainer")->addChild(getNamedChild("presetsContainer"));

    saveConstraints("presetsLabel", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .95, gui::RELATIVE_CONSTRAINT, .12));
    getNamedChild("presetsContainer")->addChild(new gui::native::Text(font, "Presets: ", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("presetsLabel")));

    saveConstraints("synthPresets", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .8, gui::RELATIVE_CONSTRAINT, .95, gui::RELATIVE_CONSTRAINT, .09));
    buttons["synthPresets"] = new gui::native::Button({.2, .2, .2, 1}, 1, {0, .5, 1, 1}, clickCallbacks["synthPresets"], getConstraints("synthPresets"));
    buttons["synthPresets"]->setClickColor({0, .5, 1, 1});
    getNamedChild("presetsContainer")->addChild(buttons["synthPresets"]);

    saveConstraints("harmonicsPresets", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .7, gui::RELATIVE_CONSTRAINT, .95, gui::RELATIVE_CONSTRAINT, .09));
    buttons["harmonicsPresets"] = new gui::native::Button({.2, .2, .2, 1}, 1, {1, 0, 0, 1}, clickCallbacks["harmonicsPresets"], getConstraints("harmonicsPresets"));
    buttons["harmonicsPresets"]->setClickColor({1, 0, 0, 1});
    getNamedChild("presetsContainer")->addChild(buttons["harmonicsPresets"]);

    saveConstraints("adsrPresets", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .6, gui::RELATIVE_CONSTRAINT, .95, gui::RELATIVE_CONSTRAINT, .09));
    buttons["adsrPresets"] = new gui::native::Button({.2, .2, .2, 1}, 1, {1, .5, 0, 1}, clickCallbacks["adsrPresets"], getConstraints("adsrPresets"));
    buttons["adsrPresets"]->setClickColor({1, .5, 0, 1});
    getNamedChild("presetsContainer")->addChild(buttons["adsrPresets"]);

    buttons["synthPresets"]->addChild(new gui::native::Text(font, "Synth", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    buttons["harmonicsPresets"]->addChild(new gui::native::Text(font, "Harmonics", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    buttons["adsrPresets"]->addChild(new gui::native::Text(font, "ADSR", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));

    saveConstraints("mainSliders", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .4, gui::RELATIVE_CONSTRAINT, .7, gui::RELATIVE_CONSTRAINT, .2));
    saveNamedChild("mainSliders", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("mainSliders")));
    getNamedChild("mainContainer")->addChild(getNamedChild("mainSliders"));

    createInputWithSlider("volume", "Volume:", 0, 1, 1, "1.000",
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, 1 / 3.0),
                          getNamedChild("mainSliders"));
    sliders["volume"]->callbackOnMouseDown = true;

    createInputWithSlider("stereoPan", "Stereo Pan:", -1, 1, 0, "0.000",
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1 / 3.0, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, 1 / 3.0),
                          getNamedChild("mainSliders"));

    short polyphonyMax = 1 / waveform->amplitude;
    createInputWithSlider("polyphony", "Polyphony:", 1, polyphonyMax, polyphonyMax, std::to_string(polyphonyMax),
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 2 / 3.0, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, 1 / 3.0),
                          getNamedChild("mainSliders"));

    saveConstraints("contentDisplay", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .6, gui::RELATIVE_CONSTRAINT, .7, gui::RELATIVE_CONSTRAINT, .4));
    saveNamedChild("contentDisplay", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("contentDisplay")));
    getNamedChild("mainContainer")->addChild(getNamedChild("contentDisplay"));

    saveConstraints("outputDisplay", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .05, gui::RELATIVE_CONSTRAINT, .8, gui::RELATIVE_CONSTRAINT, .17));
    buttons["outputDisplay"] = new gui::native::Button({.2, .2, .2, 1}, 1, glm::vec4(1), clickCallbacks["outputDisplay"], getConstraints("outputDisplay"));
    buttons["outputDisplay"]->setClickColor({.4, .4, .4, 1});
    getNamedChild("contentDisplay")->addChild(buttons["outputDisplay"]);

    saveConstraints("harmonicsDisplay", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .25, gui::RELATIVE_CONSTRAINT, .8, gui::RELATIVE_CONSTRAINT, .17));
    buttons["harmonicsDisplay"] = new gui::native::Button({.2, .2, .2, 1}, 1, glm::vec4(1), clickCallbacks["harmonicsDisplay"], getConstraints("harmonicsDisplay"));
    buttons["harmonicsDisplay"]->setClickColor({.4, .4, .4, 1});
    getNamedChild("contentDisplay")->addChild(buttons["harmonicsDisplay"]);

    saveConstraints("adsrDisplay", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .45, gui::RELATIVE_CONSTRAINT, .8, gui::RELATIVE_CONSTRAINT, .17));
    buttons["adsrDisplay"] = new gui::native::Button({.2, .2, .2, 1}, 1, glm::vec4(1), clickCallbacks["adsrDisplay"], getConstraints("adsrDisplay"));
    buttons["adsrDisplay"]->setClickColor({.4, .4, .4, 1});
    getNamedChild("contentDisplay")->addChild(buttons["adsrDisplay"]);

    buttons["outputDisplay"]->addChild(new gui::native::Text(font, "Audio Output", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    buttons["harmonicsDisplay"]->addChild(new gui::native::Text(font, "Waveform Harmonics", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
    buttons["adsrDisplay"]->addChild(new gui::native::Text(font, "ADSR Envelope", 45, glm::vec3(1), false, gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));
}