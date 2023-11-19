#pragma once

#include "texture.h"

namespace vera {

class TextureBump : public Texture {
public:
    virtual bool    load(const std::string& _filepath, bool _vFlip);
};

}