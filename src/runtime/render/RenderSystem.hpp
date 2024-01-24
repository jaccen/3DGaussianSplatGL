#pragma once

#include <core/ecs/System.hpp>
#include <render/Components.hpp>
#include <render/Shader.hpp>
#include <render/Texture.hpp>

#include <filesystem>

namespace jaccen
{
    class RenderSystem : public System
    {
    public:
        ADD_COMPONENT_FUNCTION(Mesh, m_meshTable);

    public:
        RenderSystem(Database& db);
        ~RenderSystem() override;

        void update(const Scene& scene) override;

        void beginFrame();
        void render();
        void endFrame();

    private:
        TableRef<Mesh> m_meshTable;

        std::vector<gfx::Shader> m_shaders;
        std::vector<gfx::Texture> m_textures;
    };
}

