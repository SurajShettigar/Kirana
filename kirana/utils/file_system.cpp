#include "file_system.hpp"
#include <filesystem>
#include <sys/stat.h>

bool kirana::utils::filesystem::fileExists(const std::string &path)
{
    struct stat buffer
    {
    };
    return stat(path.c_str(), &buffer) == 0;
}

std::pair<std::string, std::string> kirana::utils::filesystem::getFilename(
    const std::string &absolutePath, bool includePeriodsInName)
{
    const auto &filename = std::filesystem::path(absolutePath).filename();
    std::filesystem::path name;
    std::string extension = "";

    if (includePeriodsInName)
    {
        name = filename.stem();
        extension = filename.extension().string();
    }
    else
    {
        name = filename;
        std::vector<std::string> exts;
        while (name.string().find('.') != std::string::npos)
        {
            exts.push_back(name.extension().string());
            name = name.stem();
        }
        for (auto it = exts.rbegin(); it != exts.rend(); it++)
            extension += *it;
    }
    return {name.string(), extension};
}

std::string kirana::utils::filesystem::getFolder(const std::string &path)
{
    return std::filesystem::path(path).parent_path().string();
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
            const std::string &fName =
                getFilename(f.path().string(), false).first;
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

std::string kirana::utils::filesystem::combinePath(
    const std::string &directory,
    const std::initializer_list<std::string> &paths,
    const std::string &extension)
{
    std::filesystem::path finalPath(directory);

    for (const auto &p : paths)
        finalPath = finalPath / std::filesystem::path(p);

    if (!extension.empty())
        finalPath = finalPath.replace_extension(extension);

    return finalPath.string();
}
