#include "datapool.h"
#include <QTime>
#include <QPointF>
#include <qmath.h>
#include <qmutex.h>
#include <iostream>
DataPool::DataPool():
    DataBuf_CH(4)
{
    mutex = new QMutex;
    time_ms = new QTime;
    time_ms->start();

}

//
//destructor
//
DataPool::~DataPool()
{
    delete mutex;
}

//
// QPointF GetPoint(int)
//
QPointF DataPool::GetPoint(int CH)
{
    qreal t = (qreal)time_ms->elapsed()/1000;
    if(CH == 1)
    {
        return QPointF(t, 3*sin(2*t));
    }
    else if(CH == 2)
    {
        return QPointF(t, 1.2*sin(1.5*t)+sin(3*t-1));
    }
    else if(CH == 3)
    {
        return QPointF(t, 0.5*sin(t)+cos(10*t-1));
    }
    else if(CH == 4)
    {
        return QPointF(t, cos(3*t-1));
    }
    else
    {
        return QPointF(t, 0);
    }
}

//
// QPolygonF GetPoints(int)
//
QPolygonF DataPool::GetPoints(int ch)
{
    QPolygonF temp;
    mutex->lock();
    temp = DataBuf_CH[ch];
    DataBuf_CH[ch].clear();
    mutex->unlock();
    return temp;
}
qreal DataPool::GetTimeStampF() const
{
//    return (qreal)time_ms->elapsed();
    return timeStamp;
}

//
// WritePoints
//
void DataPool::WritePoints(uint8_t ch, QPointF value)
{
    mutex->lock();
    DataBuf_CH[ch].push_back(value);
    timeStamp = value.x();
//    if(ch == 0)
//        std::cout << "x = " <<value.x() << " y = " << value.y() <<std::endl;
    mutex->unlock();
}
