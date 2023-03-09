#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <vector>
#include <string>
#include <initializer_list>

namespace kirana::utils::filesystem
{
bool fileExists(const std::string &path);

std::pair<std::string, std::string> getFilename(
    const std::string &absolutePath, bool includePeriodsInName = true);

std::string getFolder(const std::string &path);

std::vector<std::string> listFilesInPath(const std::string &directory,
                                         const std::string &filename = "");
std::string combinePath(const std::string &directory,
                        const std::initializer_list<std::string> &paths,
                        const std::string &extension = "");
} // namespace kirana::utils::filesystem
#endif