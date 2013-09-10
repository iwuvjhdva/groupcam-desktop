#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"


extern TTInstance* ttInst;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Bnei Baruch", "groupcam", 0);
    settings->setIniCodec("UTF-8");

    videoWidget = new VideoWidget(this, settings);
    this->centralWidget()->layout()->addWidget(videoWidget);

    if(ttInst != NULL)
        qDebug() << "Video instance initialized successfully";
    else
        qCritical() << "Video instance initialization failed";

    connectServer();

    timers.insert(startTimer(1000), TIMER_ONE_SECOND);
    timers.insert(startTimer(50), TIMER_UI_UPDATE);

    this->setFixedSize(settings->value("window/width", 640).toInt(),
                       settings->value("window/height", 480).toInt());
    this->centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::killLocalTimer(TimerEvent e)
{
    timers_t::iterator ite = timers.begin();
    while(ite != timers.end())
    {
        if(*ite == e)
        {
            killTimer(ite.key());
            timers.erase(ite);
            break;
        }
        ite++;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QMainWindow::timerEvent(event);

    timers_t::iterator ite = timers.find(event->timerId());

    switch(*ite)
    {
        case TIMER_ONE_SECOND:
            break;
        case TIMER_UI_UPDATE:
            {
                TTMessage message;
                int msecs = 0;
                while(TT_GetMessage(ttInst, &message, &msecs))
                    processTTMessage(message);
            }
            break;
        case TIMER_RECONNECT:
            Q_ASSERT( (TT_GetFlags(ttInst) & CLIENT_CONNECTED) == 0);
            disconnectServer();
            connectServer();
            break;
    }
}

void MainWindow::connectServer() {
    if(TT_GetFlags(ttInst) & CLIENT_CONNECTION)
        return;

    qDebug() << "Connecting to server...";

    if(!settings->allKeys().count())
    {
        qCritical() << "No settings available, aborting";
        return;
    }

    TT_Connect(ttInst,
            _W(settings->value("server/host").toString()),
            settings->value("server/tcp_port").toInt(),
            settings->value("server/udp_port").toInt(),
            0, 0
            );
}

void MainWindow::commandProcessing(int commandID, bool complete)
{
    cmdreply_t::iterator ite;

    if(complete && (ite = commands.find(commandID)) != commands.end())
    {
        switch(*ite)
        {
            case CMD_COMPLETE_LOGIN:
                {
                    QString path = settings->value("server/channel_path").toString();
                    int channelID = TT_GetChannelIDFromPath(ttInst, _W(path));

                    if(channelID)
                    {
                        int commandID = TT_DoJoinChannelByID(ttInst, channelID, _W(settings->value("server/channel_password").toString()));

                        if(commandID>0)
                            commands.insert(commandID, CMD_COMPLETE_JOINCHANNEL);

                        qDebug() << QString("Joining the channel %1...").arg(path);
                    } else {
                        qDebug() << "Invalid channel path";
                    }
                }
                break;
            case CMD_COMPLETE_JOINCHANNEL:
                qDebug() <<"Joined the channel";
                break;
        }
    }
}

void MainWindow::processTTMessage(const TTMessage& msg)
{
    switch(msg.wmMsg)
    {
        case WM_TEAMTALK_CON_SUCCESS:
            {
                // disable reconnect timer
                killLocalTimer(TIMER_RECONNECT);

                int commandID = TT_DoLogin(ttInst,
                        _W(settings->value("server/nickname").toString()),
                        _W(settings->value("server/server_password").toString()),
                        _W(settings->value("server/user_name").toString()),
                        _W(settings->value("server/password").toString())
                        );
                if(commandID>0)
                    commands.insert(commandID, CMD_COMPLETE_LOGIN);

                qDebug() << "Connected to server";
            }
            break;
        case WM_TEAMTALK_CON_FAILED:
            disconnectServer();
            qDebug() << "Failed to connect to server";
            break;
        case WM_TEAMTALK_CON_LOST:
            disconnectServer();
            timers[startTimer(5000)] = TIMER_RECONNECT;
            qDebug() << "Connection to server lost, reconnecting...";
            break;
        case WM_TEAMTALK_CMD_MYSELF_LOGGEDIN:
            qDebug() << "Logged in to server";
            break;
        case WM_TEAMTALK_CMD_MYSELF_LOGGEDOUT:
            qDebug() << "Logged out from server";
            disconnectServer();
            break;
        case WM_TEAMTALK_USER_VIDEOFRAME:
            videoWidget->getUserFrame(msg.wParam, msg.lParam);
            break;
        case WM_TEAMTALK_CMD_PROCESSING:
            commandProcessing(msg.wParam, msg.lParam);
            break;
        case WM_TEAMTALK_CMD_ERROR:
            qDebug() << QString("Error performing the command (error code %1").arg(msg.wParam);
            disconnectServer();
            break;
        case WM_TEAMTALK_CMD_USER_LOGGEDOUT:
        case WM_TEAMTALK_CMD_USER_LEFT:
            this->videoWidget->removeUser(msg.wParam);
            break;
    }
}

void MainWindow::disconnectServer() {
    TT_Disconnect(ttInst);
}


MainWindow::~MainWindow() {
    delete videoWidget;
    delete layout;
    delete ui;
    delete settings;
}
