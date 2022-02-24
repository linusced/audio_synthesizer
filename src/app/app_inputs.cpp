#include "../../header/app/app.hpp"

void App::inputs()
{
    bool guiInput = false;
    if (_gui->popupGUI)
    {
        _gui->popupGUI->inputs(window, *windowWidth, *windowHeight);
        guiInput = true;
    }
    else
        guiInput = _gui->inputs(window, *windowWidth, *windowHeight);

    if (glfwGetKey(window, GLFW_KEY_PERIOD) && !guiInput)
    {
        if (!keyDown[GLFW_KEY_PERIOD] && synth->octave < 7)
        {
            synth->octave++;
            _gui->setOctave(synth->octave);
            synth->releaseAll();
            keyDown[GLFW_KEY_PERIOD] = true;

            _gui->keysPressedMIDI.clear();
        }
    }
    else
        keyDown[GLFW_KEY_PERIOD] = false;

    if (glfwGetKey(window, GLFW_KEY_COMMA) && !guiInput)
    {
        if (!keyDown[GLFW_KEY_COMMA] && synth->octave > 0)
        {
            synth->octave--;
            _gui->setOctave(synth->octave);
            synth->releaseAll();
            keyDown[GLFW_KEY_COMMA] = true;

            _gui->keysPressedMIDI.clear();
        }
    }
    else
        keyDown[GLFW_KEY_COMMA] = false;

    for (auto &i : keysMIDI)
        if (glfwGetKey(window, i.first) && !guiInput)
        {
            if (!keyDown[i.first])
            {
                synth->addNote(i.second);
                keyDown[i.first] = true;

                _gui->keysPressedMIDI.push_back((synth->octave + 2) * 12 + i.second);
            }
        }
        else if (keyDown[i.first])
        {
            synth->releaseNote(i.second);
            keyDown[i.first] = false;

            auto it = std::find(_gui->keysPressedMIDI.begin(), _gui->keysPressedMIDI.end(), (synth->octave + 2) * 12 + i.second);
            if (it != _gui->keysPressedMIDI.end())
                _gui->keysPressedMIDI.erase(it);
        }

    if (inputMIDI)
    {
        inputMIDI->getMessage(&inputDataMIDI);
        while (inputDataMIDI.size() > 1)
        {
            short relativeOctave = (inputDataMIDI[1] / 12 - 2) - synth->octave,
                  octaveNoteID = (inputDataMIDI[1] % 12) + synth->MIDI_NOTE_ID.at("C"),
                  relativeNoteID = relativeOctave * 12 + octaveNoteID;

            /* double velocity = inputDataMIDI[2] / 60.0;
            if (velocity > 1)
                velocity = 1;
            else if (velocity < .5)
                velocity = .5; */

            if (inputDataMIDI[0] == 144)
            {
                synth->addNote(relativeNoteID /*, velocity */);

                _gui->keysPressedMIDI.push_back((synth->octave + 2) * 12 + relativeNoteID);
            }
            else if (inputDataMIDI[0] == 128)
            {
                synth->releaseNote(relativeNoteID);

                auto it = std::find(_gui->keysPressedMIDI.begin(), _gui->keysPressedMIDI.end(), (synth->octave + 2) * 12 + relativeNoteID);
                if (it != _gui->keysPressedMIDI.end())
                    _gui->keysPressedMIDI.erase(it);
            }

            inputMIDI->getMessage(&inputDataMIDI);
        }
    }
}