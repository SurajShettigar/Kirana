#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <string>
#include <initializer_list>

namespace kirana::utils::filesystem
{
bool fileExists(const char *path);
inline bool fileExists(const std::string &path)
{
    return fileExists(path.c_str());
}
std::vector<std::string> listFilesInPath(const std::string &directory,
                                         const std::string &filename = "");
std::pair<std::string, std::string> getFilename(const std::string &absolutePath);
std::string combinePath(const char *directory,
                        const std::initializer_list<const char *> &paths,
                        const char *extension = "");
} // namespace kirana::utils::filesystem
#endif