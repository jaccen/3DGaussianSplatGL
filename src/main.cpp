#include "MyApplication.hpp"
#include "vera/ops/string.h"
#include "vera/ops/meshes.h"

using namespace std;
using namespace vera;
using namespace glm;

int main(int argc, char **argv) {
    WindowProperties prop;
    MyApplication app;
    // prop.style = LENTICULAR;
    // setQuiltProperties(2);
    app.run(prop);
    return 1;
}
