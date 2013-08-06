#include "mainwindow.h"
#include "ui_mainwindow.h"


extern TTInstance* ttInst;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    videoWidget = new VideoWidget(this);
    ui->insertWidget(0, videoWidget);

    if(ttInst != NULL)
        qDebug() << "Video instance initialized successfully";
    else
        qCritical() << "Video instance initialization failed";

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Bnei Baruch", "groupcam", 0);
    settings->setIniCodec("UTF-8");

    connect(ttInst);

    timers[ttInst].insert(startTimer(1000), TIMER_ONE_SECOND);
    timers[ttInst].insert(startTimer(50), TIMER_UI_UPDATE);
}

void MainWindow::killLocalTimer(TimerEvent e)
{
    timers_t::iterator ite = timers[ttInst].begin();
    while(ite != timers[ttInst].end())
    {
        if(*ite == e)
        {
            killTimer(ite.key());
            timers[ttInst].erase(ite);
            break;
        }
        ite++;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QMainWindow::timerEvent(event);

    switch(*ite)
    {
        case TIMER_ONE_SECOND:
            break;
        case TIMER_UI_UPDATE:
            {
                TTMessage message;
                int msecs = 0;
                while(TT_GetMessage(ttInst, &message, &msecs))
                    processTTMessage(ttInst, message);
            }
            break;
        case TIMER_RECONNECT:
            Q_ASSERT( (TT_GetFlags(ttInst) & CLIENT_CONNECTED) == 0);
            disconnect();
            connect();
            break;
    }
}

void MainWindow::connect():
{
    if(TT_GetFlags(ttInst) & CLIENT_CONNECTION)
        return;

    qDebug() << "Connecting to server...";

    if(!settings->allKeys().count())
    {
        qCritical() << "No settings available, aborting";
        return;
    }

    TT_Connect(ttInst,
              _W(settings->value(instType+"_server/host").toString()),
              settings->value(instType+"_server/tcp_port").toInt(),
              settings->value(instType+"_server/udp_port").toInt(),
              0, 0
          );
}

MainWindow::~MainWindow()
{
    delete videoWidget;
    delete ui;
    delete settings;
}
