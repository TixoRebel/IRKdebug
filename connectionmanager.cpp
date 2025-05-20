#include "connectionmanager.h"

BaseConnection::BaseConnection() { }

BaseConnection::~BaseConnection() { }

TcpConnection::TcpConnection(const QHostAddress &address, uint16_t port) : BaseConnection(), address(address), port(port) { }

TcpConnection *TcpConnection::create(const QHostAddress &address, uint16_t port)
{
    return new TcpConnection(address, port);
}

IrkError TcpConnection::connect()
{
    IrkError status = IrkError::OK;

    socket.connectToHost(address, port);

    if (!socket.waitForConnected(10000))
    {
        internal_error = socket.error();

        switch (internal_error)
        {
            case QAbstractSocket::SocketError::SocketTimeoutError:
                status = IrkError::TIMEOUT;
                break;
            default:
                status = IrkError::NETWORK;
                break;
        }

        socket.abort();
    }

    return status;
}

IrkError TcpConnection::receivePacket(uint8_t *packet, size_t max_size, size_t *actual_size)
{
    if (!socket.waitForReadyRead(-1))
    {
        internal_error = socket.error();
        socket.abort();
        return IrkError::NETWORK;
    }

    int64_t read = socket.read(reinterpret_cast<char *>(packet), static_cast<qint64>(max_size));

    if (read < 0)
    {
        internal_error = socket.error();
        socket.abort();
        return IrkError::NETWORK;
    }

    *actual_size = static_cast<size_t>(read);

    return IrkError::OK;
}

IrkError TcpConnection::sendPacket(const uint8_t *packet, size_t length)
{
    size_t total_written = 0;

    while (total_written < length)
    {
        int64_t written = socket.write(reinterpret_cast<const char *>(packet + total_written), static_cast<qint64>(length - total_written));

        if (written < 0)
        {
            internal_error = socket.error();
            socket.abort();
            return IrkError::NETWORK;
        }

        total_written += static_cast<size_t>(written);

        if (!socket.waitForBytesWritten(-1))
        {
            internal_error = socket.error();
            socket.abort();
            return IrkError::NETWORK;
        }
    }

    return IrkError::OK;
}

ConnectionManager::ConnectionManager(QObject *parent) : QObject{parent}, internal(*this) { }

ConnectionManagerInternal::ConnectionManagerInternal(ConnectionManager &manager) : manager(manager) { }

WorkerThread *ConnectionManager::addConnection(BaseConnection *connection)
{
    auto thread = new WorkerThread(connection, this);

    connect(thread, &WorkerThread::finished, thread, &WorkerThread::deleteLater);
    connect(&internal, &ConnectionManagerInternal::sendPacket, thread, &WorkerThread::sendPacket);

    return thread;
}

void ConnectionManager::sendPacket(const uint8_t *data, size_t length)
{
    emit internal.sendPacket(data, length);
}

WorkerThread::WorkerThread(BaseConnection *connection, ConnectionManager *manager, QObject *parent) : QThread(parent), connection(connection), manager(manager) { }

void WorkerThread::run()
{
    IrkError status = IrkError::OK;

    emit connectingStarted(status);

    status = connection->connect();

    emit connectingFinished(status);

    while (IrkError::OK == status)
    {
        uint64_t actual_size = 0;
        status = connection->receivePacket(connection->buffer, connection->buf_size, &actual_size);

        if (actual_size > 0)
        {
            emit manager->packetReceived(connection->buffer, actual_size);
        }
    }

    emit disconnected(status);

    delete connection;
    connection = nullptr;
}

void WorkerThread::sendPacket(const uint8_t *data, size_t length)
{
    connection->sendPacket(data, length);
}
