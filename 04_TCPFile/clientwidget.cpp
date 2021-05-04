#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFileDialog>

#define cout qDebug() << "[" << __FILE__ << "]" << __LINE__ << "]"


ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket(this);
    setWindowTitle("Client");

    isStart = true;

    connect(tcpSocket, &QTcpSocket::readyRead,
            [=]()
    {
        QByteArray buf = tcpSocket->readAll();
        //取出接受的内容
        if(isStart == true){
            isStart = false;
            //解析头部信息
//            QString str = "hello##1024"
//                    str.section("##", 0, 0)
            fileName = QString(buf).section("##", 0, 0);
            fileSize = QString(buf).section("##", 1, 1).toInt();
            receiveSize = 0;

            //初始化
            //打开文件
            file.setFileName(fileName);
            bool isok = file.open(QIODevice::WriteOnly);
            if(isok == false){
                cout << "WriteOnly error 40";
            }
        }else {
            qint64 len = file.write(buf);
            receiveSize += len;
            if(receiveSize == fileSize){
                isStart = true;
                file.close();
                QMessageBox::information(this, "完成", "文件接收完成");

//                tcpSocket->disconnectFromHost();
//                tcpSocket->close();


            }

        }


    }
            );

}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_pushButtonConnect_clicked()
{
    //获取服务器的ip和port
    QString ip = ui->lineEditIP->text();
    quint64 port = ui->lineEditPort->text().toInt();

    tcpSocket->connectToHost(QHostAddress(ip), port);

}
