#include "radarrectelement.h"
#include "zchxmapframe.h"
#include "map_layer/zchxmaplayermgr.h"
#include <QPainter>
#include "zchxutils.hpp"
#include "profiles.h"
#include "zchxMapDataUtils.h"

#define OFFSET_X 0.00014
#define OFFSET_Y 0.00014
#define MIN_SHOW_DIS 20

#define     CUT_FISRT_DIRECTLY          0

using namespace ZCHX::Data;

namespace qt {
int RadarRectGlowElement::mMaxRectLength = Profiles::instance()->value(MAP_INDEX, MAX_RECT_PIXEL_LENGTH).toInt();
int RadarRectGlowElement::mMaxGapOf2Rect = 20;
RadarRectGlowElement::RadarRectGlowElement(const ZCHX::Data::ITF_RadarRect& data, zchxMapWidget* frame)
    :Element(data.current.centerlatitude, data.current.centerlongitude, frame, ZCHX::Data::ELE_RADAR_RECTGLOW)
{
    setData(data);
}

const ITF_RadarRect &RadarRectGlowElement::data() const
{
    return mRect;
}

void RadarRectGlowElement::setData(const ZCHX::Data::ITF_RadarRect &rect)
{    
    QMutexLocker locker(&m_mutex);

    mRect = rect;
    int size = mRect.rects.size();
    if(size == 0) return;
    mInsertPointList.clear();
    mOrigonPointList.clear();
    //检查轨迹点列,从反向的点开始截断
    bool early_first = false;
    //1)检查历史轨迹的点列,判定他的存储方式   第一个轨迹点在数组的最后点还是第一个点
    int last_time = int(mRect.rects.last().timeOfDay);
    int first_time = int(mRect.rects.first().timeOfDay);
    if(last_time > first_time) early_first = true;
    //2)第一个轨迹点在第一个元素,则将数组进行反向,使得第一个轨迹点在最后
    if(early_first)
    {
        for(int i=0; i<size / 2; i++)
        {
            ZCHX::Data::ITF_RadarHistoryRect temp = mRect.rects[size-1-i];
            mRect.rects[size-1-i] = mRect.rects[i];
            mRect.rects[i] = temp;
        }
    }
    //3) 检查历史轨迹的第一个点是否和当前的点重合,如果重合,就将他删除
    int  cur_time = int(mRect.current.timeOfDay);
    first_time = int(mRect.rects.first().timeOfDay);
    if(cur_time == first_time) mRect.rects.removeFirst();
    //4)从当前点开始计算轨迹的运动方向,反向点开始截断
    if(CUT_FISRT_DIRECTLY)
    {
        if(mRect.rects.size() > 0)
        {
            ITF_RadarHistoryRect last = mRect.rects.last();
            mRect.rects.clear();
            last.angle = mRect.current.angle;
            mRect.rects.append(last);
        }

    } else
    {
        //重新计算各个点的运动方向
        ITF_RadarHistoryRect pre_rect = mRect.current;
        for(int i=0; i<mRect.rects.size();)
        {
            ITF_RadarHistoryRect &cur_rect = mRect.rects[i];
            //计算当前点和前一个点之间的距离
            int dis = qRound(ZCHX::Utils::getDistanceDeg(pre_rect.centerlatitude, pre_rect.centerlongitude, cur_rect.centerlatitude, cur_rect.centerlongitude));
            if(dis < mMaxGapOf2Rect)
            {
                mRect.rects.removeAt(i);
                continue;
            }

            if(PROFILE_INS->value(MAP_INDEX, MAP_USE_RECT_COG, true).toBool())
            {
                double angle = ZCHX::Utils::calcAzimuth(cur_rect.centerlongitude,
                                                        cur_rect.centerlatitude,
                                                        pre_rect.centerlongitude,
                                                        pre_rect.centerlatitude);
                cur_rect.angle = angle;
            }
            pre_rect = cur_rect;
            i++;
        }

        double pre_angle = mRect.current.angle;
        for(int i=0; i<mRect.rects.size(); i++)
        {
            double cur_angle = mRect.rects[i].angle;
            if(fabs(cur_angle - pre_angle) >= 90)
            {
                //开始截断
                mRect.rects = mRect.rects.mid(0, i);
                break;
            }

            pre_angle = cur_angle;
        }
    }

    //开始进行插值处理
    mRect.rects.prepend(mRect.current);
    for(int i= 1; i < mRect.rects.size();)
    {
        ITF_RadarHistoryRect pre = mRect.rects[i-1];
        ITF_RadarHistoryRect cur = mRect.rects[i];
        ITF_RadarHistoryRectList midList;
        calculateSkipRect(midList, pre, cur);
        for(int k = 0; k < midList.size(); k++)
        {
            mRect.rects.insert(i, midList[k]);
            i++;
        }
        i++;
    }
    mRect.rects.removeFirst();
    setIsUpdate(true);
}

void RadarRectGlowElement::drawOutline(QPainter *painter, const QPointF& center, double in, double out)
{
    if(!painter) return;
    PainterPair chk(painter);
    painter->setPen(QColor(200,200,100,200));
    painter->drawEllipse(center, out, in);
}

void RadarRectGlowElement::drawRadarRect(const QPointF &leftTopPos, const QPointF &bottomRightPos, QPainter *painter)
{
    QPolygonF polygon;
    polygon << QPointF(leftTopPos.x(), (leftTopPos.y() + bottomRightPos.y()) / 2)
         << QPointF(bottomRightPos.x(), leftTopPos.y())
         << QPointF(bottomRightPos.x(), bottomRightPos.y());
    painter->drawPolygon(polygon);
    polygon.clear();
    polygon << QPointF(bottomRightPos.x(), (leftTopPos.y() + bottomRightPos.y()) / 2)
         << QPointF(leftTopPos.x(), leftTopPos.y())
         << QPointF(leftTopPos.x(), bottomRightPos.y());
    painter->drawPolygon(polygon);
}


#define     RECT_BACK_RATIO     0.8
//#define     RECT_HEGHT   10

int  RadarRectGlowElement::getRectHight(double refer_dis)
{
    int height = refer_dis * 0.1;
//    if(height >= 60) height = 60;
    return height;
}

void RadarRectGlowElement::calculateRect(double& angle, double& length, const ITF_RadarHistoryRect &rect)
{
    //计算起始和结束的两者之间的长边角度差的过度值
    QPoint p1 = mView->framework()->LatLon2Pixel(rect.startlatitude, rect.startlongitude).toPoint();
    QPoint p2 = mView->framework()->LatLon2Pixel(rect.endlatitude, rect.endlongitude).toPoint();
    QPoint sub_p = p2 - p1;
    QVector2D sub(sub_p);
    angle = atan2(sub.y(), sub.x());  //长轴的角度
    if(angle < 0) angle += (GLOB_PI * 2);
    length = sub.length();
//modification on 20191125 for nonsence. every time let the front clean your ass for not correct data
//    //需要对长轴进行修正
//    double distance = ZCHX::Utils::getDistanceDeg(rect.startlatitude, rect.startlongitude, rect.endlatitude, rect.endlongitude);

//    if(distance > mMaxRectLength)
//    {
//        length = length * mMaxRectLength / distance;
//    }
    if(length > mMaxRectLenthAtZoom) length = mMaxRectLenthAtZoom;

    //检查目标的宽度和目标的历史轨迹的像素长度,进行调整,使得轨迹图形看起来是长方形 宽度:长度 = 1:3
    if(length > 0.3333333 * mHistoryPixelLength)
    {
        length = 0.3333333 * mHistoryPixelLength;
    }


}

void RadarRectGlowElement::calculateSkipRect(ITF_RadarHistoryRectList& list, const ITF_RadarHistoryRect &pre, const ITF_RadarHistoryRect &cur)
{
    //检查两者之间的距离是否超出了阈值,如果是,就生成临时的中间图形,否则不生产
    int dis = qRound(ZCHX::Utils::getDistanceDeg(pre.centerlatitude, pre.centerlongitude, cur.centerlatitude, cur.centerlongitude));
    if(dis <= mMaxGapOf2Rect) return;
    double pre_length = ZCHX::Utils::getDistanceDeg(pre.startlatitude, pre.startlongitude, pre.endlatitude,pre.endlongitude);
    double cur_length = ZCHX::Utils::getDistanceDeg(cur.startlatitude, cur.startlongitude, cur.endlatitude,cur.endlongitude);
    double pre_angle  = ZCHX::Utils::calcAzimuth(pre.startlongitude, pre.startlatitude, pre.endlongitude, pre.endlatitude);
    double cur_angle  = ZCHX::Utils::calcAzimuth(cur.startlongitude, cur.startlatitude, cur.endlongitude, cur.endlatitude);
    int num = (dis + mMaxGapOf2Rect - 1) / mMaxGapOf2Rect; //中间的线段数
//    qDebug()<<"dis:"<<dis<<num;
    double unit_dis = dis * 1.0 / num;
    double unit_angle = (cur_angle - pre_angle) /num;
    double unit_lenth = (cur_length - pre_length) /num;
    double unit_cog = (cur.angle - pre.angle)/num;
    double unit_time = (cur.timeOfDay - pre.timeOfDay) / num;
    num --;  //实际的点的个数
    if(num == 0) return;


    //计算方位角
    double azimuth = ZCHX::Utils::calcAzimuth(pre.centerlongitude, pre.centerlatitude, cur.centerlongitude, cur.centerlatitude);
    double src_lat = pre.centerlatitude;
    double src_lon = pre.centerlongitude;
    double angle = pre_angle;
    double length = pre_length;
    double cog = pre.angle + unit_cog;
    double time_of_day = pre.timeOfDay + unit_time;
    for(int i=0; i<num; i++)
    {
        ZCHX::Data::LatLon ll = ZCHX::Utils::distbear_to_latlon(src_lat, src_lon, unit_dis, azimuth);
        //在这里进行随机的经纬度处理,使得目标图形有一定的偏移
        if(1)
        {
            qsrand(QDateTime::currentDateTime().toTime_t());
            ll.lat = ll.lat + 0.000001 * (qrand() % 2 == 0? 1 : (-1));
            ll.lon = ll.lon + 0.000001 * (qrand() % 2 == 0? 1 : (-1));
        }
//        mInsertPointList.append(ll);
        //开始生成模拟的点
        ITF_RadarHistoryRect rect;
        rect.centerlatitude = ll.lat;
        rect.centerlongitude = ll.lon;
        rect.angle = cog;
        rect.diameter = cur.diameter;
        rect.rectNumber = cur.rectNumber;
        rect.isRealData = false;
        rect.timeOfDay = time_of_day;
        angle += unit_angle;
        length += unit_lenth;
        cog += unit_cog;
        time_of_day += unit_time;
        //计算开始点的坐标值
        ZCHX::Data::LatLon start = ZCHX::Utils::distbear_to_latlon(ll.lat, ll.lon, 0.5 * length, 180+angle);
        rect.startlatitude = start.lat;
        rect.startlongitude = start.lon;
        //计算结束点的坐标
        ZCHX::Data::LatLon end = ZCHX::Utils::distbear_to_latlon(ll.lat, ll.lon, 0.5*length, angle);
        rect.endlatitude = end.lat;
        rect.endlongitude = end.lon;
        list.append(rect);
        src_lat = ll.lat;
        src_lon = ll.lon;
    }
}

void  drawTarget(QPainter* painter, const QColor& targetColor, const QColor& edgeColor, const QRect& targetRect, const QTransform& transform, bool round, double angle)
{
    PainterPair chk(painter);
#if 0
    //生成渐变颜色的目标矩形框
    QLinearGradient back_color(targetRect.topLeft(), targetRect.bottomRight());
    back_color.setColorAt(0, Qt::transparent );
    back_color.setColorAt(0.05, edgeColor );
    back_color.setColorAt(1-RECT_BACK_RATIO, targetColor );
    back_color.setColorAt(RECT_BACK_RATIO, targetColor );
    back_color.setColorAt(0.05, edgeColor);
    back_color.setColorAt(1, Qt::transparent );
    painter->setPen(Qt::transparent);
    painter->setBrush(back_color);
#else
    QColor penColor(Qt::darkGray);
    penColor.setAlpha(targetColor.alpha());
    painter->setPen(QPen(penColor, 2));
    painter->setBrush(targetColor);
#endif
    painter->setTransform(transform);
    if(round)
    {
        painter->drawRoundRect(targetRect);

    } else
    {
        painter->drawRect(targetRect);
//        painter->setPen(Qt::white);
//        painter->drawText(QPoint(0, 0), QString::number(angle));
    }
}

void RadarRectGlowElement::drawRadarRect(QPainter *painter,
                                         const QColor& targetColor,
                                         const QColor& edgeColor,
                                         const QColor& backgroundColor,
                                         const ITF_RadarHistoryRect &rect,
                                         bool  first,
                                         QPolygon* side)
{
    //将矩形长轴的坐标转换为屏幕坐标,获取长轴的长度和角度
    double dis = 0.0, angle = 0.0;
    calculateRect(angle, dis, rect);
    QPoint center = mView->framework()->LatLon2Pixel(rect.centerlatitude, rect.centerlongitude).toPoint();


    //开始进行坐标转换
    QTransform transform;
    transform.translate(center.x(), center.y());  //坐标原点移到中心位置
    double rotate = angle *180 / GLOB_PI;
    transform.rotate(rect.angle);

    //目标框的大小设定
    int rect_width = dis;
    int rect_height = getRectHight(dis);
    if(!rect.isRealSize)
    {
        rect_width = rect.referWidth;
        rect_height = rect.referHeight;
    }
//    double height = getRectHight(dis);
//    if(first) height *= 0.5;
    QRect  back_rect(0, 0, rect_width, rect_height);
    back_rect.moveCenter(QPoint(0, 0));

    bool target_draw = false;
//    if(rect.angle >270 || rect.angle < 90)
//    {
//        //方向向上的情况
//        drawTarget(painter, targetColor, edgeColor, back_rect, transform, first, rect.angle);
//        target_draw = true;
//    }

    //开始画背景图形
    if(side)
    {
        PainterPair chk(painter);
        painter->setPen(Qt::transparent);
        painter->setBrush(backgroundColor);
        //开始确定图形的开始边和结束边
        QLine start(back_rect.bottomLeft(), back_rect.bottomRight());
        QLine end(back_rect.topLeft(), back_rect.topRight());
        start = transform.map(start);
        end = transform.map(end);
        QPolygon background = transform.mapToPolygon(back_rect);
        int size = background.size();

        if(side->size() == 2)
        {
            QPolygon p1;
            p1.append(side->at(0));
            p1.append(end.p1());
            p1.append(end.p2());
            p1.append(side->at(1));
            background = p1.united(background);
        }
        side->clear();
        side->append(start.p1());
        side->append(start.p2());
        if(!first)
        {
            painter->drawPolygon(background);
//            painter->setPen(Qt::red);
//            painter->drawLine(start);
        }
    }


    //生成渐变颜色的目标矩形框
    if(!target_draw)drawTarget(painter, targetColor, edgeColor, back_rect, transform, first, rect.angle);
//    QLinearGradient back_color(back_rect.topLeft(), back_rect.bottomRight());
//    back_color.setColorAt(0, Qt::transparent );
//    back_color.setColorAt(0.1, edgeColor );
//    back_color.setColorAt(1-RECT_BACK_RATIO, targetColor );
//    back_color.setColorAt(RECT_BACK_RATIO, targetColor );
//    back_color.setColorAt(0.9, edgeColor);
//    back_color.setColorAt(1, Qt::transparent );
//    painter->setPen(Qt::transparent);
//    painter->setBrush(back_color);

//    //开始画目标框
//    PainterPair chk(painter);
//    painter->setTransform(transform);
//    if(first)
//    {
//        painter->drawRoundRect(back_rect);

//    } else
//    {
//        painter->drawRect(back_rect);
//    }
}

void RadarRectGlowElement::drawRadarTracks(QPainter *painter)
{
    //开始画历史轨迹
//    if(!mRect.HisBlockColor.isValid())
//    {
        mRect.HisBlockColor.setNamedColor(QColor(0, 47, 147, 255).name());
//    }
    if(!mRect.blockColor.isValid())
    {
        mRect.blockColor = Qt::red;

    }
    if(!mRect.blockEdgeColor.isValid())
    {
        mRect.blockEdgeColor = Qt::darkGray;
    }
    //    if(!mRect.historyBackgroundColor.isValid())
    //    {
    mRect.historyBackgroundColor.setRgb(68,89,182);
    //    }

    //计算在当前层级对应的最大矩形长度
    int cur_zoom = mView->framework()->getZoom();
    int max_zoom = mView->framework()->getMaxZoom();
    double max_mct_len = zchxMapDataUtils::calResolution(max_zoom);
    double cur_mct_len = zchxMapDataUtils::calResolution(cur_zoom);
    mMaxRectLenthAtZoom = max_mct_len * mMaxRectLength / cur_mct_len;
    //计算开始和结束点之间的距离
    mHistoryPixelLength = 0.0;
    if(mRect.rects.size() > 0)
    {
        ITF_RadarHistoryRect pre_rect = mRect.rects.last();
        ITF_RadarHistoryRect cur_rect = mRect.current;
        QPoint p1 = mView->framework()->LatLon2Pixel(pre_rect.centerlatitude, pre_rect.centerlongitude).toPoint();
        QPoint p2 = mView->framework()->LatLon2Pixel(cur_rect.centerlatitude, pre_rect.centerlongitude).toPoint();
        QVector2D vec(p1-p2);
        mHistoryPixelLength = vec.length();
    }

    QPolygon line;

    //开始画当前的目标矩形
    drawRadarRect(painter, mRect.blockColor, mRect.blockEdgeColor, mRect.historyBackgroundColor,  mRect.current, true, &line);
    //获取第一个目标的大小
    double cur_length = 0.0, cur_angle = 0.0;
    calculateRect(cur_angle, cur_length, mRect.current);


    //开始画后面的历史轨迹,时间在前的在数组的前面 这里就需要反序遍历
    //需要将目标和背景的颜色进行渐变,直到透明
    int size = mRect.rects.size();
    double delta = 255.0 / size;
    double width_delta = cur_length / size;
//    qDebug()<<"current rect width:"<<cur_length<<" delta:"<<width_delta;

    if(size >= 6 )
    {
        qsrand(QDateTime::currentDateTime().toTime_t());
        for(int i=0; i<size; i++)
        {
            int index = size - 1 - i;
            ITF_RadarHistoryRect& cur = mRect.rects[i];
            cur.isRealSize = false;
            //强制对历史回波轨迹的大小进行赋值
            cur.referWidth = qRound(index * width_delta);
            cur.referHeight = getRectHight(cur.referWidth) * 2;
            int color_step = qRound( index * delta);
            QColor wk_rect = mRect.HisBlockColor;
            wk_rect.setAlpha(color_step);
            QColor wk_edge = mRect.blockEdgeColor;
            wk_edge.setAlpha(color_step);
            QColor wk_back = mRect.historyBackgroundColor;
            wk_back.setAlpha(color_step);
            drawRadarRect(painter, wk_rect, wk_edge, wk_back, cur, false, &line);
        }
    }

    //插入点打印
    {
        foreach (ZCHX::Data::LatLon ll, mInsertPointList) {
            QPoint now = mView->framework()->LatLon2Pixel(ll).toPoint();
            painter->setPen(Qt::black);
            painter->setBrush(Qt::white);
            painter->drawEllipse(now, 5, 5);

        }

        foreach (ZCHX::Data::LatLon ll, mOrigonPointList) {
            QPoint now = mView->framework()->LatLon2Pixel(ll).toPoint();
            painter->setPen(Qt::black);
            painter->setBrush(Qt::red);
            painter->drawEllipse(now, 5, 5);

        }

    }

}

void RadarRectGlowElement::drawElement(QPainter *painter)
{
    if(!painter || !MapLayerMgr::instance()->isLayerVisible(ZCHX::LAYER_RADARRECT) || !mView->framework())
    {
        return;
    }

    if(mView->framework()->getZoom() <= 13) return;

    QMutexLocker locker(&m_mutex);

    if (getIsUpdate())
    {
#if 1
        drawRadarTracks(painter);
#else
        int targetSizeIndex = mView->getTargetSizeIndex();
        int traceLenIndex = mView->getTraceLenIndex();
        int continueTimeIndex = mView->getContinueTimeInde();

        PainterPair chk(painter);
        painter->setPen(QColor(128, 128, 128, 128));
        painter->setBrush(QBrush(QColor(0, 0, 255, 255)));

        QPointF centerPos = mView->framework()->LatLon2Pixel(mRect.centerlatitude, mRect.centerlongitude).toPointF();
        QPointF leftTopPos = mView->framework()->LatLon2Pixel(mRect.centerlatitude + OFFSET_Y / 2, mRect.centerlongitude - OFFSET_X / 2).toPointF();
        QPointF bottomRightPos = mView->framework()->LatLon2Pixel(mRect.centerlatitude - OFFSET_Y / 2, mRect.centerlongitude + OFFSET_X / 2).toPointF();
        QRectF rect(leftTopPos, bottomRightPos);
        painter->drawRect(rect);
//        drawRadarRect(leftTopPos, bottomRightPos, painter);

//        //辐射渐变
//        double radius = (bottomRightPos.x() - leftTopPos.x()) / 2;
//        QRadialGradient radialGradient(QPointF(centerPos.x(), centerPos.y()), radius, QPointF(centerPos.x(), centerPos.y()));
//        radialGradient.setColorAt(0, QColor(0, 255, 0, 255));
//        radialGradient.setColorAt(1, QColor(0, 255, 0, 0));
//        painter->setBrush(radialGradient);
//        painter->drawEllipse(QPointF(centerPos.x(), centerPos.y()), radius, radius);

        //显示设置
        bool showStatus = isRadarDisplayByTargetSize(mRect, targetSizeIndex);
        if (!showStatus)
        {
            return;
        }

        showStatus = isRadarDisplayByTraceLen(mRect, traceLenIndex);
        if (!showStatus)
        {
            return;
        }

        showStatus = isRadarDisplayByContinueTime(mRect, continueTimeIndex);
        if (!showStatus)
        {
            return;
        }

        int hisNum = mRect.rects.size();
        double angle = 0;
        if (hisNum > 2)
        {
            const ITF_RadarHistoryRect & hisRect1 = mRect.rects.at(0);
            const ITF_RadarHistoryRect & hisRect2 = mRect.rects.at(hisNum - 1);

            int dis = ZCHX::Utils::instance()->getDistanceDeg(hisRect1.centerlatitude, hisRect1.centerlongitude, hisRect2.centerlatitude, hisRect2.centerlongitude);
            if (dis < MIN_SHOW_DIS)
            {
                return;
            }

            QPointF centerPos1 = mView->framework()->LatLon2Pixel(hisRect1.centerlatitude, hisRect1.centerlongitude).toPointF();
            QPointF centerPos2 = mView->framework()->LatLon2Pixel(hisRect2.centerlatitude, hisRect2.centerlongitude).toPointF();

            angle = atan2(centerPos2.y() - centerPos1.y(), centerPos2.x() - centerPos1.x()) * 180 / 3.1415926;

            for (int i = 0; i < hisNum; i++)
            {
                int colorDeep = 128 * (hisNum - i ) / hisNum;
                painter->setPen(QColor(128, 128, 128, colorDeep));
                painter->setBrush(QBrush(QColor(0, 0, 255, colorDeep)));

                double centerlatitude = hisRect1.centerlatitude + (hisRect2.centerlatitude - hisRect1.centerlatitude) * i / hisNum;
                double centerlongitude = hisRect1.centerlongitude + (hisRect2.centerlongitude - hisRect1.centerlongitude) * i / hisNum;

                QPointF centerPos = mView->framework()->LatLon2Pixel(centerlatitude, centerlongitude).toPointF();
                QPointF leftTopPos = mView->framework()->LatLon2Pixel(centerlatitude + OFFSET_Y, centerlongitude - OFFSET_X / 2).toPointF();
                QPointF bottomRightPos = mView->framework()->LatLon2Pixel(centerlatitude - OFFSET_Y, centerlongitude + OFFSET_X / 2).toPointF();
                double roomValue = (double)(i + 1) / (hisNum + 1) / 4;
                double leftX = leftTopPos.x() + (bottomRightPos.x() - leftTopPos.x()) * roomValue;
                double leftY = leftTopPos.y() + (bottomRightPos.y() - leftTopPos.y()) * roomValue;
                double rightX = bottomRightPos.x() - (bottomRightPos.x() - leftTopPos.x()) * roomValue;
                double rightY = bottomRightPos.y() - (bottomRightPos.y() - leftTopPos.y()) * roomValue;

                // 旋转
                {
                    PainterPair chk(painter);
//                    painter->translate(centerPos.x(), centerPos.y());
//                    painter->rotate(angle);
                    QRectF rect(QPointF(leftX, leftY), QPointF(rightX, rightY));
                    painter->drawRoundedRect(rect, 5, 5);
//                    painter->translate(-centerPos.x(), -centerPos.y());
                }
            }
        }
        //开始画当前目标回波封闭图形
        if(mRect.blocks.size() > 3)
        {
            PainterPair chk(painter);
            QPolygon poly;
            QList<VideoData> list;
            for(int i=0; i<mRect.blocks.size(); i++)
            {
                ZCHX::Data::LatLon ll = mRect.blocks[i];
                QPointF curPos = mView->framework()->LatLon2Pixel(ll.lat, ll.lon).toPointF();
                double dx = curPos.x() - centerPos.x();
                double dy = curPos.y() - centerPos.y();
                double dis = dx * dx + dy * dy;
                double rad = atan2(dy, dx);
                VideoData data;
                data.len = dis;
                data.rad = rad;
                data.ll = curPos;
                list.append(data);
                poly.append(mView->framework()->LatLon2Pixel(ll.lat, ll.lon).toPoint());
            }
            if(mRect.blockColor.isValid())
            {
                painter->setBrush(QBrush(mRect.blockColor));
            } else
            {
                painter->setBrush(QColor("#FFEC8B"));
            }

            if(0){
                for(int i=0; i<poly.size(); i++)
                {
                    QRect rect(0, 0, 5, 5);
                    rect.moveCenter(poly[i]);
                    painter->drawRect(rect);
                }

                for(int i=0; i<poly.size(); i++)
                {
                    QRect rect(0, 0, 5, 5);
                    rect.moveCenter(poly[i]);
                    painter->setPen(Qt::black);
                    painter->drawText(poly[i], QString::number(i+1));
                }
            } else
            {
                poly.clear();
                if(list.size() > 0)
                {
                    std::stable_sort(list.begin(), list.end());
                    foreach (VideoData data, list) {
                        poly.append(data.ll.toPoint());
                    }
                }
                QMap<int, QList<PolygonPnt>> map = pointLines(poly);
                foreach (QList<PolygonPnt> list, map.values())
                {
                    for(int i=0; i<list.size(); i++)
                    {
                        QRect rect(0, 0, 5, 5);
                        rect.moveCenter(list[i].mData);
                        painter->drawRect(rect);
                        painter->setPen(Qt::black);
                        painter->drawText(list[i].mData, QString("%1-%2").arg(list[i].line).arg(i+1));
                    }
                }

//                for(int i=0; i<poly.size(); i++)
//                {
//                    QRect rect(0, 0, 5, 5);
//                    rect.moveCenter(poly[i]);
//                    painter->setPen(Qt::black);
//                    painter->drawText(poly[i], QString::number(i+1));
//                }
//                QColor brush = painter->brush().color();
//                brush.setAlpha(50);
//                painter->setBrush(brush);

//                poly = convertToPolygon(poly);
//                painter->drawPolygon(poly);
            }
        }

//        for (int i = 1; i < hisNum && false; i++)
//        {
//            int colorDeep = 128 * (hisNum - i ) / hisNum;
//            painter->setBrush(QBrush(QColor(0, 0, 255, colorDeep)));
//            const ITF_RadarHistoryRect & hisRect = mRect.rects.at(i);

//            QPointF centerPos = mView->framework()->LatLon2Pixel(hisRect.centerlatitude, hisRect.centerlongitude).toPointF();
//            QPointF leftTopPos = mView->framework()->LatLon2Pixel(hisRect.centerlatitude + OFFSET_Y, hisRect.centerlongitude - OFFSET_X / 2).toPointF();
//            QPointF bottomRightPos = mView->framework()->LatLon2Pixel(hisRect.centerlatitude - OFFSET_Y, hisRect.centerlongitude + OFFSET_X / 2).toPointF();
//            double leftX = leftTopPos.x() + (bottomRightPos.x() - leftTopPos.x()) * (i + 1) / (hisNum + 1) / 2;
//            double leftY = leftTopPos.y() + (bottomRightPos.y() - leftTopPos.y()) * (i + 1) / (hisNum + 1) / 2;
//            double rightX = bottomRightPos.x() - (bottomRightPos.x() - leftTopPos.x()) * (i + 1) / (hisNum + 1) / 2;
//            double rightY = bottomRightPos.y() - (bottomRightPos.y() - leftTopPos.y()) * (i + 1) / (hisNum + 1) / 2;

////            if (bottomRightPos.x() - leftTopPos.x() > 50)
////            {
////                qDebug() << "aaaaaaaaaaaaa" << leftTopPos.x() << leftTopPos.y() << bottomRightPos.x() << bottomRightPos.y();
////                qDebug() << "hhhhhhhhhhh" << mRect.rectNumber << i << leftX << leftY << rightX << rightY << colorDeep;
////            }
////            drawRadarRect(QPointF(leftX, leftY), QPointF(rightX, rightY), painter);

//            // 旋转
//            {
//                PainterPair chk(painter);
//                painter->translate(centerPos.x(), centerPos.y());
//                painter->rotate(angle);
//                QRectF rect(QPointF(leftX - centerPos.x(), leftY - centerPos.y()), QPointF(rightX - centerPos.x(), rightY - centerPos.y()));
//                painter->drawRect(rect);
//                painter->translate(-centerPos.x(), -centerPos.y());
//            }

////            //辐射渐变
////            double radius = (bottomRightPos.x() - leftTopPos.x()) / 2;
////            QRadialGradient radialGradient(QPointF(centerPos.x(), centerPos.y()), radius, QPointF(centerPos.x(), centerPos.y()));
////            radialGradient.setColorAt(0, QColor(0, 0, 255, colorDeep));
////            radialGradient.setColorAt(1, QColor(0, 0, 255, 0));
////            painter->setBrush(radialGradient);
////            painter->drawEllipse(QPointF(centerPos.x(), centerPos.y()), radius, radius);

//        }

//        // 链接轨迹线，测试用
//        QList<QPointF> points;
//        for (int i = 1; i < hisNum; i++)
//        {
//            const ITF_RadarHistoryRect & hisRect = mRect.rects.at(i);
//            QPointF centerPos = mView->framework()->LatLon2Pixel(hisRect.centerlatitude, hisRect.centerlongitude).toPointF();
//            points.append(centerPos);
//        }
//        painter->setPen(Qt::SolidLine);
//        painter->drawPolyline(points.toVector());

#endif
    }

}

bool RadarRectGlowElement::isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarRect &data,
                                                      int targetSizeIndex)
{
    if (targetSizeIndex == 0)
    {
        return true;
    }

    int diameter = data.current.diameter;

    switch (targetSizeIndex)
    {
    case 1:
        if (diameter >= 200 && diameter < 300)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    case 2:
        if (diameter >= 100 && diameter < 200)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    case 3:
        if (diameter >= 50 && diameter < 100)
        {
            return true;
        }
        else
        {
            return false;
        }
        break;
    default:
        break;
    }

    return false;
}

