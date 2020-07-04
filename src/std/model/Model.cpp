#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/Model.h>

namespace algine {
Model::Model(const uint rotatorType): Rotatable(rotatorType) {
    /* empty */
}

void Model::updateMatrix() {
    m_transform = m_translation * m_rotation * m_scaling;
}
}
