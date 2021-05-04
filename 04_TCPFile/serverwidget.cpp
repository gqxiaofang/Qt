#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QFileDialog>
#include <QDebug>
//#include <QHostAddress>
#include <QFileInfo>
#define cout qDebug() << "[" << __FILE__ << "]" << __LINE__ << "]"


ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    //监听套接字
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 8888);
    setWindowTitle("服务器端口为:8888");

    //两个按钮都不能按
    ui->pushButtonFile->setEnabled(false);
    ui->pushButtonSend->setEnabled(false);

    //如果客户端陈工和服务器链接
    //tcpServer自动触发 newConnection()

    connect(tcpServer, &QTcpServer::newConnection,
            [=]()
    {
        //取出建立好连接的套接字
        tcpSocket = tcpServer->nextPendingConnection();
        //获取对方的ip和端口
        QString ip = tcpSocket->peerAddress().toString();
        quint16 port = tcpSocket->peerPort();

        QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);

        ui->textEdit->setText(temp);
        //成功连接后 才能按选择文件
        ui->pushButtonFile->setEnabled(true);
//        ui->pushButtonSend->setEnabled(true);

    }
            );

    connect(&timer, &QTimer::timeout,
            [=]()
    {
        timer.stop();
        sendData();
    }
            );

}


ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::on_pushButtonFile_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "open", "../");
    if(filepath.isEmpty() == false){
        fileName.clear();
        fileSize = 0;
        //获取文件信息
        //只读方式打开文件
        QFileInfo info(filepath);
        fileName = info.fileName();
        fileSize = info.size(); //获取文件大小

        sendSize = 0;   //发送文件的小

        //指定文件的名字

        file.setFileName(filepath);
        //打开文件
        bool isok = file.open(QIODevice::ReadOnly);
        if(isok == false){
            cout << "只读方式打开失败 77";
        }

        //提示打开文件的路径
        ui->textEdit->append(filepath);
        ui->pushButtonFile->setEnabled(false);
        ui->pushButtonSend->setEnabled(true);

    }else{
        cout << "选择文件路径出错 62";
    }

}

void ServerWidget::on_pushButtonSend_clicked()
{
    //先发送文件头信息
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    qint64 len = tcpSocket->write(head.toUtf8());
    if(len > 0){
        //发送真正的文件信息
        //防止tcp黏包问题
        //需要通过定时器延时20ms
        timer.start(20);
    }else{
        cout << "头部信息发送失败";
        file.close();
        ui->pushButtonFile->setEnabled(true);
        ui->pushButtonSend->setEnabled(false);
    }
    //发送真正的文件信息
}
void ServerWidget::sendData()
{
    qint64 len = 0;
    do{
        //每次发送数据大大小
        char buf[4*1024] = {0};
//        len = 0;
        //往文件中读数据
        len = file.read(buf, sizeof(buf));
        len = tcpSocket->write(buf, len);
        //发送的数据需要累积
        sendSize += len;

    }while(len > 0);

    //是否发送文件完毕
    if(sendSize == fileSize){
        ui->textEdit->append("文件发送完毕");
        file.close();
//        tcpSocket->disconnectFromHost();
//        tcpSocket->close();

        ui->pushButtonFile->setEnabled(true);
        ui->pushButtonSend->setEnabled(false);

    }


}
