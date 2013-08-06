#include <QPainter>
#include <QLayout>

#include "videowidget.h"
#include "common.h"

#include <QFile>

extern TTInstance* ttVideoInst;

VideoWidget::VideoWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    currentUserID = 0;
}

VideoWidget::~VideoWidget()
{
    glDeleteTextures(1, &texture);

    delete shader;
    delete program;
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

    GLint param = useFilter ? GL_NEAREST : GL_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
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
        GLenum target = useFilter ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;

        glTexImage2D(target, 0, 4, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
        glBindTexture(target, texture);

        if(useFilter)
        {
            int textureLocation = program->attributeLocation("src");
            program->setUniformValue(textureLocation, 0);
            program->bind();
        }

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
    if(currentUserID && currentUserID!=userID)
        return;
    else
        currentUserID = userID;

    Q_UNUSED(framesCount);

    CaptureFormat format;

    if(!TT_GetVideoFrame(ttVideoInst, userID, NULL, 0, &format))
        return;

    if(image.width() != format.nWidth || image.height() != format.nHeight)
        image = QImage(format.nWidth, format.nHeight, QImage::Format_RGB32);

    if(TT_GetVideoFrame(ttVideoInst, userID, image.bits(), image.numBytes(), &format))
        update();
}
