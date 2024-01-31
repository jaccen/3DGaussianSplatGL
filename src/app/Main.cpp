#include <core/Core.hpp>
#include <scene/Scene.hpp>
#include <ui/Window.hpp>

#include <filesystem>
#include "gaussiancloud.h"
#include "splatrenderer.h"

;
std::shared_ptr<GaussianCloud> gaussianCloud;
std::shared_ptr<SplatRenderer> splatRenderer;

int  Init(const std::string& plyFilename) {
    //加载Splat数据化
    gaussianCloud = std::make_shared<GaussianCloud>();
    if (!gaussianCloud->ImportPly(plyFilename))
    {
        ECS_LOG_ERROR("Error loading GaussianCloud!\n");
        return -1;
    }
    splatRenderer = std::make_shared<SplatRenderer>();
    if (!splatRenderer->Init(gaussianCloud, true, true))
    {
        ECS_LOG_ERROR("Error initializing splat renderer!\n");
        return -1;
    }
    return 0;
}
int main(int /*argc*/, char** /*argv*/)
{
    std::cout << "Running in " << std::filesystem::current_path() << std::endl;

    auto window = std::make_shared<jaccen::Window>(1920, 1080, "Test");

    if (Init("data.ply"))
    {

    }
    
    auto scene = std::make_shared<jaccen::Scene>();
    scene->cameraSystem().registerWindow(*window);

    //加载3dgaussian
    //auto id = scene->createEntity();
   // jaccen::Transform transform;
   // transform.position = jaccen::vec3(0.0, 0.0, 5.0);

    scene->createCube(jaccen::vec3(0.0f, 0.0f, -3.0f));
    
    while (window->pollEvents())
    {
        window->beginFrame();

        scene->update(window->frameInput());
        scene->render();

        window->endFrame();
    }
    
    return 0;
}
