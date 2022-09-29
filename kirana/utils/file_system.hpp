#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <string>
#include <initializer_list>

namespace kirana::utils::filesystem
{
bool fileExists(const char *path);
std::string combinePath(const char *directory,
                        const std::initializer_list<const char *> &paths,
                        const char *extension = "");
} // namespace kirana::utils::filesystem
#endif