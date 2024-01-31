
#pragma once

#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>


class Ply
{
public:
    bool CheckLine(std::ifstream& plyFile, const std::string& validLine);
    bool GetNextPlyLine(std::ifstream& plyFile, std::string& lineOut);
    bool Parse(std::ifstream& plyFile);

    enum class Type
    {
        Char,
        UChar,
        Short,
        UShort,
        Int,
        UInt,
        Float,
        Double
    };

    struct Property
    {
        size_t offset;
        size_t size;
        Type type;

        template <typename T>
        const T Get(const uint8_t* data) const { assert(size == sizeof(T)); return *reinterpret_cast<const T*>(data + offset); }
    };

    bool GetProperty(const std::string& key, Property& propertyOut) const;

    using VertexCallback = std::function<void(const uint8_t*, size_t)>;
    void ForEachVertex(const VertexCallback& cb);

    size_t GetVertexCount() const { return vertexCount; }

protected:
    std::unordered_map<std::string, Property> propertyMap;
    std::vector<uint8_t> dataVec;
    size_t vertexCount = 0;
    size_t vertexSize = 0;
};
