#include "myprob.h"

MyProb::MyProb(QObject *parent) :
    QObject(parent),
    Prob(parent),
    sock_ctl_send(this),
    sock_ctl_recv(this)
{
    pQmlCameraObj = nullptr;
    pCamera = nullptr;
    enableAsyncRecv(true);

    status = waitingHandShake;

    sock_ctl_recv.bind(QHostAddress::AnyIPv4, 9001);

    connect(&sock_ctl_recv, SIGNAL(readyRead()),
            this, SLOT(recvServerMsg()));
    
    // send handshake msg to server
    sock_ctl_send.writeDatagram("STA\x01", 4, QHostAddress::Broadcast, 9000);

    status = waitingHandShake;

}

MyProb::~MyProb()
{

}

QObject *MyProb::getQmlCamera() const
{
    return pQmlCameraObj;
}

void MyProb::setQmlCamera(QObject *qmlCamera)
{
    pQmlCameraObj = qmlCamera;
    if (pQmlCameraObj) {
        pCamera = qvariant_cast<QCamera*>(pQmlCameraObj->property("mediaObject"));
        if (pCamera && Prob.setSource(pCamera)) {
            connect(&Prob, SIGNAL(videoFrameProbed(const QVideoFrame&)),
                    this, SLOT(processFrame(const QVideoFrame&)));
        }
        else {
            qDebug() << "Can's set source camera to QVideoProb";
        }
    }
}

bool MyProb::isTimeout() const
{
    return bTimeout;
}

void MyProb::setTimeout(bool b)
{
    bTimeout = b;
}

int MyProb::getType()
{
    return type;
}

void MyProb::setType(int t)
{
    type = t;
}

void MyProb::generateJson(QJsonObject &obj, const int height, const int width)
{

    obj.insert("type", QJsonValue::fromVariant(type));
    obj.insert("height", QJsonValue::fromVariant(height));
    obj.insert("width", QJsonValue::fromVariant(width));
    obj.insert("interval", QJsonValue::fromVariant(500));
}

bool MyProb::transmitFile(const void *file, size_t fileSize, int fileType)
{
    if (status == waitingHandShake) {
        return false;
    }
    bool isOK = false;
    // disable async handle first
    enableAsyncRecv(false);
    // Send a start transmit msg
    QByteArray msg;
    msg += "STA\x04:";
    msg += int2QBG(uint32_t(fileType));
    msg += ":";
    msg += int2QBG(uint32_t(fileSize));
    sock_ctl_send.writeDatagram(msg, serverIP, 9000);

    // set a timer & waiting for server feedback
    // timeout=100ms
    QTimer timer(this);
    timer.start(1000);
    while (!sock_ctl_recv.hasPendingDatagrams()) {
        if (timer.remainingTime() == 0) {
            qDebug() << "Waiting for server a long time, exit";
            emit TransmitError(-2, "Recv start flag ack timeout");
            return false;
        }
    }
    // read feed back
    int tcpPort = 0;
    while (sock_ctl_recv.hasPendingDatagrams()) {
        QByteArray Buffer;
        Buffer.resize(int(sock_ctl_recv.pendingDatagramSize()));
        QHostAddress ip;
        quint16 port;
        sock_ctl_recv.readDatagram(Buffer.data(), Buffer.size(),
                                   &ip, &port);
        qDebug() << "Receive msg from: " << ip.toString() << ":" << port
            << " Received msg: " << Buffer;
        if (ip != serverIP)
            continue;
        if (strncmp(Buffer.data(), (const char*)"STA\x05:", 5)) {
            tcpPort = QString(Buffer.data() + 5).toInt();
            isOK = true;
            qDebug() << "Get ack from server, tcp port: " << tcpPort;
        }
    }
    // re-enable async
    enableAsyncRecv(true);

    if (!isOK)
        return false;

    TcpTransmiter a((const char*)file, fileSize, serverIP, tcpPort);
    a.start();
    a.wait();

    if (a.result != 0) {
        emit TransmitError(a.result, "Transmit error");
        return false;
    }

    return true;
}

