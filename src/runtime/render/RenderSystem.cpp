#include <render/RenderSystem.hpp>

#include <core/Core.hpp>
#include <editor/Components.hpp>
#include <scene/Scene.hpp>
#include <scene/components/Transform.hpp>
#include <ui/Window.hpp>

using namespace jaccen;
using namespace jaccen::gfx;

RenderSystem::RenderSystem(Database& db) :
    m_meshTable(db.createTable<Mesh>())
{
    m_shaders.emplace_back(Shader(
        "shaders/vertex.vert",
        "shaders/fragment.frag"));
    m_shaders.emplace_back(Shader(
        "shaders/vertex.vert",
        "shaders/fragment_single.frag"));
    m_shaders.emplace_back(Shader(
        "shaders/vertex.vert",
        "shaders/fragment_single.frag"));

    m_textures.emplace_back(Texture(
        "data/container.jpg", GL_CLAMP_TO_EDGE, GL_NEAREST, GL_RGB));
    m_textures.emplace_back(Texture(
        "data/awesomeface.png", GL_REPEAT, GL_NEAREST, GL_RGBA));
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(const Scene&)
{
    query()
        .hasComponent<Added>()
        .hasComponent<Mesh>(m_meshTable)
        .execute([&](
            EntityId,
            const Added&,
            Mesh& mesh)
    {
        // Generate and bind vertex array object
        glGenVertexArrays(1, &mesh.VAO);
        glBindVertexArray(mesh.VAO);
        
        // Generate buffers
        glGenBuffers(1, &mesh.VBOV);
        glGenBuffers(1, &mesh.VBOC);
        glGenBuffers(1, &mesh.EBO);

        // Bind vertex buffer object for vertices
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOV);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        // Bind vertex buffer object for vertex colors
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOC);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mesh.colors.size(), &mesh.colors[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);

        // Bind element buffer object for vertex indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * mesh.indices.size(), &mesh.indices[0], GL_STATIC_DRAW);
    });
        
    query()
        .hasComponent<Updated>()
        .hasComponent<Transform>()
        .hasComponent<Mesh>(m_meshTable)
        .execute([&](
            EntityId,
            const Updated&,
            const Transform& transform,
            Mesh& mesh)
    {
        // Compute axis-aligned bounding box for updated rotation and scale,
        // so that it fully contains the mesh in any orientation
        {
            // TODO: Sooo... we might not want to orientate the AABB at all after its 
            // initially constructed from the mesh vertices, but instead use and expose
            // the OBB as the bounding volume for the mesh.

            mesh.aabb.clear();

            mat4 rotate = glm::mat4_cast(transform.rotation);
            mat4 scale = glm::scale(mat4(1.0f), transform.scale);
            mat4 mat = rotate * scale;

            for (auto& v : mesh.vertices)
            {
                vec4 p = mat * vec4(v, 1.0f);
                // p /= p.w; // Always 1.0 since we ignore position

                mesh.aabb.expand(p);
            }
        }

        {
            // ALTERNATIVE:
            //vec4 xa = model[0] * mesh.aabb.min().x;
            //vec4 xb = model[0] * mesh.aabb.max().x;

            //vec4 ya = model[1] * mesh.aabb.min().y;
            //vec4 yb = model[1] * mesh.aabb.max().y;

            //vec4 za = model[2] * mesh.aabb.min().z;
            //vec4 zb = model[2] * mesh.aabb.max().z;

            //vec4 min = glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + model[3];
            //min /= min.w;

            //vec4 max = glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + model[3];
            //max /= max.w;

            //mesh.aabb.clear();
            //mesh.aabb.expand(min);
            //mesh.aabb.expand(max);
        }

        // Compute object oriented bounding box
        {
            // NOTE: Use untransformed AABB so mesh rotation doesn't affect extents
            AABB aabb;
            for (auto& v : mesh.vertices)
            {
                aabb.expand(v);
            }
            
            //mat4 rotate = glm::mat4_cast(transform.rotation);

            // OBB orientation in world space. Don't include scale,
            // because scale should only affect size
            //mat4 model = translate * rotate;

            // OBB world position
            mat4 translate = glm::translate(mat4(1.0f), transform.position);
            (void)translate;
            //vec4 position = translate * vec4(aabb.center(), 1.0f); // translate * rotate * vec4(aabb.center(), 1.0f);
            //position /= position.w;

            // OBB size
            mat4 scale = glm::scale(mat4(1.0f), transform.scale);
            vec4 halfExtents = scale * vec4(aabb.halfExtents(), 1.0f);
            (void)halfExtents;

            //mesh.obb.position = vec3(position);
            //mesh.obb.halfExtents = vec3(halfExtents);
            //mesh.obb.rotation = mat3(rotate); // mat3(model);

            // TODO: Matrix transformation or direct vector calculation?
            mesh.obb.position = transform.position + aabb.center();
            mesh.obb.halfExtents = transform.scale * aabb.halfExtents();
            mesh.obb.rotation = glm::mat3_cast(transform.rotation);
        }
    });

    query()
        .hasComponent<Deleted>()
        .hasComponent<Mesh>(m_meshTable)
        .execute([&](
            EntityId, 
            const Deleted&,
            Mesh& mesh)
    {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBOV);
        glDeleteBuffers(1, &mesh.VBOC);
        glDeleteBuffers(1, &mesh.EBO);
    });
}

