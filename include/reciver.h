#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore/qstringview.h>
#include <QtNetwork/qtcpsocket.h>
#include <QBuffer>
#include <cstddef>

class Reciver : public QObject
{
    Q_OBJECT
public:
    Reciver(QObject *parent = nullptr);

public slots:
    void listen(int port);

private slots:
    void onNewConnection();
    
private:
    QTcpServer *_server;
};

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QTcpSocket* socket,QObject *parent = nullptr);
    
private slots:
    void onReadyRead();

private:
    void processData();
    enum MessageType{
        UpdateValue = 0x01,
        ConsoleMessage = 0x02,
        MessageBox = 0x03,
    };

    void processUpdateValue(QDataStream& stream);
    void processConsoleMessage(QDataStream& stream);
    void processMessageBox(QDataStream& stream);

private:
    QTcpSocket *_socket;
    QByteArray _buffer;
    int _packageSize;
};