#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <vector>
#include <string>
#include <initializer_list>

namespace kirana::utils::filesystem
{
bool fileExists(const char *path);
inline bool fileExists(const std::string &path)
{
    return fileExists(path.c_str());
}

std::pair<std::string, std::string> getFilename(
    const std::string &absolutePath, bool includePeriodsInName = true);

std::vector<std::string> listFilesInPath(const std::string &directory,
                                         const std::string &filename = "");
std::string combinePath(const char *directory,
                        const std::initializer_list<const char *> &paths,
                        const char *extension = "");
} // namespace kirana::utils::filesystem
#endif