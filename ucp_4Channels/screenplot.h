#ifndef ScreenPlot_H
#define ScreenPlot_H

#include <QWidget>
#include <qvector.h>
class QPushButton;
class QScrollBar;
class QwtPlot;
class QTimer;
class SignalChannel;
class DataPool;
class QDoubleSpinBox;
class QLineEdit;
class QFileDialog;

class ScreenPlot : public QWidget
{
    Q_OBJECT
    friend class SignalChannel;
public:
    explicit ScreenPlot(QWidget *parent = 0);
    DataPool* DataPool1;
    void attach_DataPool(DataPool*);
    void attach_SignalChannel(SignalChannel*);
    void Update_Timestamp();

private:
    QPushButton* PauseUpdatePoints_PushButton;
    QDoubleSpinBox* TimeScaling_SpinBox;
    QScrollBar* SetOffset_ScrollBar;
    QTimer* Replot_Timer;
    QwtPlot* Wave_Plot;
    bool UpdatePoints_Flag;
    SignalChannel* SigCH1;
    SignalChannel* SigCH2;
    SignalChannel* SigCH3;
    SignalChannel* SigCH4;
    qreal latestTimestamp;
    QFileDialog* selectDir_FileDialog;
    QPushButton* saveData_PushButton;
    bool fout_SigCH_xy(std::ofstream&, SignalChannel*, uint16_t);

signals:
    
public slots:
private slots:
    void Update_WavePlot();
    void Change_UpdatePointsFlag();
    void on_saveDir_selected(QString);
    
};

#endif // ScreenPlot_H
