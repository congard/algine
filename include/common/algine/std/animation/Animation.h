#ifndef ALGINE_ANIMATION_H
#define ALGINE_ANIMATION_H

#include <algine/std/animation/AnimNode.h>

#include <string>
#include <vector>

class aiAnimation;

namespace algine {
class Animation {
public:
    explicit Animation(const aiAnimation *anim);

    inline double getTicksPerSecond64() const;
    inline double getDuration64() const;
    inline float getTicksPerSecond() const;
    inline float getDuration() const;
    inline const std::string& getName() const;
    inline const std::vector<AnimNode>& getChannels() const;

private:
    double m_ticksPerSecond;
    double m_duration;
    std::string m_name;
    std::vector<AnimNode> m_channels;
};

double Animation::getTicksPerSecond64() const {
    return m_ticksPerSecond;
}

double Animation::getDuration64() const {
    return m_duration;
}

float Animation::getTicksPerSecond() const {
    return static_cast<float>(m_ticksPerSecond);
}

float Animation::getDuration() const {
    return static_cast<float>(m_duration);
}

const std::string& Animation::getName() const {
    return m_name;
}

const std::vector<AnimNode>& Animation::getChannels() const {
    return m_channels;
}
}

#endif //ALGINE_ANIMATION_H
