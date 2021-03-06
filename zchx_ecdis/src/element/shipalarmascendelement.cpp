#include "shipalarmascendelement.h"
#include "map_layer/zchxmaplayermgr.h"
#include "zchxmapframe.h"
#include "zchxmapwidget.h"

namespace qt {

ShipAlarmAscendElement::ShipAlarmAscendElement(const ZCHX::Data::ITF_ShipAlarmAscend &ele, zchxMapWidget* f)
    :Element(ele.lat, ele.lon, f, ZCHX::Data::ELE_SHIP_ALARM_ASCEND)
{
    setData(ele);
    setIsUpdate(true);
}

void ShipAlarmAscendElement::setData(const ZCHX::Data::ITF_ShipAlarmAscend &ele)
{
    m_data = ele;
    setIsUpdate(true);
}

ZCHX::Data::ITF_ShipAlarmAscend ShipAlarmAscendElement::data() const
{
    return m_data;
}

std::string ShipAlarmAscendElement::name() const
{
    return m_data.getName().toStdString();
}
void ShipAlarmAscendElement::drawElement(QPainter *painter)
{
    if(!painter || !mView->getLayerMgr()->isLayerVisible(ZCHX::LAYER_ALARMASCEND)) return;
    QPointF point = mView->framework()->LatLon2Pixel(m_data.lat, m_data.lon).toPointF();

    if (m_data.alarmType > 0)
    {
        QPixmap pixmap(ZCHX::Utils::getAlarmImg(m_data.alarmType));
        painter->drawPixmap(point.x() - pixmap.width() / 2, point.y() - pixmap.height() / 2,
                            pixmap.width(), pixmap.height(), pixmap);
    }
}
}

