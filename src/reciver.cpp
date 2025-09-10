#include "reciver.h"
#include "setting_manager.hpp"
#include "valve_manager.h"
#include <QtCore/qstringview.h>
#include <QtCore/qvariant.h>
#include <cstdint>

Reciver::Reciver(QObject *parent)
    : QObject(parent)
{
    _server = new QTcpServer(this);
    _server->listen(
        QHostAddress::Any,
        ConfigManager::getValue<int>("server_port", 8080)
    );

    connect(
        _server,
        &QTcpServer::newConnection,
        this,
        &Reciver::onNewConnection
    );


};

void Reciver::onNewConnection(){
    while(_server->hasPendingConnections()){
        QTcpSocket *socket = _server->nextPendingConnection();
        new Connection(socket);
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
    QByteArray data = _buffer.mid(1,_packageSize);
    _buffer.remove(0, _packageSize);

    QDataStream stream{data};

    ValueType value;
    uint8_t nameLength;
    char* rawName;

    stream.skipRawData(1); // 数据包长度
    stream >> value;
    stream >> nameLength;
    rawName = new char[nameLength];
    stream.readRawData(rawName, nameLength);

    std::string name{rawName,nameLength};

    valueManager.set(name,value);
}