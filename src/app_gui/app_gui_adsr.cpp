#include "../../header/app/app_gui.hpp"

void AppGUI::ADSR_GUI()
{
    sliderCallbacks["attack_time"] = [this](double &value)
    {
        std::string text = std::to_string(value);
        text = text.substr(0, text.find('.') + 4);
        textInputs["attack_time"]->setText(text, textInputs["attack_time"]->getFontSize(), textInputs["attack_time"]->getColor(), *windowWidth, *windowHeight);
        adsr->attack_time = value;

        adsrElement->update(adsr);
    };
    inputCallbacks["attack_time"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
        {
            text = std::to_string(adsr->attack_time);
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.')
            {
                text = std::to_string(adsr->attack_time);
                text = text.substr(0, text.find('.') + 4);
                return;
            }

        double value = std::stod(text);
        if (value < 0)
        {
            value = 0;
            text = "0.000";
        }
        else if (value > 2)
        {
            value = 2;
            text = "2.000";
        }

        sliders["attack_time"]->setValue(value);
        adsr->attack_time = value;

        adsrElement->update(adsr);
    };

    sliderCallbacks["decay_time"] = [this](double &value)
    {
        std::string text = std::to_string(value);
        text = text.substr(0, text.find('.') + 4);
        textInputs["decay_time"]->setText(text, textInputs["decay_time"]->getFontSize(), textInputs["decay_time"]->getColor(), *windowWidth, *windowHeight);
        adsr->decay_time = value;

        adsrElement->update(adsr);
    };
    inputCallbacks["decay_time"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
        {
            text = std::to_string(adsr->decay_time);
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.')
            {
                text = std::to_string(adsr->decay_time);
                text = text.substr(0, text.find('.') + 4);
                return;
            }

        double value = std::stod(text);
        if (value < 0)
        {
            value = 0;
            text = "0.000";
        }
        else if (value > 2)
        {
            value = 2;
            text = "2.000";
        }

        sliders["decay_time"]->setValue(value);
        adsr->decay_time = value;

        adsrElement->update(adsr);
    };

    sliderCallbacks["sustain_value"] = [this](double &value)
    {
        std::string text = std::to_string(value);
        text = text.substr(0, text.find('.') + 4);
        textInputs["sustain_value"]->setText(text, textInputs["sustain_value"]->getFontSize(), textInputs["sustain_value"]->getColor(), *windowWidth, *windowHeight);
        adsr->sustain_value = value;

        adsrElement->update(adsr);
    };
    inputCallbacks["sustain_value"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
        {
            text = std::to_string(adsr->sustain_value);
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.')
            {
                text = std::to_string(adsr->sustain_value);
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

        sliders["sustain_value"]->setValue(value);
        adsr->sustain_value = value;

        adsrElement->update(adsr);
    };

    sliderCallbacks["release_time"] = [this](double &value)
    {
        std::string text = std::to_string(value);
        text = text.substr(0, text.find('.') + 4);
        textInputs["release_time"]->setText(text, textInputs["release_time"]->getFontSize(), textInputs["release_time"]->getColor(), *windowWidth, *windowHeight);
        adsr->release_time = value;

        adsrElement->update(adsr);
    };
    inputCallbacks["release_time"] = [this](std::string &text)
    {
        if (text.size() == 0 || text == "." || std::count(text.begin(), text.end(), '.') > 1)
        {
            text = std::to_string(adsr->release_time);
            text = text.substr(0, text.find('.') + 4);
            return;
        }
        for (auto &c : text)
            if ((c < '0' || c > '9') && c != '.')
            {
                text = std::to_string(adsr->release_time);
                text = text.substr(0, text.find('.') + 4);
                return;
            }

        double value = std::stod(text);
        if (value < 0)
        {
            value = 0;
            text = "0.000";
        }
        else if (value > 2)
        {
            value = 2;
            text = "2.000";
        }

        sliders["release_time"]->setValue(value);
        adsr->release_time = value;

        adsrElement->update(adsr);
    };

    saveNamedChild("adsrContainer", new gui::native::Container(glm::vec4(0), 1, glm::vec4(1), getConstraints("defaultContainer")), true);

    saveNamedChild("adsrTitle", new gui::native::Container({.2, .2, .2, 1}, 1, glm::vec4(1), getConstraints("defaultTitle")));
    getNamedChild("adsrContainer")->addChild(getNamedChild("adsrTitle"));
    getNamedChild("adsrTitle")->addChild(new gui::native::Text(font, "Attack - Decay - Sustain - Release", 45, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, getConstraints("btnText")));

    saveNamedChild("adsrContent", new gui::native::Container(glm::vec4(0), 0, getConstraints("defaultContentContainer")));
    getNamedChild("adsrContainer")->addChild(getNamedChild("adsrContent"));

    saveConstraints("adsr", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .4));
    adsrElement = new customGUI::ADSR(1000, 500, getConstraints("adsr"));
    adsrElement->update(adsr);
    getNamedChild("adsrContent")->addChild(adsrElement);

    createInputWithSlider("attack_time", "Attack Time:", 0, 2, adsr->attack_time, std::to_string(adsr->attack_time).substr(0, 5),
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, getConstraints("adsr")->getH() + .02, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, .075),
                          getNamedChild("adsrContent"));

    createInputWithSlider("decay_time", "Decay Time:", 0, 2, adsr->decay_time, std::to_string(adsr->decay_time).substr(0, 5),
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, getConstraints("attack_timeContainer")->getY() + .075, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, .075),
                          getNamedChild("adsrContent"));

    createInputWithSlider("sustain_value", "Sustain Value:", 0, 1, adsr->sustain_value, std::to_string(adsr->sustain_value).substr(0, 5),
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, getConstraints("decay_timeContainer")->getY() + .075, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, .075),
                          getNamedChild("adsrContent"));

    createInputWithSlider("release_time", "Release Time:", 0, 2, adsr->release_time, std::to_string(adsr->release_time).substr(0, 5),
                          new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, getConstraints("sustain_valueContainer")->getY() + .075, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, .075),
                          getNamedChild("adsrContent"));
}