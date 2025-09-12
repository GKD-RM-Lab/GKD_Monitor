#include "reciver.h"
#include "mainwindow.h"
#include "setting_manager.hpp"
#include "valve_manager.h"
#include <QtCore/qobject.h>
#include <QtCore/qstringview.h>
#include <QtCore/qvariant.h>
#include <QtWidgets/qmessagebox.h>
#include <cstdint>
#include <QMessageBox>
#include <iostream>
#include "log_box.h"
#include <QMessageBox>
Reciver::Reciver(QObject *parent)
    : QObject(parent)
{
    _server = new QTcpServer(this);

    connect(
        _server,
        &QTcpServer::newConnection,
        this,
        &Reciver::onNewConnection
    );


};

void Reciver::listen(int port){
    _server->listen(QHostAddress::Any,port);
}

void Reciver::onNewConnection(){
    while(_server->hasPendingConnections()){
        QTcpSocket *socket = _server->nextPendingConnection();
        new Connection(socket,_mainWindow,this);
        QString info = tr("New connection from %1:%2").arg(socket->peerAddress().toString(),QString::number(socket->peerPort()));
        QMessageBox::information(nullptr,tr("Information"),info);
    }
}

Connection::Connection(QTcpSocket* socket,MainWindow *mainWindow,QObject *parent)
    : QObject(parent)
    , _socket(socket)
    , _mainWindow(mainWindow)
{
    connect(
        _socket,
        &QTcpSocket::readyRead,
        this,
        &Connection::onReadyRead
    );

    connect(
        _socket,
        &QTcpSocket::disconnected,
        this,
        &Connection::deleteLater
    );
}

void Connection::onReadyRead(){
    _buffer += _socket->readAll();

    if(_buffer.size() < 2)
        return;

    _packageSize = _buffer.left(2).toUShort();

    if(_buffer.size() >= _packageSize){
        processData();
        _packageSize = 0;
    }
}

void Connection::processData(){ 
    QByteArray data = _buffer.mid(0,_packageSize);
    _buffer.remove(0, _packageSize);

    QDataStream stream{data};
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.skipRawData(2); // 数据包长度

    quint8 type;
    stream >> type;

    switch(type){
        case MessageType::UpdateValue:
            processUpdateValue(stream);
            break;
        case MessageType::ConsoleMessage:
            processConsoleMessage(stream);
            break;
        case MessageType::MessageBox:
            processMessageBox(stream);
            break;
    }
}

void Connection::processUpdateValue(QDataStream& stream){
    ValueType value;
    uint8_t nameLength;
    char* rawName;

    stream >> value;
    stream >> nameLength;
    rawName = new char[nameLength];
    stream.readRawData(rawName, nameLength);

    std::string name{rawName,nameLength};

    valueManager.updateValue(name,value);
}

void Connection::processConsoleMessage(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    _mainWindow->logBox()->addText(QString::fromStdString({message,messageSize}));
    delete[] message;
}
void Connection::processMessageBox(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    QMessageBox::information(nullptr,tr("Message"),QString::fromStdString({message,messageSize}));
    delete[] message;
}