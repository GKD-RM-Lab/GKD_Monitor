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

private:
    QTcpSocket *_socket;
    QByteArray _buffer;
    int _packageSize;
};