void RenderSystem::beginFrame()
{
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderSystem::render()
{
    auto camera = query().find<Camera>();
    assert(camera != nullptr && "No camera in scene");

    // Draw meshes
    query()
        .hasComponent<Transform>()
        .hasComponent<Mesh>()
        .execute([&](
            EntityId, 
            const Transform& transform,
            const Mesh& mesh)
    {
        glEnable(GL_STENCIL_TEST);

        // Write to stencil buffer
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        m_shaders[0].use();
        m_shaders[0].setMat4("view", camera->viewMatrix);
        m_shaders[0].setMat4("projection", camera->projectionMatrix);
        m_shaders[0].setMat4("model", transform.modelMatrix());

        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDisable(GL_STENCIL_TEST);
    });

    // Draw hovered meshes outline
    query()
        .hasComponent<Transform>()
        .hasComponent<Mesh>()
        .hasComponent<Hovered>()
        .execute([&](
            EntityId,
            const Transform& transform,
            const Mesh& mesh,
            const Hovered&)
    {
        glEnable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);

        // Disable writing to the stencil buffer
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        
        m_shaders[1].use();
        m_shaders[1].setMat4("view", camera->viewMatrix);
        m_shaders[1].setMat4("projection", camera->projectionMatrix);
        m_shaders[1].setMat4("model", transform.modelMatrix());
        m_shaders[1].setVec3("color", vec3(1.0f, 0.9f, 0.3f));
        
        glBindVertexArray(mesh.VAO);
        glLineWidth(4.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.0f);
        glBindVertexArray(0);

        glStencilMask(0xFF);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    });

    // Draw selected meshes outline
    query()
        .hasComponent<Transform>()
        .hasComponent<Mesh>()
        .hasComponent<Selected>()
        .execute([&](
            EntityId,
            const Transform& transform,
            const Mesh& mesh,
            const Selected&)
    {
        glEnable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);

        // Disable writing to the stencil buffer
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        m_shaders[1].use();
        m_shaders[1].setMat4("view", camera->viewMatrix);
        m_shaders[1].setMat4("projection", camera->projectionMatrix);
        m_shaders[1].setMat4("model", transform.modelMatrix());
        m_shaders[1].setVec3("color", vec3(0.2f, 1.0f, 0.4f));

        glBindVertexArray(mesh.VAO);
        glLineWidth(4.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.0f);
        glBindVertexArray(0);

        glStencilMask(0xFF);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    });
    
    // Draw AABBs
    query()
        .hasComponent<Transform>()
        .hasComponent<Mesh>()
        .execute([&](
            EntityId,
            const Transform& transform,
            const Mesh& mesh)
    {
        auto min = mesh.aabb.min();
        auto max = mesh.aabb.max();

        auto vertices = std::vector<vec3>
        {
            vec3(min.x, min.y, min.z),
            vec3(max.x, min.y, min.z),

            vec3(max.x, min.y, min.z),
            vec3(max.x, max.y, min.z),

            vec3(max.x, max.y, min.z),
            vec3(min.x, max.y, min.z),

            vec3(min.x, max.y, min.z),
            vec3(min.x, min.y, min.z),

            vec3(min.x, min.y, min.z),
            vec3(min.x, min.y, max.z),

            vec3(min.x, min.y, max.z),
            vec3(max.x, min.y, max.z),

            vec3(max.x, min.y, max.z),
            vec3(max.x, max.y, max.z),

            vec3(max.x, max.y, max.z),
            vec3(min.x, max.y, max.z),

            vec3(min.x, max.y, max.z),
            vec3(min.x, max.y, min.z),

            vec3(min.x, max.y, min.z),
            vec3(min.x, min.y, min.z),

            vec3(min.x, min.y, min.z),
            vec3(min.x, min.y, max.z),

            vec3(min.x, min.y, max.z),
            vec3(min.x, max.y, max.z),

            vec3(min.x, max.y, max.z),
            vec3(max.x, max.y, max.z),

            vec3(max.x, max.y, max.z),
            vec3(max.x, max.y, min.z),

            vec3(max.x, max.y, min.z),
            vec3(max.x, min.y, min.z),

            vec3(max.x, min.y, min.z),
            vec3(max.x, min.y, max.z)
        };

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        mat4 translate = glm::translate(mat4(1.0f), transform.position);
        mat4 rotate = mat4(1.0f);
        mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
        mat4 model = translate * rotate * scale;

        m_shaders[2].use();
        m_shaders[2].setMat4("view", camera->viewMatrix);
        m_shaders[2].setMat4("projection", camera->projectionMatrix);
        m_shaders[2].setMat4("model", model);
        m_shaders[2].setVec3("color", vec3(0.6f, 0.7f, 0.9f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);
    });

    // Draw OBBs
    query()
        .hasComponent<Transform>()
        .hasComponent<Mesh>()
        .execute([&](
            EntityId,
            const Transform&,
            const Mesh& mesh)
    {
        vec3 pos = mesh.obb.position;
        vec3 ext = mesh.obb.halfExtents;
        mat3 rot = mesh.obb.rotation;
        vec3 min = -ext;
        vec3 max = +ext;

        auto vertices = std::vector<vec3>
        {
            vec3(min.x, min.y, min.z),
            vec3(max.x, min.y, min.z),

            vec3(max.x, min.y, min.z),
            vec3(max.x, max.y, min.z),

            vec3(max.x, max.y, min.z),
            vec3(min.x, max.y, min.z),

            vec3(min.x, max.y, min.z),
            vec3(min.x, min.y, min.z),

            vec3(min.x, min.y, min.z),
            vec3(min.x, min.y, max.z),

            vec3(min.x, min.y, max.z),
            vec3(max.x, min.y, max.z),

            vec3(max.x, min.y, max.z),
            vec3(max.x, max.y, max.z),

            vec3(max.x, max.y, max.z),
            vec3(min.x, max.y, max.z),

            vec3(min.x, max.y, max.z),
            vec3(min.x, max.y, min.z),

            vec3(min.x, max.y, min.z),
            vec3(min.x, min.y, min.z),

            vec3(min.x, min.y, min.z),
            vec3(min.x, min.y, max.z),

            vec3(min.x, min.y, max.z),
            vec3(min.x, max.y, max.z),

            vec3(min.x, max.y, max.z),
            vec3(max.x, max.y, max.z),

            vec3(max.x, max.y, max.z),
            vec3(max.x, max.y, min.z),

            vec3(max.x, max.y, min.z),
            vec3(max.x, min.y, min.z),

            vec3(max.x, min.y, min.z),
            vec3(max.x, min.y, max.z)
        };

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        mat4 translate = glm::translate(mat4(1.0f), pos);
        mat4 rotate = mat4(rot);
        mat4 scale = glm::scale(mat4(1.0f), vec3(1.0f));
        mat4 model = translate * rotate * scale;

        m_shaders[2].use();
        m_shaders[2].setMat4("view", camera->viewMatrix);
        m_shaders[2].setMat4("projection", camera->projectionMatrix);
        m_shaders[2].setMat4("model", model);
        m_shaders[2].setVec3("color", vec3(0.6f, 0.7f, 0.9f));
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
        glBindVertexArray(0);
    });
}

void RenderSystem::endFrame()
{
}