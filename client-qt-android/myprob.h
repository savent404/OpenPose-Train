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

class MyProb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* qmlCamera READ getQmlCamera WRITE setQmlCamera)
    Q_PROPERTY(bool timeout READ isTimeout WRITE setTimeout)
public:
    explicit MyProb(QObject *parent = nullptr);
    ~MyProb();

    QObject *getQmlCamera() const;
    void setQmlCamera(QObject *qmlCamera);

    bool isTimeout() const;
    void setTimeout(bool);
private:
    QObject* pQmlCameraObj;
    QCamera* pCamera;
    QVideoProbe Prob;
    bool bTimeout;

    bool recvAsyncEnabled;
    // -> port: 9000
    QUdpSocket sock_ctl_send;
    // port: 9001 <-
    QUdpSocket sock_ctl_recv;

    QHostAddress serverIP;

    enum { waitingHandShake, running} status;

private:
    void enableAsyncRecv(bool isEnable=true) {
        recvAsyncEnabled = isEnable;
    }
    bool strncmp(char* arg1, char* arg2, int len) {
        while (len--) {
            if (*arg1++ != *arg2++)
                return false;
        }  return false;
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
    void ResponseMsg(int code, const QString& errMsg);
public slots:
    void processFrame(const QVideoFrame &frame);
private slots:
    void recvServerMsg();
};

#endif // MYPROB_H
