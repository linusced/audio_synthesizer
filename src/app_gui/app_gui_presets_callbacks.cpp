#include "../../header/app/app_gui.hpp"

void AppGUI::createPresetsCallbacks()
{
    presetsPopupCallback = [this](int index)
    {
        newPopupCallback = nullptr;
        closePopup = true;

        if (presetsActionType == SELECT && index != -1)
        {
            if (presetsType == HARMONICS && index == 0)
                presetsFiles = {"Sine (default)", "Saw", "Square", "Triangle"};
            else if (index == 0)
                presetsFiles = {"Default"};
            else if (index == 1)
                presetsFiles = {"Save as a new preset"};
            else
                presetsFiles.clear();

            std::string path = applicationSupportPath(std::string("presets/") + (presetsType == SYNTH ? "synth" : (presetsType == HARMONICS ? "harmonics" : "adsr"))), str;
            for (auto &i : std::filesystem::directory_iterator(path))
                if ((i.path().extension() == ".dat"))
                {
                    str = i.path().filename();
                    str = str.substr(0, str.size() - 4);
                    presetsFiles.push_back(str);
                }

            presetsActionType = (PRESET_ACTION_TYPE)((int)LOAD + index);
            newPopupCallback = clickCallbacks["presets"];
        }
        else if (presetsActionType != SELECT && index == -1)
        {
            presetsActionType = SELECT;
            newPopupCallback = clickCallbacks["presets"];
        }
        else if (presetsActionType == LOAD)
        {
            if ((presetsType == HARMONICS && index < 4) || (presetsType == SYNTH && index == 0))
            {
                int fontSize = textInputs["harmonic_1_amplitude"]->getFontSize();
                glm::vec3 color = textInputs["harmonic_1_amplitude"]->getColor();

                switch (index)
                {
                case 0:
                {
                    int h = 1;
                    textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText("1", fontSize, color, *windowWidth, *windowHeight);
                    textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText("0", fontSize, color, *windowWidth, *windowHeight);

                    for (h++; h <= NUM_HARMONICS; h++)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                    }
                }
                break;
                case 1:
                {
                    for (int h = 1; h <= NUM_HARMONICS; h++)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText("1 / h", fontSize, color, *windowWidth, *windowHeight);
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                    }
                }
                break;
                case 2:
                {
                    for (int h = 1; h <= NUM_HARMONICS; h++)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText(h % 2 == 1 ? "1 / h" : "0", fontSize, color, *windowWidth, *windowHeight);
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                    }
                }
                break;
                case 3:
                {
                    for (int h = 1; h <= NUM_HARMONICS; h++)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText(h % 2 == 1 ? "1 / h^2" : "0", fontSize, color, *windowWidth, *windowHeight);
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText(h % 4 == 3 ? "pi" : "0", fontSize, color, *windowWidth, *windowHeight);
                    }
                }
                break;
                }
                updateWaveform();
            }

            if ((presetsType == ADSR || presetsType == SYNTH) && index == 0)
            {
                double value;
                sliders["attack_time"]->setValue(.01);
                value = sliders["attack_time"]->getValue();
                sliderCallbacks["attack_time"](value);

                sliders["decay_time"]->setValue(0);
                value = sliders["decay_time"]->getValue();
                sliderCallbacks["decay_time"](value);

                sliders["sustain_value"]->setValue(1);
                value = sliders["sustain_value"]->getValue();
                sliderCallbacks["sustain_value"](value);

                sliders["release_time"]->setValue(.01);
                value = sliders["release_time"]->getValue();
                sliderCallbacks["release_time"](value);

                if (presetsType == SYNTH)
                {
                    setVolume(1);
                    volumeChange = true;
                    setStereoPan(0);
                    stereoPanChange = true;
                    setPolyphony(7);
                    polyphonyChange = true;
                }
            }
            else
                loadPreset(applicationSupportPath(std::string("presets/") + (presetsType == SYNTH ? "synth/" : (presetsType == HARMONICS ? "harmonics/" : "adsr/")) + presetsFiles[index] + ".dat"));
        }
        else if (presetsActionType == SAVE)
        {
            if (index == 0)
                newPopupCallback = clickCallbacks["savePreset"];
            else
                savePreset(applicationSupportPath(std::string("presets/") + (presetsType == SYNTH ? "synth/" : (presetsType == HARMONICS ? "harmonics/" : "adsr/")) + presetsFiles[index] + ".dat"), presetsType);
        }
        else if (presetsActionType == DELETE)
        {
            std::filesystem::remove(applicationSupportPath(std::string("presets/") + (presetsType == SYNTH ? "synth/" : (presetsType == HARMONICS ? "harmonics/" : "adsr/")) + presetsFiles[index] + ".dat"));

            if (presetsFiles.size() > 1)
            {
                presetsActionType = DELETE_REFRESH;
                newPopupCallback = clickCallbacks["presets"];
            }
        }
    };

    clickCallbacks["presets"] = [this]()
    {
        switch (presetsActionType)
        {
        case SELECT:
            openPopup(std::string(presetsType == SYNTH ? "Synth" : (presetsType == HARMONICS ? "Harmonics" : "ADSR")) + " presets:", "Close", {"Load Preset", "Save Preset", "Delete Preset"}, &presetsPopupCallback);
            break;
        case LOAD:
            openPopup(std::string("Load ") + (presetsType == SYNTH ? "synth" : (presetsType == HARMONICS ? "harmonics" : "ADSR")) + " preset:", "Cancel", presetsFiles, &presetsPopupCallback);
            break;
        case SAVE:
            openPopup(std::string("Save ") + (presetsType == SYNTH ? "synth" : (presetsType == HARMONICS ? "harmonics" : "ADSR")) + " preset:", "Cancel", presetsFiles, &presetsPopupCallback);
            break;
        case DELETE:
            openPopup(std::string("Delete ") + (presetsType == SYNTH ? "synth" : (presetsType == HARMONICS ? "harmonics" : "ADSR")) + " preset:", "Cancel", presetsFiles, &presetsPopupCallback);
            break;
        case DELETE_REFRESH:
        {
            presetsActionType = SELECT;
            presetsPopupCallback(2);
        }
        break;
        }
    };

    clickCallbacks["savePreset"] = [this]()
    {
        openTextInputPopup(std::string("Save as a new ") + (presetsType == SYNTH ? "synth" : (presetsType == HARMONICS ? "harmonics" : "ADSR")) + " preset:", "Custom Preset 1", "Cancel", "Save", &savePresetTextInputCallback, &savePresetActionCallback);
    };
    savePresetActionCallback = [this](int value)
    {
        newPopupCallback = nullptr;
        closePopup = true;
        if (value && savePresetName.size() > 0)
            savePreset(applicationSupportPath(std::string("presets/") + (presetsType == SYNTH ? "synth/" : (presetsType == HARMONICS ? "harmonics/" : "adsr/")) + savePresetName + ".dat"), presetsType);
        else
            newPopupCallback = clickCallbacks["presets"];

        savePresetName.clear();
    };
    savePresetTextInputCallback = [this](std::string &text)
    {
        savePresetName = text;
    };
}