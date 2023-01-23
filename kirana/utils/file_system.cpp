#include "file_system.hpp"
#include <filesystem>
#include <sys/stat.h>

bool kirana::utils::filesystem::fileExists(const char *path)
{
    struct stat buffer
    {
    };
    return stat(path, &buffer) == 0;
}

std::vector<std::string> kirana::utils::filesystem::listFilesInPath(
    const std::string &directory, const std::string &filename)
{
    const std::filesystem::path dirPath(directory);
    std::vector<std::string> files;

    for (const auto &f : std::filesystem::directory_iterator(dirPath))
    {
        if (!f.is_directory())
        {
            const std::string &fName = f.path().filename().stem().string();
            if (!filename.empty())
            {
                if (fName == filename)
                    files.push_back(f.path().string());
            }
            else
                files.push_back(f.path().string());
        }
    }
    return files;
}


std::pair<std::string, std::string> kirana::utils::filesystem::getFilename(
    const std::string &absolutePath)
{
    const auto &filename = std::filesystem::path(absolutePath).filename();
    return {filename.stem().string(), filename.extension().string()};
}

std::string kirana::utils::filesystem::combinePath(
    const char *directory, const std::initializer_list<const char *> &paths,
    const char *extension)
{
    std::filesystem::path finalPath(directory);

    for (const char *p : paths)
        finalPath = finalPath / std::filesystem::path(p);

    if (*extension != '\0')
        finalPath = finalPath.replace_extension(extension);

    return finalPath.string();
}
