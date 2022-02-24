#include "../header/files.hpp"

CFStringRef appBundle_UTI;
const bool isAppBundle = CFURLCopyResourcePropertyForKey(CFBundleCopyBundleURL(CFBundleGetMainBundle()), kCFURLTypeIdentifierKey, &appBundle_UTI, NULL) && appBundle_UTI && UTTypeConformsTo(appBundle_UTI, kUTTypeApplicationBundle);
std::string resourcesDir;
bool resourcesDir_init = false;

std::string filePath(std::string relativePath)
{
    if (relativePath.substr(0, 7) == "assets/")
        relativePath = relativePath.substr(7);
    if (!resourcesDir_init)
    {
        resourcesDir_init = true;
        if (isAppBundle)
        {
            char path[PATH_MAX];
            CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
            CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8 *)path, PATH_MAX);
            CFRelease(resourcesURL);
            chdir(path);

            resourcesDir = std::string(path) + '/';
        }
        else
            resourcesDir = "assets/";
    }
    return resourcesDir + relativePath;
}

std::string applicationSupportDir;
bool applicationSupportDir_init = false;

std::string applicationSupportPath(std::string relativePath)
{
    if (!applicationSupportDir_init)
    {
        applicationSupportDir_init = true;

        char path[PATH_MAX];
        FSRef ref;
        FSFindFolder(kUserDomain, kApplicationSupportFolderType, kCreateFolder, &ref);
        FSRefMakePath(&ref, (UInt8 *)&path, PATH_MAX);

        applicationSupportDir = std::string(path) + "/Audio Synthesizer/";
    }
    return applicationSupportDir + relativePath;
}
