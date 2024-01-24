#pragma once

#include <core/ecs/System.hpp>

#include <scene/CameraController.hpp>
#include <scene/components/Camera.hpp>
#include <scene/components/CameraControl.hpp>

#include <ui/Window.hpp>

namespace jaccen
{
    class CameraSystem : public System
    {
    public:
        ADD_COMPONENT_FUNCTION(Camera, m_cameraTable);
        ADD_COMPONENT_FUNCTION(CameraControl, m_cameraControlTable);

    public:
        CameraSystem(Database& db);
        ~CameraSystem() override;

        void registerWindow(Window& window);

        void update(const Scene& scene) override;
        
        // QUERY:  'updateCameraController'
        // READS:  CameraControl
        // WRITES: (Updated)
                       
        // QUERY:  'updateCameraFov'
        // READS:  (Updated), Camera, CameraControl
        // WRITES: Camera

        // QUERY:  'updateCameraMatrices'
        // READS:  (Updated), Transform
        // WRITES: Camera

    private:
        TableRef<Camera> m_cameraTable;
        TableRef<CameraControl> m_cameraControlTable;

        std::shared_ptr<CameraController> m_cameraController;
    };
}