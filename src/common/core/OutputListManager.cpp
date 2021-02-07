#include <algine/core/OutputListManager.h>

#include <algine/core/Framebuffer.h>
#include <algine/core/JsonHelper.h>

#include "FramebufferConfigTools.h"

using namespace nlohmann;

namespace algine {
OutputListManager::OutputListManager(const Data &data) {
    set(data);
}

OutputListManager::OutputListManager() = default;

void OutputListManager::set(const Data &data) {
    m_data = data;
}

void OutputListManager::add(Index index, Attachment attachment) {
    m_data[index] = attachment;
}

void OutputListManager::addColor(Index index, Index attachmentIndex) {
    add(index, Framebuffer::ColorAttachmentZero + attachmentIndex);
}

void OutputListManager::addColor(Index index) {
    add(index, Framebuffer::ColorAttachmentZero + index);
}

OutputList OutputListManager::get() const {
    OutputList outputList;

    for (const auto & item : m_data)
        outputList.add(item.first, item.second);

    return outputList;
}

const OutputListManager::Data& OutputListManager::data() const {
    return m_data;
}

void OutputListManager::import(const JsonHelper &jsonHelper) {
    const json &config = jsonHelper.json;

    for (const auto & p : config.items()) {
        char *endPtr;
        Index index = strtol(p.key().c_str(), &endPtr, 10);

        if (*endPtr != '\0')
            throw runtime_error("Broken output index: '" + p.key() + "'");

        m_data[index] = Config::stringToAttachment(p.value());
    }
}

JsonHelper OutputListManager::dump() {
    json result;

    for (const auto & p : m_data)
        result[to_string(p.first)] = Config::attachmentToString(p.second);

    return result;
}
}
