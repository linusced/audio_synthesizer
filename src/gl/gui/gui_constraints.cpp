#include "../../../header/gl/gui.hpp"

gui::Layout gui::Constraints::windowLayout{0, 0, 1, 1};

gui::Constraints::Constraints(GUI_CONSTRAINT x, float x_value, GUI_CONSTRAINT y, float y_value, GUI_CONSTRAINT w, float w_value, GUI_CONSTRAINT h, float h_value)
{
    auto error = [&]()
    {
        x = y = w = h = RELATIVE_CONSTRAINT;
        x_value = y_value = w_value = h_value = 0;
    };

    if (x == ASPECT_CONSTRAINT || y == ASPECT_CONSTRAINT)
    {
        std::cout << "\033[1;31mError: X or Y constraints CANNOT have type: \033[0mASPECT_CONSTRAINT\n";
        error();
    }
    if (w == CENTER_CONSTRAINT || h == CENTER_CONSTRAINT)
    {
        std::cout << "\033[1;31mError: W or H constraints CANNOT have type: \033[0mCENTER_CONSTRAINT\n";
        error();
    }
    if (w == ASPECT_CONSTRAINT && h == ASPECT_CONSTRAINT)
    {
        std::cout << "\033[1;31mError: W and H constraints CANNOT BOTH have type: \033[0mASPECT_CONSTRAINT\n";
        error();
    }
    if (w == PIXEL_CONSTRAINT_INVERSE || h == PIXEL_CONSTRAINT_INVERSE)
    {
        std::cout << "\033[1;31mError: W or H constraints CANNOT have type: \033[0mPIXEL_CONSTRAINT_INVERSE\n";
        error();
    }
    if (w == RELATIVE_CONSTRAINT_INVERSE || h == RELATIVE_CONSTRAINT_INVERSE)
    {
        std::cout << "\033[1;31mError: W or H constraints CANNOT have type: \033[0mRELATIVE_CONSTRAINT_INVERSE\n";
        error();
    }
    if (w_value < 0 || h_value < 0)
    {
        std::cout << "\033[1;33mWarning: W or H constraint values will automatically be made absolute!\033[0m\n";
    }

    this->x = x;
    this->x_value = x_value;
    this->y = y;
    this->y_value = y_value;
    this->w = w;
    this->w_value = abs(w_value);
    this->h = h;
    this->h_value = abs(h_value);
}

void gui::Constraints::setX(float x_value)
{
    this->x_value = x_value;
}
void gui::Constraints::setY(float y_value)
{
    this->y_value = y_value;
}
void gui::Constraints::setW(float w_value)
{
    this->w_value = abs(w_value);
}
void gui::Constraints::setH(float h_value)
{
    this->h_value = abs(h_value);
}

float gui::Constraints::getX()
{
    return x_value;
}
float gui::Constraints::getY()
{
    return y_value;
}
float gui::Constraints::getW()
{
    return w_value;
}
float gui::Constraints::getH()
{
    return h_value;
}

void gui::Constraints::setConstraintX(GUI_CONSTRAINT x)
{
    this->x = x;
}
void gui::Constraints::setConstraintY(GUI_CONSTRAINT y)
{
    this->y = y;
}
void gui::Constraints::setConstraintW(GUI_CONSTRAINT w)
{
    this->w = w;
}
void gui::Constraints::setConstraintH(GUI_CONSTRAINT h)
{
    this->h = h;
}

gui::Layout *gui::Constraints::getLayout(Layout *parentLayout, int &windowWidth, int &windowHeight)
{
    Layout *l = new Layout();

    auto calcConstraint = [&](char constraint)
    {
        if (constraint == 'x')
            switch (x)
            {
            case PIXEL_CONSTRAINT:
                l->x = (x_value / windowWidth) + parentLayout->x;
                break;
            case PIXEL_CONSTRAINT_INVERSE:
                l->x = (parentLayout->x + parentLayout->w) - (x_value / windowWidth) - l->w;
                break;
            case RELATIVE_CONSTRAINT:
                l->x = (x_value * parentLayout->w) + parentLayout->x;
                break;
            case RELATIVE_CONSTRAINT_INVERSE:
                l->x = (parentLayout->x + parentLayout->w) - (x_value * parentLayout->w) - l->w;
                break;
            case CENTER_CONSTRAINT:
                l->x = parentLayout->x + (.5 * parentLayout->w) - (l->w / 2);
                break;
            }
        else if (constraint == 'y')
            switch (y)
            {
            case PIXEL_CONSTRAINT:
                l->y = (y_value / windowHeight) + parentLayout->y;
                break;
            case PIXEL_CONSTRAINT_INVERSE:
                l->y = (parentLayout->y + parentLayout->h) - (y_value / windowHeight) - l->h;
                break;
            case RELATIVE_CONSTRAINT:
                l->y = (y_value * parentLayout->h) + parentLayout->y;
                break;
            case RELATIVE_CONSTRAINT_INVERSE:
                l->y = (parentLayout->y + parentLayout->h) - (y_value * parentLayout->h) - l->h;
                break;
            case CENTER_CONSTRAINT:
                l->y = parentLayout->y + (.5 * parentLayout->h) - (l->h / 2);
                break;
            }
        else if (constraint == 'w')
            switch (w)
            {
            case PIXEL_CONSTRAINT:
                l->w = w_value / windowWidth;
                break;
            case RELATIVE_CONSTRAINT:
                l->w = w_value * parentLayout->w;
                break;
            case ASPECT_CONSTRAINT:
                l->w = w_value * l->h * ((float)windowHeight / (float)windowWidth);
                break;
            }
        else if (constraint == 'h')
            switch (h)
            {
            case PIXEL_CONSTRAINT:
                l->h = h_value / windowHeight;
                break;
            case RELATIVE_CONSTRAINT:
                l->h = h_value * parentLayout->h;
                break;
            case ASPECT_CONSTRAINT:
                l->h = h_value * l->w * ((float)windowWidth / (float)windowHeight);
                break;
            }
    };

    if (w == ASPECT_CONSTRAINT)
        calcConstraint('h');
    calcConstraint('w');
    if (w != ASPECT_CONSTRAINT)
        calcConstraint('h');
    calcConstraint('x');
    calcConstraint('y');

    return l;
}
gui::Layout *gui::Constraints::getWindowLayout()
{
    return &windowLayout;
}