#ifndef ALGINE_IMAGEWIDGET_H
#define ALGINE_IMAGEWIDGET_H

#include <algine/core/widgets/Widget.h>
#include <tulz/static_initializer.h>

namespace algine {
class AL_EXPORT ImageWidget: public Widget {
    STATIC_INITIALIZER_DECL

public:
    explicit ImageWidget(Object *parent = GlobalScene::getInstance());

    void setImage(Texture2D *image, bool takeOwnership = true);
    Texture2D* getImage() const;

    void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) override;

protected:
    void onMeasure(int &width, int &height) override;
    void onDraw(Painter &painter) override;

protected:
    Texture2D *m_image;
};
}

#endif //ALGINE_IMAGEWIDGET_H
