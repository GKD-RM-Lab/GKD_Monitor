#include "reciver.h"
#include "setting_manager.hpp"
#include "valve_manager.h"
#include <QtCore/qobject.h>
#include <QtCore/qstringview.h>
#include <QtCore/qvariant.h>
#include <QtWidgets/qmessagebox.h>
#include <cstdint>
#include <QMessageBox>
#include <iostream>
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
        new Connection(socket);
        QString info = tr("New connection from %1:%2").arg(socket->peerAddress().toString(),QString::number(socket->peerPort()));
        QMessageBox::information(nullptr,tr("Information"),info);
    }
}

Connection::Connection(QTcpSocket* socket,QObject *parent)
    : QObject(parent)
    , _socket(socket)
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

    if(_packageSize == 0){
        _packageSize = _buffer[0];
    }

    if(_buffer.size() >= _packageSize){
        processData();
    }
}

void Connection::processData(){ 
    QByteArray data = _buffer.mid(0,_packageSize);
    _buffer.remove(0, _packageSize);

    QDataStream stream{data};
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.skipRawData(1); // 数据包长度

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
void Connection::processMessageBox(QDataStream& stream){
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