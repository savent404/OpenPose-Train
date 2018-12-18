#ifndef MYPROB_H
#define MYPROB_H

#include <QObject>
#include <QtMultimedia/QVideoProbe>
#include <QtMultimedia/QCamera>
#include <QBuffer>
#include "yuv2rgb.h"
#include <QStandardPaths>

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
    QImage picture;
signals:
    void TransmitError(int errCode, const QString& errMsg);
    void ResponseMsg(int code, const QString& errMsg);
public slots:
    void processFrame(const QVideoFrame &frame);
};

#endif // MYPROB_H
