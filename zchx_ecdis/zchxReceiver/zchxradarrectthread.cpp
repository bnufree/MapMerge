#include "zchxradarrectthread.h"
#include "zmq.h"
#include <QDebug>
#include <QFile>
#include "zchxMapDatautils.h"

using namespace ZCHX_RADAR_RECEIVER;
using namespace qt;

#define     RECT_SAVE_SECS              (5 * 60)
#define     SECS_PER_DAY                (3600 * 24)

ZCHXRadarRectThread::ZCHXRadarRectThread(const ZCHX_RadarRect_Param& param, QObject *parent)
    : ZCHXReceiverThread(param.mSetting, parent)
    , mRectParam(param)
    , mFilterSecs(60) //默认输出1分钟以内的回波矩形块
{
    qRegisterMetaType<PROTOBUF_RadarRectList>("PROTOBUF_RadarRectList");
    qRegisterMetaType<PROTOBUF_RadarHistoryRectList>("PROTOBUF_RadarHistoryRectList");
    qRegisterMetaType<QList<ZCHX::Data::ITF_RadarRect>>("const QList<ZCHX::Data::ITF_RadarRect>&");
}



void ZCHXRadarRectThread::run()
{
    bool connect_flag = false;
    void *pCtx = NULL;
    void *pSock = NULL;

    int no_recv_num = 0;
    while(!isOver)
    {
        //检查是否需要重连
        if(!connect_flag)
        {
            qDebug()<<"start connect to server";
            if(pSock)   zmq_close(pSock);
            if(pCtx)    zmq_ctx_destroy(pCtx);
            //开始连接服务器
            //创建context，zmq的socket 需要在context上进行创建
            if((pCtx = zmq_ctx_new()) == NULL)
            {
                sleep(3);
                continue;
            }
            //创建zmq socket ，socket目前有6中属性 ，这里使用SUB方式
            //具体使用方式请参考zmq官方文档（zmq手册）
            if((pSock = zmq_socket(pCtx, ZMQ_SUB)) == NULL)
            {
                zmq_ctx_destroy(pCtx);
                pCtx = 0;
                sleep(3);
                continue;
            }

            //设置topic 过滤
            const char *filter = mRadarCommonSettings.m_sTopic.toLatin1().data();
            if(zmq_setsockopt(pSock, ZMQ_SUBSCRIBE, filter, strlen(filter)))
            {
                zmq_close(pSock);
                zmq_ctx_destroy(pCtx);
                sleep(3);
                pSock = 0;
                pCtx = 0;
                continue;
            }
            //设置等待时间
            int timeout = 5 * 1000; //5s超时限制，没有收到消息就退出
            zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

            QString sAddress = "tcp://"+mRadarCommonSettings.m_sIP+":"+mRadarCommonSettings.m_sPort;
            const char *pAddr = sAddress.toLatin1().data();// = sAddress.toLocal8Bit().data();

            int nFlag = zmq_connect(pSock, pAddr);
            qDebug()<<"---------Radar_video ip addr------"<<sAddress;
            qDebug()<<"---------Radar_video topic ------"<<mRadarCommonSettings.m_sTopic;
            qDebug()<<" Radar_video --------nFlag:"<<nFlag;
            if(nFlag < 0)
            {
                qDebug()<<"error msg:"<<zmq_strerror(zmq_errno());
                zmq_close(pSock);
                zmq_ctx_destroy(pCtx);
                qDebug()<<" Radar_video zmq_connect() fail!";
                sleep(3);
                pSock = 0;
                pCtx = 0;
                continue;
            }
            connect_flag = true;
        }
//        qDebug()<<"start recv data now";
        //每次发送3帧数据 topic + 时间戳 + 数据
        QByteArrayList recvlist;
        while (1) {
            int64_t more = 0;
            size_t more_size = sizeof (more);
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            zmq_recvmsg(pSock, &msg, 0);
            QByteArray bytes2 = QByteArray((char*)zmq_msg_data(&msg),(int)(zmq_msg_size(&msg)));
            zmq_msg_close(&msg);
            if(bytes2.length() > 0)
            {
                recvlist.append(bytes2);
//                qDebug()<<"receive msg:"<<QString::fromUtf8(bytes2.data());
            }
            zmq_getsockopt (pSock, ZMQ_RCVMORE, &more, &more_size);
            if (more == 0)      //判断是否是最后消息
            {
                break; // 已到达最后一帧
            }
            //msleep(1000);
        }
//        qDebug()<<"receive video data ok:"<<recvlist.size();
        if(recvlist.length() < 3)
        {
            no_recv_num++;
            if(no_recv_num >= 10)
            {
                //这里清除掉旧的记录,自动重连
                no_recv_num = 0;
                connect_flag = false;
            }
            continue;
        } else
        {
            no_recv_num = 0;
        }
//        qDebug()<<"receive video data ok:"<<recvlist.at(2).size()<<QDateTime::fromMSecsSinceEpoch(recvlist[1].toLongLong()).toString("yyyy-MM-dd hh:mm:ss");
        qint64 secs = QDateTime::currentDateTime().toTime_t() - QDateTime(QDate::currentDate(), QTime(0, 0, 0)).toTime_t();
        PROTOBUF_RadarRectList objRadarRect;
        if(objRadarRect.ParseFromArray(recvlist.at(2).data(), recvlist.at(2).size()))
        {
            QTime t;
            t.start();
            QList<ZCHX::Data::ITF_RadarRect*> list;
            convertZMQ2ZCHX(/*list,*/ objRadarRect);
#if 0
            //检查数据,移除已经超时的历史数据
            QMap<int, ZCHX::Data::ITF_RadarRect>::iterator it = mDataMap.begin();
            for(; it != mDataMap.end();)
            {
                ZCHX::Data::ITF_RadarRect* rect = (ZCHX::Data::ITF_RadarRect*)(&(*it));
                if(rect)
                {
                    int sub = secs - rect->current.timeOfDay;
                    if(sub < 0) sub += SECS_PER_DAY;
                    if(sub > RECT_SAVE_SECS)
                    {
                        //目标已经根究没有更新,删除
                        mDataMap.erase(it++);
                        continue;
                    } else
                    {
                        //实时位置未过期,检查历史轨迹
                        for(int i=0; i<rect->rects.size(); i++)
                        {
                            ZCHX::Data::ITF_RadarHistoryRect& his = rect->rects[i];
                            sub = secs - his.timeOfDay;
                            if(sub < 0) sub += SECS_PER_DAY;
                            if(sub > RECT_SAVE_SECS)
                            {
                                rect->rects = rect->rects.mid(0, i);
                                break;
                            }
                        }
                    }
                    list.append(rect);
                }
                ++it;

            }
#endif

            //开始生成对应的回波矩形图形
            QThreadPool pool;
            pool.setMaxThreadCount(10);
            pool.setExpiryTimeout(-1);
            for(int i=0; i<list.size();i++)
            {
                zchxRectVideoFunc* func = new zchxRectVideoFunc(list[i]);
                pool.start(func);
            }
            pool.waitForDone();
            qDebug()<<"Parse video data ok:"<<objRadarRect.radarrects_size()<<QDateTime::fromMSecsSinceEpoch(recvlist[1].toLongLong()).toString("yyyy-MM-dd hh:mm:ss")<<" elapsed:"<<t.elapsed();
            {
                //开始构造输出的图形,输出在指定时间间隔内的图形
                QList<ZCHX::Data::ITF_RadarRect> list;
                QMap<int, ZCHX::Data::ITF_RadarRect>::iterator it = mDataMap.begin();
                for(; it != mDataMap.end(); it++)
                {
                    ZCHX::Data::ITF_RadarRect rect = *it;
        #if 0
                    int sub = secs - rect.current.timeOfDay;
                    if(sub < 0) sub += SECS_PER_DAY;
                    if(sub > mFilterSecs)
                    {
                        continue;
                    }

                    //实时位置未过期,检查历史轨迹
                    for(int i=0; i<rect.rects.size(); i++)
                    {
                        ZCHX::Data::ITF_RadarHistoryRect his = rect.rects[i];
                        sub = secs - his.timeOfDay;
                        if(sub < 0) sub += SECS_PER_DAY;
                        if(sub > mFilterSecs)
                        {
                            rect.rects = rect.rects.mid(0, i);
                            break;
                        }
                    }
        #endif
                    list.append(rect);

                }
                qDebug()<<"send rect size:"<<list.size();
                emit sendVideoMsg(mRadarCommonSettings.m_sSiteID, list);
            }
        } else
        {
            msleep(300);
        }
#if 0
        //开始构造输出的图形,输出在指定时间间隔内的图形
        QList<ZCHX::Data::ITF_RadarRect> list;
        QMap<int, ZCHX::Data::ITF_RadarRect>::iterator it = mDataMap.begin();
        for(; it != mDataMap.end(); it++)
        {
            ZCHX::Data::ITF_RadarRect rect = *it;
#if 0
            int sub = secs - rect.current.timeOfDay;
            if(sub < 0) sub += SECS_PER_DAY;
            if(sub > mFilterSecs)
            {
                continue;
            }

            //实时位置未过期,检查历史轨迹
            for(int i=0; i<rect.rects.size(); i++)
            {
                ZCHX::Data::ITF_RadarHistoryRect his = rect.rects[i];
                sub = secs - his.timeOfDay;
                if(sub < 0) sub += SECS_PER_DAY;
                if(sub > mFilterSecs)
                {
                    rect.rects = rect.rects.mid(0, i);
                    break;
                }
            }
#endif
            list.append(rect);

        }
        qDebug()<<"send rect size:"<<list.size();
        emit sendVideoMsg(mRadarCommonSettings.m_sSiteID, list);
#endif
    }
    if(pSock)   zmq_close(pSock);
    if(pCtx)    zmq_ctx_destroy(pCtx);
}

