#include <glm/glm.hpp>
#include <vector>
class   BoundingBox {
public:
    glm::vec3 min, max;

    BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

};

class SplatTreeNode
{
private:
    /* data */
public:
    SplatTreeNode(const glm::vec3& min, const glm::vec3& max, int depth, int id) 
        : min(min), max(max), boundingBox(min, max), depth(depth), data(nullptr), id(id) 
    {
        center = center.copy(max).sub(min).multiplyScalar(0.5).add(min);
    }
    virtual ~SplatTreeNode(){};
    
    void setIndexes(const std::vector<int>& newIndexes) {
        indexes = newIndexes;
    }
  private:      
    glm::vec3 min, max, center;
    BoundingBox boundingBox;
    int depth;
    std::vector<SplatTreeNode> children;
    void* data = nullptr;
    int id = 0;
};

