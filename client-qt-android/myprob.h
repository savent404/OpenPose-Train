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
#include <QJsonArray>
#include <QJsonObject>

class MyProb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* qmlCamera READ getQmlCamera WRITE setQmlCamera)
    Q_PROPERTY(int type READ getType WRITE setType)
    Q_PROPERTY(bool timeout READ isTimeout WRITE setTimeout)
    Q_PROPERTY(int a1 WRITE setA1)
    Q_PROPERTY(int a2 WRITE setA2)
    Q_PROPERTY(int a3 WRITE setA3)
    Q_PROPERTY(int a4 WRITE setA4)
    Q_PROPERTY(int a5 WRITE setA5)
    Q_PROPERTY(int times WRITE setTimes)
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
    int angles[5];
    int times;
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
    void setA1(int v) { angles[0] = v; }
    void setA2(int v) { angles[1] = v; }
    void setA3(int v) { angles[2] = v; }
    void setA4(int v) { angles[3] = v; }
    void setA5(int v) { angles[4] = v; }
    void setTimes(int v) { times = v; }

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

public:
signals:
    void transmitError(int errCode, const QString& errMsg);
    void feedBack(bool isCorrect, const QString& msg);
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
