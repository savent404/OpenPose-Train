#include "myprob.h"

MyProb::MyProb(QObject *parent) : QObject(parent), Prob(parent)
{
    pQmlCameraObj = nullptr;
    pCamera = nullptr;
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

void MyProb::processFrame(const QVideoFrame& frame)
{
    if (!frame.isValid())
        return;
    if (!bTimeout)
        return;
    bTimeout = false;

    QVideoFrame f(frame);
    QImage img;
    f.map(QAbstractVideoBuffer::ReadOnly);

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

