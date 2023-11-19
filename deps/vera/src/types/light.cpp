#include "vera/types/light.h"

#include <iostream>

namespace vera {

Light::Light(): 
    color(1.0), 
    direction(0.0),
    intensity(1.0),
    falloff(-1.0),
    m_mvp_biased(1.0), 
    m_mvp(1.0), 
    m_lightType(LIGHT_DIRECTIONAL) {
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _dir): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0; 
    m_lightType = LIGHT_DIRECTIONAL;
    direction = normalize(_dir);
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _pos, float _falloff): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0;
    m_lightType = LIGHT_POINT;
    setPosition(_pos);
    falloff = _falloff;
    std::fill(m_viewport, m_viewport+4, 0);
}

Light::Light(glm::vec3 _pos, glm::vec3 _dir, float _falloff): m_mvp_biased(1.0), m_mvp(1.0) {
    color = glm::vec3(1.0); 
    intensity = 1.0;
    falloff = -1.0;
    m_lightType = LIGHT_SPOT;
    direction = _dir;
    setPosition(_pos);
    falloff = _falloff;
}

Light::~Light() {
}

const glm::mat4& Light::getMVPMatrix( const glm::mat4 &_model, float _area) {
    // TODO:
    //      - Extend this to match different light types and not just directional

    if (bChange) {
        // From http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping
        _area *= 2.0;
        m_near = -_area * 2.0;
        m_far = _area * 4.0;

        // Compute the MVP matrix from the light's point of view
        m_projectionMatrix = glm::ortho<float>(-_area, _area, -_area, _area, m_near, m_far);
        m_viewMatrix = glm::lookAt(glm::normalize(getPosition()), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        m_mvp = m_projectionMatrix * m_viewMatrix * glm::mat4(1.0f);// * _model;

        // Make biased MVP matrix (0 ~ 1) instad of (-1 to 1)
        const glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );

        m_mvp_biased = biasMatrix * m_mvp;
        bChange = false;
    }

    return m_mvp;
}

void Light::bindShadowMap() {
    glGetIntegerv(GL_VIEWPORT, m_viewport);

    if (m_shadowMap.getDepthTextureId() == 0)
    #if defined(PLATFORM_RPI)
        m_shadowMap.allocate(512, 512, DEPTH_TEXTURE);
    #elif defined(PLATFORM_OSX)
        m_shadowMap.allocate(2048, 2048, COLOR_DEPTH_TEXTURES);
    #else
        m_shadowMap.allocate(2048, 2048, DEPTH_TEXTURE);
    #endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    m_shadowMap.bind();

    glClear(GL_DEPTH_BUFFER_BIT);
}

void Light::unbindShadowMap() {
    m_shadowMap.unbind();
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
}

}
