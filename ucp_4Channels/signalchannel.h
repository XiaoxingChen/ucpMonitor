#ifndef SIGNALCHANNEL_H
#define SIGNALCHANNEL_H

#include <QWidget>
class QCheckBox;
class QDoubleSpinBox;
class QSlider;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QwtPlot;
class QwtPlotCurve;
class ScreenPlot;
class DataPool;
class CComCalier;

/*********************************************************
*@brief: this part for SigDataF
*********************************************************/
class SigDataF
{
public:
    explicit SigDataF();
    SigDataF(const SigDataF&);
    qreal y_offset_;
    qreal y_gain_;

    void append_points(const QPolygonF&);
    void set_offset_(qreal offset);
    void set_gain_(qreal gain);

    void clear()
    {
        dispPoints_.clear();
    }
    void pop_front()
    {
        dispPoints_.pop_front();
    }
    const QPolygonF& dispPoints()
    {
          return dispPoints_;
    }
    QPolygonF rawPoints() const;
    QPointF rawPoints_at(int i) const;

protected:

    QPolygonF scaling_PolygonF(const QPolygonF&, qreal gain) const;
    void scaling_PolygonF(QPolygonF&, qreal gain) const;
    QPolygonF dispPoints_;
};
/*********************************************************
*@brief: this part for SignalChannel
*********************************************************/
class SignalChannel : public QWidget
{
    Q_OBJECT
public:
    explicit SignalChannel(QWidget *parent = 0, ScreenPlot* pScreenPlot = NULL);
    SignalChannel(const SignalChannel&);

//    void attach_screenPlot(ScreenPlot* pScreenPlot);
    void attach_pointspool(DataPool*);
    void attach_Calibrator(CComCalier*);
    void Update_Points();

    SigDataF Points_Queue;

    QwtPlotCurve* Curve;

    void set_channelNum(uint8_t ch);
    void set_Gain(qreal);
    void set_dataIdx(uint8_t);
    uint8_t get_channelNum() const;
    const QComboBox* get_SelectVar_ComboBox() const;
private:
    QCheckBox* EnableCh_CheckBox;
    QLabel* Enable_Label;
    QLabel* SetGain_Label;
    QComboBox* SelectVariable_ComboBox;
    QDoubleSpinBox* SetGain_SpinBox;
    QPushButton* Setting_PushButton;
    QSlider* SetOffset_Slider;
    QLabel* SetOffset_Label;
    QLabel* CurrenValue_Label;
    QGroupBox* GroupBox;
    QwtPlot* Wave_Plot;
    /* need attach */
    CComCalier* Calibrator;
    DataPool* Points_Pool;

    bool* UpdatePoints_Flag;
    uint8_t channelNum_;

signals:
    void send_ucp_pack(uint8_t, uint16_t);

public slots:
    void sync_dataVector(std::vector<uint16_t>);

private slots:
    void Change_SetOffset(int);
    void Change_EnableCheckBox();
    void Change_SetGain(double);
    void on_varSelect_changed(int);
};

#endif // SIGNALCHANNEL_H