bool RadarRectGlowElement::isRadarDisplayByTraceLen(const ZCHX::Data::ITF_RadarRect &data,
                                                    int traceLenIndex)
{
    if (traceLenIndex == 0)
    {
        return true;
    }

    if (data.rects.size() < 2)
    {
        return true;
    }

    int traceLen = ZCHX::Utils::instance()->getDistanceDeg(data.rects.at(0).centerlatitude,
                                                           data.rects.at(0).centerlongitude,
                                                           data.rects.at(data.rects.size() - 1).centerlatitude,
                                                           data.rects.at(data.rects.size() - 1).centerlongitude);

    switch (traceLenIndex)
    {
    case 1:
        if (traceLen >= 200 && traceLen < 300)
        {
           return true;
        }
        else
        {
            return false;
        }
    case 2:
        if (traceLen >= 100 && traceLen < 200)
        {
           return true;
        }
        else
        {
            return false;
        }
    case 3:
        if (traceLen >= 50 && traceLen < 100)
        {
           return true;
        }
        else
        {
            return false;
        }
    case 4:
        if (traceLen < 50)
        {
           return true;
        }
        else
        {
            return false;
        }
    default:
        return false;
    }
}

bool RadarRectGlowElement::isRadarDisplayByContinueTime(const ZCHX::Data::ITF_RadarRect &data,
                                                        int continueTimeIndex)
{
    if (continueTimeIndex == 0)
    {
        return true;
    }

    if (data.rects.size() < 2)
    {
        return true;
    }

    int continueTime = data.rects.at(0).timeOfDay -
            data.rects.at(data.rects.size() - 1).timeOfDay;

    if (continueTime >= 5 && continueTime <= 30)
    {
        return true;
    }
    else
    {
        return false;
    }
}
}

