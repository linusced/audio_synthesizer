#ifndef CUSTOM_GUI_SLIDER
#define CUSTOM_GUI_SLIDER

#include "../gl/gui.hpp"

namespace customGUI
{
    class Slider : public gui::Element
    {
    private:
        int *windowWidth = nullptr, *windowHeight = nullptr;
        double min, max, value;
        std::function<void(double &)> callback = nullptr;

    public:
        Slider(double min, double max, double value, std::function<void(double &)> valueChangeCallback, gui::Constraints *constraints);

        void resizePoly(int &windowWidth, int &windowHeight) override;
        bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight) override;

        double getValue();
        void setValue(double newValue);
        void setMin(double newMin);
        void setMax(double newMax);
        bool callbackOnMouseDown = false;
    };
}

#endif