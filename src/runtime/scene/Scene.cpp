#include <scene/Scene.hpp>

#include <editor/Components.hpp>
#include <render/components/Mesh.hpp>
#include <scene/components/Transform.hpp>

using namespace jaccen;

Scene::Scene() :
    m_transformSystem(m_database),
    m_renderSystem(m_database),
    m_cameraSystem(m_database),
    m_editorSystem(m_database)
{
    registerSystem(m_transformSystem);
    registerSystem(m_renderSystem);
    registerSystem(m_cameraSystem);
    registerSystem(m_editorSystem);

    createCamera();
    createGizmo();
}

Scene::~Scene()
{
}

void Scene::registerSystem(ISystem& system)
{
    m_systems.emplace_back(&system);

    system.registerDatabase(m_database);
}

// TODO: Integrate input to ECS
void Scene::update(const FrameInput& input)
{
    // TODO: Thread-safe and lock-free entity creation and deletion within multithreaded
    //       system updates
    // - Deletion is already thread-safe in a way, because each system marks deleted 
    //   entities locally and the changes are committed to the scene database on each
    //   logic thread update cycle
    // - Creation is inherently not thread-safe at the moment, since creating an entity
    //   might require mutable access to various systems
    //
    // How would a e.g. WeaponSystem instantiate a Bullet entity?
    //
    // A) Divide entity creation into multiple frames, and into multiple systems
    //   1) WeaponSystem initiates entity creation, by either:
    //      a) directly creating a new id, if the database id counter is atomic
    //      b) delegate id and component creation to a sync point, by allowing each system
    //         to keep local copies of the necessary tables in the database
    //   2) assigns the Bullet component to the new entity (along with any other relevant
    //      components owned by the system) and also the (local) Added component
    //   3) other systems react to the new entity and add their own components in the
    //      following frame(s) e.g. TransformSystem notices the entity with the Added
    //      and Bullet components and assigns a Transform component to it
    //   
    //   * Should be easy to parallize because the systems are controlling access to their
    //     own data
    //   * Would spread out entity creation to multiple systems and make composition harder
    //     to understand
    //   * An entity would be in this "building process" phase for at least 1-2 frames;
    //     there's the possibility that the entity is picked by a query whilst still being
    //     built, one which might filter it out in its final form e.g. moving an object
    //     which should have a collider into a wall
    //   * If entity creation takes multiple frames, we might not want to render or activate
    //     it until the process has completed; this would prevent the "premature query" issue;
    //     how would these "dormant" entities be tagged and activated? 
    //   * Anyway, this adds a 1-2 frame delay to entity spawning and activation; how would
    //     this affect input and/or visually sensitive contexts e.g. firing a weapon, an
    //     explosion, etc?
    //
    // B) add a synchronous "create" step for systems where they have mutable access to each
    //    other (and the database), and which is executed before the parallel update step
    //
    // C) change database entity id counter to atomic and allow ids to be created within
    //    concurrent system updates, but dispatch the component assignment (and mutable 
    //    system access) to the start of next frame

    // TODO: unit test Updated, Deleted
    for (auto system : m_systems)
    {
        system->commitUpdated(m_database);
        system->commitDeleted(m_database);
    }

    m_editorSystem.processInput(input);
    
    // TODO: Scheduler (currently update order matters)
    m_cameraSystem.update(*this);
    m_transformSystem.update(*this);
    m_renderSystem.update(*this);
    
    // TODO: multithreading, within system update or between updates?
    // causality scheduling (const, non-const) for individual queries?
    //for (auto system : m_systems)
    //{
    //    system->update(*this);
    //}

    m_database.purgeDeleted();
    m_database.clearTags();
}

void Scene::render()
{
    m_renderSystem.beginFrame();
    m_renderSystem.render();
    m_renderSystem.endFrame();
}

EntityId Scene::createEntity()
{
    return m_database.createEntity();
}

EntityId Scene::createCamera()
{
    auto id = m_database.createEntity();

    Transform transform;
    transform.rotation = glm::quatLookAt(
        vec3(0.0f, 0.0f, -1.0f), Camera::WorldUp);

    m_cameraSystem.addCamera(id, Camera());
    m_cameraSystem.addCameraControl(id, CameraControl());
    m_transformSystem.addTransform(id, std::move(transform));

    m_database.table<Added>().assign(id, Added());
    m_database.table<Updated>().assign(id, Updated());

    return id;
}

EntityId Scene::createCube(vec3 position)
{
    Mesh mesh;
    mesh.vertices = std::vector<vec3>
    {
        vec3(0.5f,   0.5f, -0.5f),
        vec3(0.5f,  -0.5f, -0.5f),
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f,  0.5f, -0.5f),

        vec3(0.5f,   0.5f,  0.5f),
        vec3(0.5f,  -0.5f,  0.5f),
        vec3(-0.5f, -0.5f,  0.5f),
        vec3(-0.5f,  0.5f,  0.5f)
    };
    mesh.colors = std::vector<vec3>
    {
        vec3(0.9f, 0.9f, 0.9f),
        vec3(0.9f, 0.9f, 0.9f),
        vec3(0.9f, 0.9f, 0.9f),
        vec3(0.9f, 0.9f, 0.9f),

        vec3(0.6f, 0.6f, 0.6f),
        vec3(0.6f, 0.6f, 0.6f),
        vec3(0.6f, 0.6f, 0.6f),
        vec3(0.6f, 0.6f, 0.6f)
    };
    mesh.indices = std::vector<unsigned>
    {
        0, 1, 3, // front
        1, 2, 3,

        4, 5, 7, // back
        5, 6, 7,

        4, 0, 7, // top
        0, 3, 7,

        5, 1, 6, // bottom
        1, 2, 6,

        3, 2, 7, // left
        2, 6, 7,

        4, 5, 0, // right
        5, 1, 0
    };

    auto id = createEntity();

    Transform transform;
    transform.position = position;

    m_renderSystem.addMesh(id, std::move(mesh));
    m_transformSystem.addTransform(id, std::move(transform));

    m_database.table<Added>().assign(id, Added());
    m_database.table<Updated>().assign(id, Updated());

    return id;
}

EntityId Scene::createGizmo()
{
    auto id = m_database.createEntity();

    m_transformSystem.addTransform(id, Transform());
    m_editorSystem.addTransformGizmo(id, TransformGizmo());

    m_database.table<Added>().assign(id, Added());
    m_database.table<Updated>().assign(id, Updated());

    return id;
}
