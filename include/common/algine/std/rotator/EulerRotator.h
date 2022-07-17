#ifndef ALGINE_EULERROTATOR_H
#define ALGINE_EULERROTATOR_H

#include <algine/std/rotator/Rotator.h>

namespace algine {
class EulerRotator: public Rotator {
public:
    EulerRotator();

    void rotate(glm::mat4 &matrix) override;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);
};
}

#endif //ALGINE_EULERROTATOR_H
