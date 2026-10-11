#pragma once

#include "image_io.hpp"

namespace slugcat::vrm::mipmap_gen {

image_io::LoadedImageBuffer genMipmaps(image_io::LoadedImageBuffer buffer);

}
