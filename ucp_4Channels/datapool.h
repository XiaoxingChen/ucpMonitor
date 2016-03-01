#ifndef DATAPOOL_H
#define DATAPOOL_H
#include <QPolygonF>
class QTime;
class QPointF;
class QMutex;
class DataPool
{
public:
    DataPool();
    ~DataPool();
    QPointF GetPoint(int);
    QPolygonF GetPoints(int);
    void WritePoints(uint8_t, QPointF);
    qreal GetTimeStampF(void) const;

private:
    QVector<QPolygonF> DataBuf_CH;
    QMutex* mutex;
    qreal timeStamp;
    QTime* time_ms;
};

#endif // DATAPOOL_H
