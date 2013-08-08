#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QGLWidget>

class VideoWidget : public QGLWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget *parent);
    ~VideoWidget();

    void getUserFrame(int userID, int framesCount);

private:
    QImage image;
    GLuint texture;

    bool useFilter;

    int currentUserID;
    void drawQuad();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

signals:
    void debug(QString messageText);
};

#endif // VIDEOWIDGET_H
