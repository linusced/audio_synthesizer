#ifndef GUI
#define GUI

#include "polygon.hpp"
#include "gui_utility.hpp"

#include <GLFW/glfw3.h>
#include <map>
#include <functional>

#ifndef STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#endif
#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#endif

namespace gui
{
    enum GUI_CONSTRAINT
    {
        PIXEL_CONSTRAINT,
        RELATIVE_CONSTRAINT,
        CENTER_CONSTRAINT,
        ASPECT_CONSTRAINT,
        PIXEL_CONSTRAINT_INVERSE,
        RELATIVE_CONSTRAINT_INVERSE
    };
    enum ALIGNMENT_H
    {
        ALIGNMENT_LEFT_H,
        ALIGNMENT_CENTER_H,
        ALIGNMENT_RIGHT_H
    };
    enum ALIGNMENT_V
    {
        ALIGNMENT_TOP_V,
        ALIGNMENT_CENTER_V,
        ALIGNMENT_BOTTOM_V
    };

    struct Layout
    {
        float x, y, w, h;
    };
    struct Font
    {
        unsigned char *buffer;
        Font(std::string filePath);
    };

    class Constraints
    {
    private:
        GUI_CONSTRAINT x, y, w, h;
        float x_value, y_value, w_value, h_value;
        static Layout windowLayout;

    public:
        Constraints(GUI_CONSTRAINT x, float x_value, GUI_CONSTRAINT y, float y_value, GUI_CONSTRAINT w, float w_value, GUI_CONSTRAINT h, float h_value);

        void setX(float x_value);
        void setY(float y_value);
        void setW(float w_value);
        void setH(float h_value);

        float getX();
        float getY();
        float getW();
        float getH();

        void setConstraintX(GUI_CONSTRAINT x);
        void setConstraintY(GUI_CONSTRAINT y);
        void setConstraintW(GUI_CONSTRAINT w);
        void setConstraintH(GUI_CONSTRAINT h);

        Layout *getLayout(Layout *parentLayout, int &windowWidth, int &windowHeight);
        static Layout *getWindowLayout();
    };

    class Element
    {
    protected:
        Constraints *c = nullptr;
        std::vector<Element *> children;

        std::vector<Polygon *> poly;
        Layout *layout = nullptr;
        bool mouseDrag = false;

    public:
        Element(Constraints *constraints);
        ~Element();

        void addChild(Element *child);
        void resize(Layout *parentLayout, int &windowWidth, int &windowHeight);
        Layout getRelativePixelCoords(Layout *parentLayout, int &windowWidth, int &windowHeight);
        bool mouseDragActive();
        bool draw_b = true, inputs_b = true;

        virtual void resizePoly(int &windowWidth, int &windowHeight);
        virtual bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight);
        virtual void draw();
    };

    class Master
    {
    protected:
        std::vector<Element *> children;
        bool prevBlock = false;

        std::map<std::string, Element *> children_named;
        std::map<std::string, Constraints *> constraints_named;

    public:
        ~Master();

        void addChild(Element *child);
        void resize(int &windowWidth, int &windowHeight);
        void draw();
        bool inputs(GLFWwindow *window, int &windowWidth, int &windowHeight);

        void saveNamedChild(std::string name, Element *child, bool add = false);
        Element *getNamedChild(std::string name);
        void saveConstraints(std::string name, Constraints *con);
        Constraints *getConstraints(std::string name);
    };

    namespace native
    {
        class Container : public Element
        {
        private:
            int borderRadius{0}, borderWidth{0};

        public:
            Container(glm::vec4 backgroundColor, int borderRadius, Constraints *constraints);
            Container(glm::vec4 backgroundColor, int borderWidth, glm::vec4 borderColor, Constraints *constraints);

            void resizePoly(int &windowWidth, int &windowHeight) override;
        };

        class Text : public Element
        {
        private:
            Font *font;
            unsigned char *textData = nullptr;
            std::string text;
            glm::vec3 color, scrollTrackColor{0, 0, 0}, scrollThumbColor{.6, .6, .6};
            int fontSize, textWidth{0}, textHeight{0}, lineScroll{0}, currentLines{0}, visibleLines{0}, scrollDragPrevY{-1};
            float textSizeMult{1};
            bool keepFontSize, scrollResize = false;
            ALIGNMENT_H alignment_h;
            ALIGNMENT_V alignment_v;
            float *charWidth = nullptr;
            int numOfChars{0};

            void createData(std::string text, int fontSize, glm::vec3 color, int maxWidth = 0, int maxHeight = 0);

        public:
            Text(Font *font, std::string text, int fontSize, glm::vec3 color, bool keepFontSize, ALIGNMENT_H textAlignment_horizontal, ALIGNMENT_V textAlignment_vertical, Constraints *constraints);

            void setText(std::string text, int fontSize, glm::vec3 color, int &windowWidth, int &windowHeight);
            std::string getText();
            int getFontSize();
            glm::vec3 getColor();
            int getTextWidth();
            int getTextHeight();
            float getCharWidth(int index);

            void setScrollSliderColors(glm::vec3 trackColor, glm::vec3 thumbColor);

            void resizePoly(int &windowWidth, int &windowHeight) override;
            void scrollPoly(int &windowWidth, int &windowHeight);
            bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight) override;
        };

        class Image : public Element
        {
        private:
            Texture *t = nullptr;
            int imgWidth, imgHeight;
            bool scaleToCoverRect;
            ALIGNMENT_H alignment_h;
            ALIGNMENT_V alignment_v;

        public:
            Image(std::string filePath, bool scaleToCoverRect, ALIGNMENT_H imgAlignment_horizontal, ALIGNMENT_V imgAlignment_vertical, Constraints *constraints);
            ~Image();

            void updateImage(std::string filePath);

            void resizePoly(int &windowWidth, int &windowHeight) override;
        };

        class Button : public Container
        {
        private:
            glm::vec4 defaultColor, clickColor;
            std::function<void()> clickCallback;
            void changeColor(int color);

        public:
            Button(glm::vec4 color, int borderRadius, std::function<void()> onClick, Constraints *constraints);
            Button(glm::vec4 color, int borderWidth, glm::vec4 borderColor, std::function<void()> onClick, Constraints *constraints);

            void setClickColor(glm::vec4 color);
            void setDefaultColor(glm::vec4 color);
            bool keepClickColorAfterClick = false;

            bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight) override;
        };

        class TextInput : public Text
        {
        private:
            bool arrowLeftKeyDown = false, arrowRightKeyDown = false, backspaceKeyDown = false;
            double lastChange{0}, backspaceKeyDownTime{0}, arrowLeftKeyDownTime{0}, arrowRightKeyDownTime{0};
            unsigned int textCursorOffset{0};

            std::function<void(std::string &)> escapeCallback, enterCallback;

            void textCursor(bool active, int &windowWidth, int &windowHeight);

        public:
            TextInput(Font *font, std::string defaultText, int fontSize, glm::vec3 textColor, std::function<void(std::string &)> onInputEscape, std::function<void(std::string &)> onInputEnter, Constraints *constraints);

            bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight) override;
        };
    }
}

#endif