//这里将目标最大定位到1000米,图像大小为255*255
void zchxRectVideoFunc::run()
{
    if(!mRect) return;
    int    img_size = 255;
    int     pix_size = 500;
    double center_lat = mRect->current.centerlatitude;
    double center_lon = mRect->current.centerlongitude;
    //将经纬度点对应到图片上.图元的中心经纬度对应图片的中心
    //1)计算中心点对应的墨卡托坐标
    ZCHX::Data::Mercator cen_mct = zchxMapDataUtils::wgs84LatLonToMercator(center_lat, center_lon);
    //2)开始构建图片的二维数组点列
    char *img = new char[img_size * img_size];
    memset(img, 0, img_size * img_size);

    for(int i=0; i<mRect->current.blocks.size(); i++)
    {
        double lat = mRect->current.blocks[i].latitude;
        double lon = mRect->current.blocks[i].longitude;
        ZCHX::Data::Mercator circle_mct = zchxMapDataUtils::wgs84LatLonToMercator(lat, lon);
        //计算当前点对应中心点之间的距离
        double sub_mct_x = circle_mct.mX - cen_mct.mX;
        double sub_mct_y = circle_mct.mY - cen_mct.mY;
        //距离超处理图片的范围,不处理
        if(fabs(sub_mct_x) > pix_size / 2 || fabs(sub_mct_y) > pix_size / 2) continue;

        //开始计算对应的二维数组的下标
        int x = qFloor(sub_mct_x * (img_size / 2) / (pix_size / 2)); //[-127~127]
        int y = 0 - qFloor(sub_mct_y * (img_size / 2) / (pix_size / 2));//[-127~127]  直角坐标的Y向上变为绘图坐标系的向下
//        qDebug()<<sub_mct_x<<sub_mct_y <<x<<y;
        //转换到[0-254]范围
        x += (img_size / 2);
        y += (img_size / 2);
        //给数组赋值
        *(img+ y* img_size + x) = 1;
    }

    //3)遍历二维数组进行填充,统计每一行的开始点和结束点
    QPolygon left, right;
    int pre_start = -1, pre_end = -1;
    //首先获取最后有效的行
    int line_end = -1, line_start = -1;
    for(int i=img_size-1; i>=0; i--)
    {
        for(int j=0; j<img_size; j++)
        {
            if(*(img+i*img_size+j) == 1)
            {
                line_end = i;
                break;
            }
        }
        if(line_end >= 0) break;
    }

    for(int i=0; i<=line_end; i++)
    {
        int cur_start = -1, cur_end = -1;
        for(int j=0; j<img_size; j++)
        {
            if(*(img+i*img_size+j) == 1)
            {
                if(cur_start == -1) cur_start = j;
                cur_end = j;
                if(line_start == -1) line_start = i;
            }
        }
        if(cur_start == -1 && pre_start >= 0) cur_start = pre_start;
        if(cur_end == -1 && pre_end >= 0) cur_end = pre_end;

        //检查这两个点是否是相距太近,如果是,合并
        if(cur_start != -1 && fabs(cur_start - cur_end) < 5)
        {
            cur_start = cur_end;
        }

        if(cur_start == -1 && cur_end == -1) continue;
        if(pre_start == -1 && pre_end == -1)
        {
            //第一个点进来了,这里不管什么情况直接将开始和结束点分别插入左右侧
            left.append(QPoint(cur_start, i));
            right.append(QPoint(cur_end, i));
        } else
        {
            //计算最新的点相对于前一排的两个点的位置关系
            int start_d1 = cur_start - pre_start;
            int start_d2 = cur_start - pre_end;
            int end_d1 = cur_end - pre_start;
            int end_d2 = cur_end - pre_end;
            bool start_flag = false;
            bool end_flag = false;
            if(start_d1 <= 0)
            {
                //开始点再前一个开始点的左侧
                left.append(QPoint(cur_start, i));
                start_flag = true;
            }
            if(end_d2 >= 0)
            {
                //结束点再前一个结束点的右侧
                right.append(QPoint(cur_end, i));
                end_flag = true;
            }
            if((!start_flag) && start_d1 > 0)
            {
                //开始点再前一个开始点的右侧
                if(fabs(start_d1) < fabs(start_d2))
                {
                    left.append(QPoint(cur_start, i));
                    start_flag = true;
                }
            }

            if((!end_flag) && end_d2 < 0)
            {
                //结束点再前一个结束点的左侧
                if(fabs(end_d2) < fabs(end_d1))
                {
                    right.append(QPoint(cur_end, i));
                    end_flag = true;
                }
            }
        }

        if(left.size() > 0)pre_start = left.last().x();
        if(right.size() > 0) pre_end = right.last().x();
    }

    //左右点列进行合并
    for(int i=right.size()-1; i>=0; i--)
    {
        QPoint target = right[i];
        if(left.contains(target)) continue;
        left.append(target);
    }
    //对点列进行检查,去除锐角钝化
    for(int i=1; i<left.size()-1; )
    {
        QPoint pre = left[i-1];
        QPoint cur = left[i];
        if(i+1 > left.size() - 1) break;
        QPoint next = left[i+1];
        //检查当前点和前后的点是否是锐角
        //先将所有点的坐标变成直角坐标系.正方向X右Y上
        pre.setY(-1 * pre.y());
        cur.setY(-1 * cur.y());
        next.setY(-1 * next.y());
        //求两个向量的夹角
        //cos(@) = b•c / (|b| |c|)
        QVector2D p1(pre.x() - cur.x(), pre.y() - cur.y());
        QVector2D p2(next.x()- cur.x(), next.y()- cur.y());
        double dot = QVector2D::dotProduct(p1, p2);
        double len1 = p1.length();
        double len2 = p2.length();
        if(len1 * len2 > 0)
        {
            double angle = acos(dot / (len1 * len2));
            if( angle < 0.5 * GLOB_PI)
            {
                //锐角,删除
                left.removeAt(i);
                if(i >= 2)i--;
                continue;
            }
        }
#if 0
        //如果两个点相距太近,也删除
        if(len1 <= 2 || len2 <= 1)
        {
            left.removeAt(i);
            if(i >= 2)i--;
            continue;
        }
#endif
        i++;

    }
    //4)将数组点列画到图片上
    if(img)
    {
        delete []img;
        img = 0;
    }

    mRect->current.pixPoints = left;
    mRect->current.pixWidth = img_size;
    mRect->current.pixHeight = img_size;
}

