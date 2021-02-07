#include <algine/core/OutputList.h>

#include <algine/core/Framebuffer.h>

#include <stdexcept>

using namespace std;

namespace algine {
OutputList::OutputList()
    : m_list({Framebuffer::ColorAttachmentZero})
{
    // see initializer list above
}

void OutputList::set(const vector<Attachment> &list) {
    m_list = list;
}

void OutputList::add(Index index, Attachment attachment) {
    if (index == m_list.size()) {
        m_list.emplace_back(attachment);
    } else if (index > m_list.size()) {
        vector<uint> tmp(index + 1, Framebuffer::EmptyAttachment);

        for (uint i = 0; i < m_list.size(); ++i)
            tmp[i] = m_list[i];

        tmp[index] = attachment;
        m_list = tmp;
    } else {
        m_list[index] = attachment;
    }
}

void OutputList::addColor(Index index, Index attachmentIndex) {
    add(index, Framebuffer::ColorAttachmentZero + attachmentIndex);
}

void OutputList::addColor(Index index) {
    add(index, Framebuffer::ColorAttachmentZero + index);
}

void OutputList::remove(Index index, bool optimizeList) {
    if (index < m_list.size()) {
        m_list[index] = Framebuffer::EmptyAttachment;
    } else {
        throw runtime_error(
            "Incorrect index value."
            "\nList size: " + to_string(m_list.size()) +
            "\nGiven index: " + to_string(index)
        );
    }

    if (optimizeList && index == m_list.size() - 1) {
        optimize();
    }
}

void OutputList::optimize() {
    for (int i = static_cast<int>(m_list.size()) - 1; i >= 0; --i) {
        if (m_list[i] != Framebuffer::EmptyAttachment) {
            m_list.erase(m_list.begin() + i + 1, m_list.end());
            return;
        }
    }

    m_list.clear();
}

size_t OutputList::size() const {
    return m_list.size();
}

const vector<Attachment>& OutputList::get() const {
    return m_list;
}

const Attachment* OutputList::data() const {
    return m_list.data();
}
}
