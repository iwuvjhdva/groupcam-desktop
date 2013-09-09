#include <QDebug>

#include <QPainter>
#include <QLayout>

#include "common.h"
#include "constants.h"
#include "videowidget.h"

#include <QFile>

extern TTInstance* ttInst;

VideoWidget::VideoWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
}

VideoWidget::~VideoWidget()
{    
    QHash <int, UserWidget*>::iterator iter;

    for (iter = this->userWidgets.begin(); iter != this->userWidgets.end(); iter++)
    {
        glDeleteTextures(1, &iter.value()->texture);
    }

    qDeleteAll(this->userWidgets.values());
}

void VideoWidget::initializeGL()
{
    glClearColor(255, 0, 0, 0);

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

void VideoWidget::drawQuad(QImage &image, GLuint texture, QRect &rect)
{
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, image.width(), image.height(),
                 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2f(rect.left(), rect.top());

    glTexCoord2i(image.width()-1, 0);
    glVertex2f(rect.right(), rect.top());

    glTexCoord2i(image.width()-1, image.height()-1);
    glVertex2f(rect.right(), rect.bottom());

    glTexCoord2i(0, image.height()-1);
    glVertex2f(rect.left(), rect.bottom());

    glEnd();
}

void VideoWidget::drawTitle()
{
    QImage titleImage = QImage(this->width(),
                          this->height() * TITLE_HEIGHT_PERCENTS / 100.0,
                          QImage::Format_RGB32);

}

void VideoWidget::updateUsers()
{
    int columnsNumber, rowsNumber;
    int column = 0, row = 0;

    // TODO: calc
    columnsNumber = 2;
    rowsNumber = 2;

    // TODO: sort
    QList <UserWidget*> widgetList = this->userWidgets.values();

    for(int index = 0; index < widgetList.size(); ++index)
    {
        UserWidget *widget = widgetList.at(index);
        int x = index % columnsNumber  * 320;
        int y = index / columnsNumber * 240 +
                this->height() * TITLE_HEIGHT_PERCENTS / 100.0;

        widget->rect = QRect(x, y, 320, 240);
    }
}

void VideoWidget::drawUsers()
{
    QHash <int, UserWidget *>::iterator iter;

    for (iter = this->userWidgets.begin(); iter != this->userWidgets.end(); ++iter)
    {
        UserWidget *widget = iter.value();
        this->drawQuad(widget->image, widget->texture, widget->rect);
    }
}

void VideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawTitle();
    updateUsers();
    drawUsers();
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
        userWidget = userWidgets[userID];
    else {
        userWidget = new UserWidget(userID);
        this->userWidgets.insert(userID, userWidget);
    }

    if (userWidget->update())
        this->update();
}
