#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

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

    QSettings *settings;

    typedef QMap<int, TimerEvent> timers_t;
    typedef QMap<TTInstance*, timers_t> instance_timers_t;
    instance_timers_t timers;

    VideoWidget *videoWidget;
};

#endif // MAINWINDOW_H
