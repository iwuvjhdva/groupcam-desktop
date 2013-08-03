#include <QtGui/QApplication>
#include "mainwindow.h"

TTInstance* ttVideoInst = NULL;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    ttVideoInst = TT_InitTeamTalkPoll();

    MainWindow w;
    w.show();

    int result = a.exec();
    TT_CloseTeamTalk(ttVideoInst);

    return result;
}
