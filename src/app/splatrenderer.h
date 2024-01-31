
#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <stdint.h>
#include <vector>
#include "render/Texture.hpp"
#include "render/Shader.hpp"
#include "render/components/Mesh.hpp"

#include "gaussiancloud.h"


namespace rgc::radix_sort
{
    struct sorter;
}

class SplatRenderer
{
public:
    SplatRenderer();
    ~SplatRenderer();

    bool Init(std::shared_ptr<GaussianCloud> gaussianCloud, bool isFramebufferSRGBEnabledIn,
              bool useFullSHIn);

    void Sort(const glm::mat4& cameraMat, const glm::mat4& projMat,
              const glm::vec4& viewport, const glm::vec2& nearFar);

    // viewport = (x, y, width, height)
    void Render(const glm::mat4& cameraMat, const glm::mat4& projMat,
                const glm::vec4& viewport, const glm::vec2& nearFar);
protected:
    void BuildVertexArrayObject(std::shared_ptr<GaussianCloud> gaussianCloud);

    std::shared_ptr<jaccen::gfx::Shader> splatProg;
    std::shared_ptr<jaccen::gfx::Shader> preSortProg;
    std::shared_ptr<jaccen::VertexArrayObject> splatVao;

    std::vector<uint32_t> indexVec;
    std::vector<uint32_t> depthVec;
    std::vector<glm::vec4> posVec;
    std::vector<uint32_t> atomicCounterVec;

    std::shared_ptr<jaccen::BufferObject> keyBuffer;
    std::shared_ptr<jaccen::BufferObject> valBuffer;
    std::shared_ptr<jaccen::BufferObject> posBuffer;
    std::shared_ptr<jaccen::BufferObject> atomicCounterBuffer;

    std::shared_ptr<rgc::radix_sort::sorter> sorter;
    uint32_t sortCount;
    bool isFramebufferSRGBEnabled;
    bool useFullSH;
};
