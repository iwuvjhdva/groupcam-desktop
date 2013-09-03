#ifndef USERWIDGET_H
#define USERWIDGET_H

class UserWidget
{
public:
    int id;
    QString name;
    QImage image;
    GLuint texture;
    QPoint point;

public:
    UserWidget(int id);

    bool update();
};

#endif // USERWIDGET_H
