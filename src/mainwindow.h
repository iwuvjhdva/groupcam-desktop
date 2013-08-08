#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QSettings>

#include "common.h"

#include "videowidget.h"


enum TimerEvent
{
    TIMER_ONE_SECOND,
    TIMER_UI_UPDATE,
    TIMER_RECONNECT,
};

enum CommandComplete
{
    CMD_COMPLETE_LOGIN,
    CMD_COMPLETE_JOINCHANNEL,
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void killLocalTimer(TimerEvent e);
    void connect();
    void commandProcessing(int commandID, bool complete);
    void processTTMessage(const TTMessage& msg);
    void disconnect();
    
private:
    Ui::MainWindow *ui;
    QGridLayout *layout;

    QSettings *settings;

    typedef QMap<int, TimerEvent> timers_t;
    timers_t timers;

    VideoWidget *videoWidget;

    void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
