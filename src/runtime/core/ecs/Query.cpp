#include <core/ecs/Query.hpp>

#include <core/Core.hpp>

using namespace jaccen;

jaccen::Query<> jaccen::query(const Database& database)
{
    return Query<>(database);
}
