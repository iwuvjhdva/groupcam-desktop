#include <QDebug>

#include <QPainter>
#include <QLayout>

#include "videowidget.h"
#include "common.h"

#include <QFile>

extern TTInstance* ttInst;

VideoWidget::VideoWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

VideoWidget::~VideoWidget()
{
    glDeleteTextures(1, &texture);
}

void VideoWidget::initializeGL()
{
    glClearColor(0, 0, 0, 0);

    glEnable(GL_TEXTURE_2D);

    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width(), height(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Initializing textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
}

void VideoWidget::drawQuad()
{
    glVertex2f(0, 0);
    glVertex2f(width()-1, 0);
    glVertex2f(width()-1, height()-1);
    glVertex2f(0, height()-1);
}

void VideoWidget::drawTitle()
{
    glTexCoord2i(0, 0);
    glTexCoord2i(image.width()-1, 0);
    glTexCoord2i(image.width()-1, image.height()-1);
    glTexCoord2i(0, image.height()-1);
}

void VideoWidget::updateUsers()
{

}

void VideoWidget::drawUsers()
{
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, image.width(), image.height(),
                 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);

    glTexCoord2i(0, 0);
    glTexCoord2i(image.width()-1, 0);
    glTexCoord2i(image.width()-1, image.height()-1);
    glTexCoord2i(0, image.height()-1);
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(!image.isNull())
    {
        glBegin(GL_QUADS);

        drawTitle();
        drawUsers();
        drawQuad();

        glEnd();
    }
}

void VideoWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void VideoWidget::getUserFrame(int userID, int framesCount)
{
    Q_UNUSED(framesCount);

    UserWidget *userWidget;

    if (this->userWidgets.contains(userID))
        userWidget = &userWidgets[userID];
    else {
        userWidget = new UserWidget(userID);
        this->userWidgets.insert(userID, userWidget);
    }

    if (userWidget->update())
        this->update();
}
