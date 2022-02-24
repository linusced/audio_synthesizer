#include "../../header/app/app_gui.hpp"

void AppGUI::loadPreset(std::string absolutePresetPath)
{
    std::ifstream input(absolutePresetPath);
    if (input.is_open())
    {
        std::string key, value;
        double dValue;

        input >> key;
        while (!input.eof())
        {
            if (key == "audio_synthesizer_main")
            {
                while (!input.eof())
                {
                    input >> key;
                    if (key.substr(0, 17) == "audio_synthesizer")
                        break;

                    input >> value;
                    try
                    {
                        dValue = std::stod(value);
                    }
                    catch (...)
                    {
                        continue;
                    }

                    if (key == "volume")
                    {
                        setVolume(dValue);
                        volumeChange = true;
                    }
                    else if (key == "stereoPan")
                    {
                        setStereoPan(dValue);
                        stereoPanChange = true;
                    }
                    else if (key == "polyphony")
                    {
                        setPolyphony(dValue);
                        polyphonyChange = true;
                    }
                }
            }
            else if (key == "audio_synthesizer_harmonics")
            {
                int h = 1;
                bool amplitude = false, phase = false;
                int fontSize = textInputs["harmonic_1_amplitude"]->getFontSize();
                glm::vec3 color = textInputs["harmonic_1_amplitude"]->getColor();

                while (!input.eof())
                {
                    input >> key;
                    if (key.substr(0, 17) == "audio_synthesizer")
                        break;

                    input >> value;
                    for (char &c : value)
                        if (c == '_')
                            c = ' ';

                    if (key == "amplitude" && h <= NUM_HARMONICS)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText(value, fontSize, color, *windowWidth, *windowHeight);
                        amplitude = true;
                    }
                    else if (key == "phase" && h <= NUM_HARMONICS)
                    {
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText(value, fontSize, color, *windowWidth, *windowHeight);
                        phase = true;
                    }

                    if (amplitude && phase)
                    {
                        h++;
                        amplitude = phase = false;
                    }
                }

                for (; h <= NUM_HARMONICS; h++)
                {
                    if (textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->getText() != "0")
                        textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                    if (textInputs["harmonic_" + std::to_string(h) + "_phase"]->getText() != "0")
                        textInputs["harmonic_" + std::to_string(h) + "_phase"]->setText("0", fontSize, color, *windowWidth, *windowHeight);
                }
                updateWaveform();
            }
            else if (key == "audio_synthesizer_adsr")
            {
                while (!input.eof())
                {
                    input >> key;
                    if (key.substr(0, 17) == "audio_synthesizer")
                        break;

                    input >> value;
                    try
                    {
                        dValue = std::stod(value);
                    }
                    catch (...)
                    {
                        continue;
                    }

                    if (key == "attack" || key == "decay" || key == "sustain" || key == "release")
                    {
                        key = key == "sustain" ? "sustain_value" : key + "_time";
                        sliders[key]->setValue(dValue);
                        dValue = sliders[key]->getValue();
                        sliderCallbacks[key](dValue);
                    }
                }
            }
            else
            {
                while (!input.eof())
                {
                    input >> key;
                    if (key.substr(0, 17) == "audio_synthesizer")
                        break;
                }
            }
        }
    }
    input.close();
}

void AppGUI::savePreset(std::string absolutePresetPath, PRESET_TYPE type)
{
    std::ofstream output(absolutePresetPath);
    if (output.is_open())
    {
        auto presetMain = [&]()
        {
            output << "audio_synthesizer_main"
                   << "\nvolume " << sliders["volume"]->getValue()
                   << "\nstereoPan " << sliders["stereoPan"]->getValue()
                   << "\npolyphony " << (int)sliders["polyphony"]->getValue();
        };
        auto presetHarmonics = [&]()
        {
            output << "audio_synthesizer_harmonics";
            int activeHarmonics = 1;
            for (int h = NUM_HARMONICS; h >= 1; h--)
                if (textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->getText() != "0")
                {
                    activeHarmonics = h;
                    break;
                }

            std::string amplitude, phase;
            for (int h = 1; h <= activeHarmonics; h++)
            {
                amplitude = textInputs["harmonic_" + std::to_string(h) + "_amplitude"]->getText();
                for (char &c : amplitude)
                    if (c == ' ')
                        c = '_';
                phase = textInputs["harmonic_" + std::to_string(h) + "_phase"]->getText();
                for (char &c : phase)
                    if (c == ' ')
                        c = '_';

                output << "\namplitude " << amplitude
                       << "\nphase " << phase;
            }
        };
        auto presetADSR = [&]()
        {
            output << "audio_synthesizer_adsr"
                   << "\nattack " << sliders["attack_time"]->getValue()
                   << "\ndecay " << sliders["decay_time"]->getValue()
                   << "\nsustain " << sliders["sustain_value"]->getValue()
                   << "\nrelease " << sliders["release_time"]->getValue();
        };

        switch (type)
        {
        case SYNTH:
        {
            presetMain();
            output << "\n\n";
            presetHarmonics();
            output << "\n\n";
            presetADSR();
        }
        break;
        case HARMONICS:
            presetHarmonics();
            break;
        case ADSR:
            presetADSR();
            break;
        }
    }
    output.close();
}