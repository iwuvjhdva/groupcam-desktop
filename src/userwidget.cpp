#include "userwidget.h"

#include "common.h"

extern TTInstance* ttInst;

UserWidget::UserWidget(int id)
{
    this->userID = id;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
}

bool UserWidget::update()
{
    CaptureFormat format;

    if(!TT_GetUserVideoFrame(ttInst, userID, NULL, 0, &format))
        return false;

    if(image.width() != format.nWidth || image.height() != format.nHeight)
        image = QImage(format.nWidth, format.nHeight, QImage::Format_RGB32);

    return TT_GetUserVideoFrame(ttInst, userID, image.bits(), image.numBytes(), &format);
}
