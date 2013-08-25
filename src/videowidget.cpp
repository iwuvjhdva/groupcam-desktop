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
    currentUserID = 0;
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

    // Creating texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint param = GL_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
}

void VideoWidget::drawQuad()
{
    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2f(0, 0);

    glTexCoord2i(image.width()-1, 0);
    glVertex2f(width()-1, 0);

    glTexCoord2i(image.width()-1, image.height()-1);
    glVertex2f(width()-1, height()-1);

    glTexCoord2i(0, image.height()-1);
    glVertex2f(0, height()-1);

    glEnd();
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(image.isNull())
    {
    } else
    {
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);

        drawQuad();
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
    if(currentUserID && currentUserID != userID)
        return;
    else
        currentUserID = userID;

    Q_UNUSED(framesCount);

    CaptureFormat format;

    if(!TT_GetUserVideoFrame(ttInst, userID, NULL, 0, &format))
        return;

    if(image.width() != format.nWidth || image.height() != format.nHeight)
        image = QImage(format.nWidth, format.nHeight, QImage::Format_RGB32);

    if(TT_GetUserVideoFrame(ttInst, userID, image.bits(), image.numBytes(), &format))
        update();
}