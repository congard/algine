#include <algine/core/widgets/ImageWidget.h>
#include <algine/core/widgets/Units.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/TypeRegistry.h>

#include <pugixml.hpp>

#include <cstring>

#include "TexturePathLoader.h"

namespace algine {
STATIC_INITIALIZER_IMPL(ImageWidget) {
    alRegisterType(ImageWidget);
}

void ImageWidget::setImage(Texture2D *image, bool takeOwnership) {
    requestLayout();
    invalidate();
    m_image = image;
}

Texture2D* ImageWidget::getImage() const {
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

    uint imageFiltering = 0;

    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        auto attr_str = attr.as_string();

        if (isAttr("image")) {
            setImage(findChild<Texture2D*>(getString(attr_str), Object::FindOption::DirectThisAndScene));
        } else if (isAttr("imageSrc")) {
            auto texture = TexturePathLoader::load(getString(attr_str), io, this);
            setImage(texture);
        } else if (isAttr("imageWidth")) {
            setWidth(getDimenPx(attr_str) + getPaddingLeft() + getPaddingRight());
        } else if (isAttr("imageHeight")) {
            setHeight(getDimenPx(attr_str) + getPaddingTop() + getPaddingBottom());
        } else if (isAttr("imageFiltering")) {
            imageFiltering = static_cast<uint>(parseFiltering(attr_str));
        }
    }

    if (m_image != nullptr && m_image->getParent() == this && imageFiltering != 0) {
        m_image->bind();
        m_image->setParams(std::map<uint, uint> {
            {Texture::MinFilter, imageFiltering},
            {Texture::MagFilter, imageFiltering}
        });
    }
}
}
