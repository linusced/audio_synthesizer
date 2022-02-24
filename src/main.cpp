#include "../header/gl/init.hpp"
#include "../header/app/app.hpp"

bool loadWindowPosition(GLFWwindow *window)
{
    int windowSizeX = -1, windowSizeY = -1, windowSizeW = -1, windowSizeH = -1;

    std::ifstream windowInput(applicationSupportPath("window.dat"));
    if (windowInput.is_open())
    {
        std::string key, value;
        windowInput >> key;
        if (key == "audio_synthesizer_window")
            while (!windowInput.eof())
            {
                windowInput >> key;
                windowInput >> value;

                bool invalidNum = false;
                for (auto &c : value)
                    if (c < '0' || c > '9')
                    {
                        invalidNum = true;
                        break;
                    }
                if (!invalidNum)
                {
                    if (key == "x")
                        windowSizeX = std::stoi(value);
                    else if (key == "y")
                        windowSizeY = std::stoi(value);
                    else if (key == "w")
                        windowSizeW = std::stoi(value);
                    else if (key == "h")
                        windowSizeH = std::stoi(value);
                }
            }
    }
    windowInput.close();

    if (windowSizeX != -1 && windowSizeY != -1 && windowSizeW != -1 && windowSizeH != -1)
    {
        glfwSetWindowPos(window, windowSizeX, windowSizeY);
        glfwSetWindowSize(window, windowSizeW, windowSizeH);
        return true;
    }
    return false;
}

void saveWindowPosition(GLFWwindow *window)
{
    int windowSizeX, windowSizeY, windowSizeW, windowSizeH;
    glfwGetWindowPos(window, &windowSizeX, &windowSizeY);
    glfwGetWindowSize(window, &windowSizeW, &windowSizeH);

    std::ofstream windowOutput(applicationSupportPath("window.dat"));
    if (windowOutput.is_open())
    {
        windowOutput << "audio_synthesizer_window"
                     << "\nx " << windowSizeX
                     << "\ny " << windowSizeY
                     << "\nw " << windowSizeW
                     << "\nh " << windowSizeH;
    }
    windowOutput.close();
}

int main()
{
    std::vector<std::string> fileDirectories{applicationSupportPath(""), applicationSupportPath("presets"), applicationSupportPath("presets/synth"), applicationSupportPath("presets/harmonics"), applicationSupportPath("presets/adsr")};
    for (auto &dir : fileDirectories)
        if (!std::filesystem::is_directory(dir))
            std::filesystem::create_directory(dir);

    fileDirectories.clear();

    int windowWidth = 1280, windowHeight = 720;
    GLFWwindow *window = nullptr;

    if (!init(window, windowWidth, windowHeight, "Audio Synthesizer"))
        return 1;

    if (!loadWindowPosition(window))
    {
        auto *monitor = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (monitor)
        {
            glfwSetWindowSize(window, monitor->width, monitor->height);
            glfwSetWindowPos(window, 0, 0);
        }
    }

    App audioApp{window, &windowWidth, &windowHeight};
    audioApp();

    saveWindowPosition(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}