void MyProb::processFrame(const QVideoFrame& frame)
{
    if (!frame.isValid())
        return;

    if (status == waitingHandShake)
        return;

    if (status == waitingJson)
    {
        int h = frame.height();
        int w = frame.width();
        QJsonObject obj;
        generateJson(obj, h, w);
        QJsonDocument doc(obj);
        QByteArray arry = doc.toJson();
        if (transmitFile(arry.data(), size_t(arry.size()), 2)) {
            status = running;
        }
        else {
            qDebug() << "Config error, try next time";
        }
        return;
    }

    if (!bTimeout)
        return;

    bTimeout = false;

    QVideoFrame f(frame);
    QImage img;
    f.map(QAbstractVideoBuffer::ReadOnly);

    if (transmitFile(f.bits(), uint32_t(f.mappedBytes()), 1) == false) {
        qDebug() << "Transmit error";
        emit TransmitError(-1, "Can't transmit img file");
    }
//    auto origin_format = f.pixelFormat();
//    auto format = QVideoFrame::imageFormatFromPixelFormat(f.pixelFormat());
//    if (format != QImage::Format_Invalid) {
//        img = QImage(f.bits(),
//                     f.width(),
//                     f.height(),
//                     format);
//    }
//    else if (origin_format == QVideoFrame::Format_NV21) {
//        cv::Mat yuv(f.height() + f.height()/2, f.width(), CV_8UC1, f.bits());
//        cv::Mat converted(f.height(), f.width(), CV_8UC3);
//        cv::cvtColor(yuv, converted, CV_YUV2BGR_NV21);
//        QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/1.jpg";
//        std::vector<uchar> buffer;
//        cv::imencode(".png", converted, buffer);
//    }
//    else {
//        int nBytes = f.mappedBytes();
//        img = QImage::fromData(frame.bits(), nBytes);
//    }

    f.unmap();

}

void MyProb::recvServerMsg()
{
    if (recvAsyncEnabled == false)
        return;

    QByteArray Buffer;
    Buffer.resize(int(sock_ctl_recv.pendingDatagramSize()));

    QHostAddress ip;
    quint16 unusedPort;
    sock_ctl_recv.readDatagram(Buffer.data(), Buffer.size(),
                               &ip, &unusedPort);
    qDebug() << "Receive msg from: " << ip.toString() << ":" << unusedPort
        << " Received msg: " << Buffer;
    
    // check if it is handshake ack msg
    if (Buffer == "STA\x02" && status == waitingHandShake) {
        qDebug() << "HandShake ack found";
        serverIP = ip;
        status = waitingJson;
    }
    if (status == running && strncmp(Buffer.data(), "STA\x06:", 5)) {
        qDebug() << "Receive a server's feed back";
        QByteArray st(Buffer.data() + 5, Buffer.size() - 5);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(st);
        QJsonObject jsonObj = jsonDoc.object();
        bool isCorrect = jsonObj["isCorrect"].toBool();
        QString msg = jsonObj["msg"].toString();
        emit FeedBack(isCorrect, msg);
    }
}

TcpTransmiter::TcpTransmiter(const char *data, size_t len, QHostAddress ip, quint16 port) :
    data(data),len(len), ip(ip), port(port), result(-1)
{
    sender = new QTcpSocket();
}

void TcpTransmiter::run()
{
    sender->connectToHost(ip, port, QTcpSocket::WriteOnly);
    if (!sender->waitForConnected()) {
        qDebug() << "Tcp connet time out";
        result = 1;
        return;
    }
    else {
        auto r = sender->write(data, qint64(len));
        if (r < 0) {
            qDebug() << "Tcp write error";
            result = 2;
            return;
        }
    }
    if (!sender->waitForBytesWritten()) {
        qDebug() << "Tcp writen timeout";
        result = 3;
        return;
    }
    sender->disconnectFromHost();
    result = 0;
}
