#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QThread>
#include <QTcpSocket>

#include "irkerror.h"

class BaseConnection
{
    Q_DISABLE_COPY_MOVE(BaseConnection)

    friend class ConnectionManager;
    friend class WorkerThread;

    static constexpr size_t buf_size = 1024;
    uint8_t buffer[buf_size];

protected:
    explicit BaseConnection();
    virtual ~BaseConnection();

    virtual IrkError connect() = 0;
    virtual IrkError receivePacket(uint8_t *packet, size_t max_size, size_t *actual_size) = 0;
    virtual IrkError sendPacket(const uint8_t *packet, size_t size) = 0;
};

class TcpConnection : public BaseConnection
{
    const QHostAddress address;
    uint16_t port;
    QTcpSocket socket;
    QAbstractSocket::SocketError internal_error = QAbstractSocket::SocketError::UnknownSocketError;

    explicit TcpConnection(const QHostAddress &address, uint16_t port);

public:
    static TcpConnection *create(const QHostAddress &address, uint16_t port);

protected:
    IrkError connect() override;
    IrkError receivePacket(uint8_t *packet, size_t max_size, size_t *actual_size) override;
    IrkError sendPacket(const uint8_t *packet, size_t size) override;
};

class ConnectionManager;

class WorkerThread : public QThread
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(WorkerThread)

    friend class ConnectionManager;

    BaseConnection *connection;
    ConnectionManager *manager;

    explicit WorkerThread(BaseConnection *connection, ConnectionManager *manager, QObject *parent = nullptr);
    void run() override;

signals:
    void connectingStarted(IrkError status);
    void connectingFinished(IrkError status);
    void disconnected(IrkError status);

private slots:
    void sendPacket(const uint8_t *data, size_t length);
};

class ConnectionManagerInternal : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(ConnectionManagerInternal)

    friend class ConnectionManager;

    ConnectionManager &manager;

    ConnectionManagerInternal(ConnectionManager &manager);

signals:
    void sendPacket(const uint8_t *data, size_t length);
};

class ConnectionManager : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(ConnectionManager)

    ConnectionManagerInternal internal;

public:
    explicit ConnectionManager(QObject *parent = nullptr);

    WorkerThread *addConnection(BaseConnection *connection);

signals:
    void packetReceived(const uint8_t *data, size_t length);

public slots:
    void sendPacket(const uint8_t *data, size_t length);
};

#endif // CONNECTIONMANAGER_H
