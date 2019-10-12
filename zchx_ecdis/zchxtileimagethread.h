#ifndef ZCHXTILEIMAGETHREAD_H
#define ZCHXTILEIMAGETHREAD_H

#include <QRunnable>
#include <QPixmap>

namespace qt {
class zchxTileImageThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit zchxTileImageThread(const QStringList& url, const QString& name,int pos_x, int pos_y, bool imgSync, QObject* retobj,  QObject *parent = 0);
    void run();
    QPixmap* loadImageFromUrl(const QString& url);
    QPixmap* loadImage();
    int Byte2Int(const QByteArray& src);

signals:
    void signalSend(const QPixmap& img, int x, int y);
    void signalSend(const QList<QPixmap>& img_list, int x, int y);
public slots:
private:
    QStringList mUrlList;
    QString mUrl;
    int mPx;
    int mPy;
    bool mImgSync;
    QString mName;
    QObject* mReturnObj;
};
}

#endif // ZCHXTILEIMAGETHREAD_H
