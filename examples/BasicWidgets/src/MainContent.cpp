#include "MainContent.h"
#include "ui/MainScene.h"

#include <algine/core/Framebuffer.h>
#include <algine/core/Window.h>

MainContent::MainContent(Object *parent)
    : Content(parent),
      m_scene(new UI::MainScene(this))
{
    m_scene->listen(this);
}

void MainContent::onShow() {
    getWindow()->setEventHandler(this);
    m_scene->setSize(width(), height());
    m_scene->show();
}

void MainContent::render() {
    Framebuffer::getDefault()->bind();
    Framebuffer::getDefault()->clear(Framebuffer::ColorBuffer | Framebuffer::DepthBuffer);
    Engine::setViewport(width(), height());
    m_scene->render();
}
