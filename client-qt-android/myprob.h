#ifndef MYPROB_H
#define MYPROB_H

#include <QObject>
#include <QtMultimedia/QVideoProbe>
#include <QtMultimedia/QCamera>
#include <QBuffer>
#include "yuv2rgb.h"
#include <QStandardPaths>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

class MyProb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* qmlCamera READ getQmlCamera WRITE setQmlCamera)
    Q_PROPERTY(int type READ getType WRITE setType)
    Q_PROPERTY(bool timeout READ isTimeout WRITE setTimeout)
public:
    explicit MyProb(QObject *parent = nullptr);
    ~MyProb();

    QObject *getQmlCamera() const;
    void setQmlCamera(QObject *qmlCamera);

    bool isTimeout() const;
    void setTimeout(bool);

    int getType();
    void setType(int t);
private:
    QObject* pQmlCameraObj;
    QCamera* pCamera;
    QVideoProbe Prob;
    int type;
    bool bTimeout;

    bool recvAsyncEnabled;
    // -> port: 9000
    QUdpSocket sock_ctl_send;
    // port: 9001 <-
    QUdpSocket sock_ctl_recv;

    QHostAddress serverIP;

    enum { waitingHandShake, waitingJson, running} status;

private:
    void generateJson(QJsonObject& json, const int height, const int width);
    bool transmitFile(const void *file, size_t fileSize, int fileType);
    void enableAsyncRecv(bool isEnable=true) {
        recvAsyncEnabled = isEnable;
    }
    bool strncmp(const char* arg1, const char* arg2, int len) {
        while (len--) {
            if (*arg1++ != *arg2++)
                return false;
        }  return true;
    }
    QByteArray bytes2QBA(void *bytes, int len) {
        QByteArray a;
        uchar* ptr = (uchar*)(bytes);
        for (int i = 0; i < len; i++) {
            uchar d = *ptr++;
            a.append(char(d));
        } return a;
    }
    QByteArray int2QBG(uint32_t num) {
        return bytes2QBA(&num, 4);
    }
    QByteArray int162QBG(uint16_t num) {
        return bytes2QBA(&num, 2);
    }

signals:
    void TransmitError(int errCode, const QString& errMsg);
    void FeedBack(bool isCorrect, const QString& msg);
    void ResponseMsg(int code, const QString& errMsg);
public slots:
    void processFrame(const QVideoFrame &frame);
private slots:
    void recvServerMsg();
};

class TcpTransmiter : public QThread
{
    Q_OBJECT
public:
    TcpTransmiter(const char* data, size_t len, QHostAddress ip, quint16 port);
    int result;
private:
    QTcpSocket *sender;
    const char* data;
    size_t len;
    QHostAddress ip;
    quint16 port;
protected:
    void run();
};

#endif // MYPROB_H
