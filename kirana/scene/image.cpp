#include "image.hpp"

#include <file_system.hpp>

kirana::scene::Image::Image(std::string filepath, ImageProperties properties)
    : Object(""), m_filepath{std::move(filepath)}, m_properties{properties}
{
    auto fName = utils::filesystem::getFilename(m_filepath);
    m_name = fName.first + fName.second;
}