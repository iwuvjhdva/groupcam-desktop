#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QGLWidget>

class UserWidget
{
public:
    int userID;
    QString name;
    QImage image;
    GLuint texture;
    QRect rect;

public:
    UserWidget(int id);

    bool update();
};

#endif // USERWIDGET_H