void ZCHXRadarRectThread::convertZMQ2ZCHX(/*QList<ZCHX::Data::ITF_RadarRect> &res,*/ const PROTOBUF_RadarRectList &src)
{
//    res.clear();
    for (int i = 0; i < src.radarrects_size(); i++)
    {
        PROTOBUF_RadarRect obj = src.radarrects(i);
        ZCHX::Data::ITF_RadarRect rect;
        rect.radarSiteId = mRadarCommonSettings.m_sSiteID;
        rect.rectNumber = obj.rectnumber();
        rect.current.rectNumber = rect.rectNumber;
        rect.current.topLeftlatitude = obj.topleftlatitude();
        rect.current.topLeftlongitude = obj.topleftlongitude();
        rect.current.bottomRightlatitude = obj.bottomrightlatitude();
        rect.current.bottomRightlongitude = obj.bottomrightlongitude();
        rect.current.centerlatitude = obj.centerlatitude();
        rect.current.centerlongitude = obj.centerlongitude();
        rect.current.timeOfDay = obj.timeofday();
        rect.current.diameter = obj.diameter();
        rect.blockColor.setNamedColor(mRectParam.m_sCurColor);
        rect.blockEdgeColor.setNamedColor(mRectParam.m_sCurColor);
        rect.HisBlockColor.setNamedColor(mRectParam.m_sHistoryColor);
        rect.historyBackgroundColor.setNamedColor(mRectParam.m_sHistoryColor);
        rect.current.startlatitude = obj.startlatitude();
        rect.current.startlongitude = obj.startlongitude();
        rect.current.endlatitude = obj.endlatitude();
        rect.current.endlongitude = obj.endlongitude();
        rect.current.angle = obj.angle();
        rect.current.isRealData = true;

        for (int j = 0; j < obj.blocks().block_size(); j++)
        {
            PROTOBUF_SingleVideoBlock block = obj.blocks().block(j);

            ZCHX::Data::ITF_SingleVideoBlock item;
            item.latitude = block.latitude();
            item.longitude = block.longitude();

            rect.current.blocks.append(item);
        }
        //接收端自己做缓存数据
#if 0
        for (int j = 0; j < obj.rects().rects_size(); j++)
        {
            PROTOBUF_RadarHistoryRect historyObj = obj.rects().rects(j);

            ZCHX::Data::ITF_RadarHistoryRect hisRect;
            hisRect.rectNumber = historyObj.rectnumber();
            hisRect.topLeftlatitude = historyObj.topleftlatitude();
            hisRect.topLeftlongitude = historyObj.topleftlongitude();
            hisRect.bottomRightlatitude = historyObj.bottomrightlatitude();
            hisRect.bottomRightlongitude = historyObj.bottomrightlongitude();
            hisRect.centerlatitude = historyObj.centerlatitude();
            hisRect.centerlongitude = historyObj.centerlongitude();
            hisRect.timeOfDay = historyObj.timeofday();
            hisRect.startlatitude = historyObj.startlatitude();
            hisRect.startlongitude = historyObj.startlongitude();
            hisRect.endlatitude = historyObj.endlatitude();
            hisRect.endlongitude = historyObj.endlongitude();
            hisRect.angle = historyObj.angle();
            hisRect.isRealData = true;
            for(int k=0; k<historyObj.blocks().block_size(); k++)
            {
                com::zhichenhaixin::proto::singleVideoBlock node = historyObj.blocks().block(k);
                ZCHX::Data::ITF_SingleVideoBlock item;
                item.latitude = node.latitude();
                item.longitude = node.longitude();
                hisRect.blocks.append(item);
            }

            rect.rects.append(hisRect);
        }
#else
        //开始缓存最大时间内的数据
        if(mDataMap.contains(rect.rectNumber))
        {
            ZCHX::Data::ITF_RadarRect& old_data = mDataMap[rect.rectNumber];
            old_data.rects.prepend(old_data.current); //将原来的实时数据更新到历史,更新到第一个位置
            old_data.current = rect.current; //将最新的实时数据更新到当时
        } else
        {
            //没有数据,则只有一个数据
            mDataMap[rect.rectNumber] = rect;
        }

#endif

//        res.append(rect);
    }
}
