#include "radarrectelement.h"
#include "zchxmapframe.h"
#include "map_layer/zchxmaplayermgr.h"
#include <QPainter>
#include "zchxutils.hpp"

#define OFFSET_X 0.00014
#define OFFSET_Y 0.00014
#define MIN_SHOW_DIS 20

using namespace ZCHX::Data;

namespace qt {
RadarRectGlowElement::RadarRectGlowElement(const ZCHX::Data::ITF_RadarRect& data, zchxMapWidget* frame)
    :Element(data.centerlatitude, data.centerlongitude, frame, ZCHX::Data::ELE_RADAR_RECTGLOW)
{
    setData(data);
}

const ITF_RadarRect &RadarRectGlowElement::data() const
{
    return mRect;
}

void RadarRectGlowElement::setData(const ZCHX::Data::ITF_RadarRect &rect)
{
    mRect = rect;
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

void RadarRectGlowElement::drawElement(QPainter *painter)
{
    if(!painter || !MapLayerMgr::instance()->isLayerVisible(ZCHX::LAYER_RADARRECT) || !mView->framework())
    {
        return;
    }

    if (getIsUpdate())
    {
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
    }
}

bool RadarRectGlowElement::isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarRect &data,
                                                      int targetSizeIndex)
{
    if (targetSizeIndex == 0)
    {
        return true;
    }

    int diameter = data.diameter;

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

