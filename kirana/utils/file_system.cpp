#include "file_system.hpp"
#include <boost/filesystem.hpp>
#include <sys/stat.h>

namespace bf = boost::filesystem;

bool kirana::utils::filesystem::fileExists(const char *path)
{
    struct stat buffer
    {
    };
    return stat(path, &buffer) == 0;
}


std::string kirana::utils::filesystem::combinePath(
    const char *directory, const std::initializer_list<const char *> &paths,
    const char *extension)
{
    bf::path finalPath(directory);

    for (const char *p : paths)
        finalPath = finalPath / bf::path(p);

    if(*extension != '\0')
        finalPath = finalPath.replace_extension(extension);

    return finalPath.string();
}
