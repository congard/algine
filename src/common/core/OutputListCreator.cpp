#include <algine/core/OutputListCreator.h>

#include <algine/core/Framebuffer.h>
#include <algine/core/JsonHelper.h>

#include "FramebufferConfigTools.h"

using namespace nlohmann;

namespace algine {
OutputListCreator::OutputListCreator(const Data &data) {
    set(data);
}

OutputListCreator::OutputListCreator() = default;

void OutputListCreator::set(const Data &data) {
    m_data = data;
}

void OutputListCreator::add(Index index, Attachment attachment) {
    m_data[index] = attachment;
}

void OutputListCreator::addColor(Index index, Index attachmentIndex) {
    add(index, Framebuffer::ColorAttachmentZero + attachmentIndex);
}

void OutputListCreator::addColor(Index index) {
    add(index, Framebuffer::ColorAttachmentZero + index);
}

OutputList OutputListCreator::create() const {
    OutputList outputList;

    for (const auto & item : m_data)
        outputList.add(item.first, item.second);

    return outputList;
}

const OutputListCreator::Data& OutputListCreator::data() const {
    return m_data;
}

void OutputListCreator::import(const JsonHelper &jsonHelper) {
    const json &config = jsonHelper.json;

    for (const auto & p : config.items()) {
        char *endPtr;
        Index index = strtol(p.key().c_str(), &endPtr, 10);

        if (*endPtr != '\0')
            throw runtime_error("Broken output index: '" + p.key() + "'");

        m_data[index] = Config::stringToAttachment(p.value());
    }
}

JsonHelper OutputListCreator::dump() {
    json result;

    for (const auto & p : m_data)
        result[to_string(p.first)] = Config::attachmentToString(p.second);

    return result;
}
}
