#ifndef ALGINE_TRANSLATABLE_H
#define ALGINE_TRANSLATABLE_H

#include <algine/core/transfer/Transferable.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace algine {
class Translatable: public Transferable {
public:
    Translatable();

    void setPos(float x, float y, float z);
    void setPos(const glm::vec3 &pos);
    void setX(float x);
    void setY(float y);
    void setZ(float z);

    void changePos(const glm::vec3 &dPos);

    virtual void translate();

    float getX() const;
    float getY() const;
    float getZ() const;
    const glm::vec3& getPos() const;
    const glm::mat4& getTranslationMatrix() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    glm::vec3 m_pos;
    glm::mat4 m_translation;
};
}

#endif //ALGINE_TRANSLATABLE_H
