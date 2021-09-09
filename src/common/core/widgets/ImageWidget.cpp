#include <algine/core/widgets/ImageWidget.h>
#include <algine/core/widgets/Units.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/PtrMaker.h>

#include <pugixml.hpp>

#include <cstring>

#include "TexturePathLoader.h"

namespace algine {
void ImageWidget::setImage(const Texture2DPtr &image) {
    requestLayout();
    invalidate();
    m_image = image;
}

const Texture2DPtr& ImageWidget::getImage() const {
    return m_image;
}

void ImageWidget::onMeasure(int &width, int &height) {
    Widget::onMeasure(width, height);

    if (m_verticalPolicy != SizePolicy::Preferred && m_horizontalPolicy != SizePolicy::Preferred)
        return;

    m_image->bind();

    if (m_verticalPolicy == SizePolicy::Preferred) {
        height = static_cast<int>(m_image->getActualHeight());
    }

    if (m_horizontalPolicy == SizePolicy::Preferred) {
        width = static_cast<int>(m_image->getActualWidth());
    }
}

void ImageWidget::onDraw(Painter &painter) {
    painter.drawTexture(m_image, {
        0, 0,
        static_cast<float>(getContentWidth()),
        static_cast<float>(getContentHeight())
    });
}

// Explicit attributes: imageFiltering

void ImageWidget::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Widget::fromXML(node, io);

    bool isImageUnique = false;
    uint imageFiltering = 0;

    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        auto attr_str = attr.as_string();

        if (isAttr("image")) {
            setImage(Texture2D::getByName(getString(attr_str)));
        } else if (isAttr("imageSrc")) {
            auto data = TexturePathLoader::load(getString(attr_str), io);
            isImageUnique = data.unique;
            setImage(data.texture);
        } else if (isAttr("imageWidth")) {
            setWidth(getDimenPx(attr_str) + getPaddingLeft() + getPaddingRight());
        } else if (isAttr("imageHeight")) {
            setHeight(getDimenPx(attr_str) + getPaddingTop() + getPaddingBottom());
        } else if (isAttr("imageFiltering")) {
            imageFiltering = static_cast<uint>(parseFiltering(attr_str));
        }
    }

    if (isImageUnique && imageFiltering != 0) {
        m_image->bind();
        m_image->setParams(std::map<uint, uint> {
            {Texture::MinFilter, imageFiltering},
            {Texture::MagFilter, imageFiltering}
        });
    }
}
}
