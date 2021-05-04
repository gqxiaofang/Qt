#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTcpServer>   //监听套接字
#include <QTcpSocket>   //通信套接字
#include <QFile>
#include <QTimer>


namespace Ui {
class ServerWidget;
}

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ServerWidget(QWidget *parent = 0);
    ~ServerWidget();

    void sendData();

private slots:
    void on_pushButtonFile_clicked();

    void on_pushButtonSend_clicked();

private:
    Ui::ServerWidget *ui;

    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    QFile file; //文件对象
    QString fileName; //文件名字
    qint64 fileSize;    //文件大小
    qint64 sendSize;

    QTimer timer;   //定时器

};

#endif // SERVERWIDGET_H
