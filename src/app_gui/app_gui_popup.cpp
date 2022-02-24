#include "../../header/app/app_gui.hpp"

void AppGUI::openPopup(std::string title, std::string closeBtnText, std::vector<std::string> items, std::function<void(int)> *callback)
{
    popupGUI = new gui::Master();
    popupGUI->saveNamedChild("popupFullscreen", new gui::native::Container({0, 0, 0, .5}, 0, getConstraints("fill")), true);
    popupGUI->saveConstraints("popupContainer", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .4, gui::RELATIVE_CONSTRAINT, .9));
    popupGUI->saveNamedChild("popupContainer", new gui::native::Container({.1, .1, .1, 1}, 2, glm::vec4(1), popupGUI->getConstraints("popupContainer")));
    popupGUI->getNamedChild("popupFullscreen")->addChild(popupGUI->getNamedChild("popupContainer"));

    popupGUI->saveConstraints("popupTitleText", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .1));
    popupGUI->getNamedChild("popupContainer")->addChild(new gui::native::Text(font, title, 35, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, popupGUI->getConstraints("popupTitleText")));

    popupGUI->saveConstraints("popupItemCloseBtn", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, .01, gui::RELATIVE_CONSTRAINT, .2, gui::RELATIVE_CONSTRAINT, .08));
    popupGUI->getNamedChild("popupContainer")->addChild(new customGUI::PopupItem(closeBtnText, -1, callback, glm::vec4(1), {1, .5, .3, 1}, glm::vec3(0), font, getConstraints("btnText"), popupGUI->getConstraints("popupItemCloseBtn"), true));

    double height = 1.0 / (items.size() < 10 ? 10 : items.size()) * .8;
    for (int i = 0; i < items.size(); i++)
    {
        popupGUI->saveConstraints("popupItem" + std::to_string(i), new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .1 + i * height, gui::RELATIVE_CONSTRAINT, .9, gui::RELATIVE_CONSTRAINT, height * .9));
        popupGUI->getNamedChild("popupContainer")->addChild(new customGUI::PopupItem(items[i], i, callback, {.1, .2, .4, 1}, {0, .5, 1, 1}, glm::vec3(1), font, getConstraints("btnText"), popupGUI->getConstraints("popupItem" + std::to_string(i))));
    }

    popupGUI->resize(*windowWidth, *windowHeight);
}

void AppGUI::openTextInputPopup(std::string title, std::string defaultText, std::string cancelBtnText, std::string continueBtnText, std::function<void(std::string &)> *textInputCallback, std::function<void(int)> *actionCallback)
{
    popupGUI = new gui::Master();
    popupGUI->saveNamedChild("popupFullscreen", new gui::native::Container({0, 0, 0, .5}, 0, getConstraints("fill")), true);
    popupGUI->saveConstraints("popupContainer", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .3, gui::RELATIVE_CONSTRAINT, .25));
    popupGUI->saveNamedChild("popupContainer", new gui::native::Container({.1, .1, .1, 1}, 2, glm::vec4(1), popupGUI->getConstraints("popupContainer")));
    popupGUI->getNamedChild("popupFullscreen")->addChild(popupGUI->getNamedChild("popupContainer"));

    popupGUI->saveConstraints("popupTitleText", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, 1, gui::RELATIVE_CONSTRAINT, .4));
    popupGUI->getNamedChild("popupContainer")->addChild(new gui::native::Text(font, title, 35, glm::vec3(1), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, popupGUI->getConstraints("popupTitleText")));

    popupGUI->saveConstraints("popupTextInputContainer", new gui::Constraints(gui::CENTER_CONSTRAINT, 0, gui::CENTER_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT, .8, gui::RELATIVE_CONSTRAINT, .25));
    popupGUI->saveNamedChild("popupTextInputContainer", new gui::native::Container(glm::vec4(1), 2, {0, 0, 0, 1}, popupGUI->getConstraints("popupTextInputContainer")));
    popupGUI->getNamedChild("popupContainer")->addChild(popupGUI->getNamedChild("popupTextInputContainer"));
    popupGUI->getNamedChild("popupTextInputContainer")->addChild(new gui::native::TextInput(font, defaultText, 40, glm::vec3(0), *textInputCallback, *textInputCallback, getConstraints("btnText")));

    popupGUI->saveConstraints("popupItemCancelBtn", new gui::Constraints(gui::RELATIVE_CONSTRAINT_INVERSE, .52, gui::RELATIVE_CONSTRAINT_INVERSE, .1, gui::RELATIVE_CONSTRAINT, .38, gui::RELATIVE_CONSTRAINT, .2));
    popupGUI->getNamedChild("popupContainer")->addChild(new customGUI::PopupItem(cancelBtnText, 0, actionCallback, {1, .5, .3, 1}, {1, 0, 0, 1}, glm::vec3(1), font, getConstraints("btnText"), popupGUI->getConstraints("popupItemCancelBtn"), true));

    popupGUI->saveConstraints("popupItemContinueBtn", new gui::Constraints(gui::RELATIVE_CONSTRAINT, .52, gui::RELATIVE_CONSTRAINT_INVERSE, .1, gui::RELATIVE_CONSTRAINT, .38, gui::RELATIVE_CONSTRAINT, .2));
    popupGUI->getNamedChild("popupContainer")->addChild(new customGUI::PopupItem(continueBtnText, 1, actionCallback, {.5, .8, .2, 1}, {0, 1, 0, 1}, glm::vec3(1), font, getConstraints("btnText"), popupGUI->getConstraints("popupItemContinueBtn"), true));

    popupGUI->resize(*windowWidth, *windowHeight);

    textInputCallback->operator()(defaultText);
}