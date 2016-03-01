#include "signalchannel.h"
#include <qcheckbox.h>
#include <qslider.h>
#include <QDoubleSpinBox>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "screenplot.h"
#include "datapool.h"
#include <iostream>
#include <qabstractitemview.h>
#include "ccomcalier.h"
#include "caliVar.h"

SignalChannel::SignalChannel(QWidget *parent, ScreenPlot* pScreenPlot) :
    QWidget(parent),
    Wave_Plot(pScreenPlot->Wave_Plot),
    Calibrator(NULL),
    Points_Pool(NULL),
    UpdatePoints_Flag(&pScreenPlot->UpdatePoints_Flag)

{
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Minimum);

    if(Wave_Plot == NULL)
    {
        std::cout << "NULL Wave_Plot" << std::endl;
        while(1);
    }

    GroupBox = new QGroupBox("- CH1 -");
    EnableCh_CheckBox = new QCheckBox;
    EnableCh_CheckBox->setChecked(true);
    EnableCh_CheckBox->setSizePolicy(policy);

    Enable_Label = new QLabel("show");
    Enable_Label->setAlignment(Qt::AlignLeft);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    Enable_Label->setSizePolicy(policy);
    Enable_Label->setMaximumWidth(30);

    SetGain_SpinBox = new QDoubleSpinBox;
    SetGain_SpinBox->setValue(1);
    SetGain_SpinBox->setRange(0, 1000);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    SetGain_SpinBox->setSizePolicy(policy);
    SetGain_SpinBox->setMaximumWidth(50);
    SetGain_Label = new QLabel("Div");

    SetOffset_Slider = new QSlider;
    SetOffset_Slider->setRange(-3000,6000);
    SetOffset_Slider->setValue(0);
    SetOffset_Label = new QLabel("offset: \n+0");
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    SetOffset_Label->setSizePolicy(policy);
    SetOffset_Label->setAlignment(Qt::AlignHCenter);

    /* current value */
    CurrenValue_Label = new QLabel("value: \n+0");
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    CurrenValue_Label->setSizePolicy(policy);
    CurrenValue_Label->setAlignment(Qt::AlignHCenter);

    Setting_PushButton = new QPushButton(tr("Setting"));
    Setting_PushButton->setMaximumWidth(50);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    Setting_PushButton->setSizePolicy(policy);

    SelectVariable_ComboBox = new QComboBox;
//    SelectVariable_ComboBox->addItem("Idx0");
//    SelectVariable_ComboBox->addItem("Idx1");
//    SelectVariable_ComboBox->addItem("Idx2");
//    SelectVariable_ComboBox->addItem("Idx3");
//    SelectVariable_ComboBox->addItem("Cpu Usage");
//    SelectVariable_ComboBox->addItem("Pos of Motor");
//    SelectVariable_ComboBox->addItem("Vel of Motor");
//    SelectVariable_ComboBox->addItem("Acc of Motor");
//    SelectVariable_ComboBox->addItem("Cur of Coil");
//    SelectVariable_ComboBox->addItem("PWM Duty");
//    SelectVariable_ComboBox->addItem("Dir of Rotate");
//    SelectVariable_ComboBox->addItem("Mode of Control");
//    SelectVariable_ComboBox->addItem("Target Value");
//    SelectVariable_ComboBox->addItem("Kp of vControl");
//    SelectVariable_ComboBox->addItem("Ki of vControl");
//    SelectVariable_ComboBox->addItem("Kd of vControl");
    for(int i = 0; i < clv::VAR_NUM; i ++)
    {
        SelectVariable_ComboBox->addItem(clv::IdxText[i]);
    }

    SelectVariable_ComboBox->setMaximumWidth(50);
    SelectVariable_ComboBox->view()->setMinimumWidth(150);
    policy.setHorizontalPolicy(QSizePolicy::Fixed);
    SelectVariable_ComboBox->setSizePolicy(policy);

    QVBoxLayout* GroupBoxLayout = new QVBoxLayout;
    QVBoxLayout* mainLayout = new QVBoxLayout;

    QHBoxLayout* Enable_Layout = new QHBoxLayout;
    QHBoxLayout* SetGain_Layout = new QHBoxLayout;

    Enable_Layout->addWidget(EnableCh_CheckBox);
    Enable_Layout->addWidget(Enable_Label);

    SetGain_Layout->addWidget(SetGain_SpinBox);
//    SetGain_Layout->addWidget(SetGain_Label);

    /* set points */
//    Points_Queue = new QPolygonF;

    /* set curve */
    Curve = new QwtPlotCurve;
    Curve->setPen(Qt::red, 1.5);
    Curve->setSamples(Points_Queue.dispPoints());
    Curve->setRenderHint(QwtPlotItem::RenderAntialiased,true);
    Curve->attach(Wave_Plot);

    mainLayout->addWidget(CurrenValue_Label);
    mainLayout->addWidget(SetOffset_Slider, 0, Qt::AlignHCenter);
    mainLayout->addWidget(SetOffset_Label);
    mainLayout->addWidget(SelectVariable_ComboBox);
    mainLayout->addLayout(Enable_Layout);
    mainLayout->addLayout(SetGain_Layout);
//    mainLayout->addWidget(Setting_PushButton);

    GroupBox->setLayout(mainLayout);
    GroupBoxLayout->addWidget(GroupBox);
    GroupBoxLayout->setMargin(0);
    GroupBoxLayout->setSpacing(0);

    setLayout(GroupBoxLayout);
    this->setMaximumWidth(70);

    connect(SetOffset_Slider, SIGNAL(valueChanged(int)), this, SLOT(Change_SetOffset(int)));
    connect(SetGain_SpinBox, SIGNAL(valueChanged(double)), this, SLOT(Change_SetGain(double)));
    connect(EnableCh_CheckBox, SIGNAL(clicked()), this, SLOT(Change_EnableCheckBox()));
    connect(SelectVariable_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_varSelect_changed(int)));
//    connect(Setting_PushButton, SIGNAL(clicked()), SelectVariable_ComboBox, SLOT(show()));
}

//
// void Change_EnableCheckBox()
//
void SignalChannel::Change_EnableCheckBox()
{
    if(EnableCh_CheckBox->isChecked())
    {
        Curve->show();
    }
    else
    {
        Curve->hide();
    }
}

//
//
//
void SignalChannel::sync_dataVector(std::vector<uint16_t> datav)
{
//    std::cout << "CH: " << (int)channelNum_ << " data: " << datav[channelNum_-1] <<std::endl;
    SelectVariable_ComboBox->setCurrentIndex(datav[channelNum_-1]);
}

//
// void Change_SetOffset(int value)
//
void SignalChannel::Change_SetOffset(int value)
{
    QString str;
    QString temp;

    /* change Label */
    str = "offset: \n";
    if(value >= 0)
    {
        temp = QString::number(value, 10);
        str += "+";
    }
    else
    {
        temp = QString::number(-value, 10);
        str += "-";
    }
    str += temp;
    SetOffset_Label->setText(str);

    /* change curve */
    Points_Queue.set_offset_(value);

}

//
// void Change_SetGain(double gain)
//
void SignalChannel::Change_SetGain(double gain)
{
    Points_Queue.set_gain_(gain);
}

//
//void on_varSelect_changed(int)
//
void SignalChannel::on_varSelect_changed(int idx)
{
    emit send_ucp_pack(channelNum_-1, (uint16_t)idx);
}

//
//void attach_pointspool(DataPool * pPool);
//
void SignalChannel::attach_pointspool(DataPool * pPool)
{
    Points_Pool = pPool;
}

//
//void attach_Calibrator(CComCalier *)
//
void SignalChannel::attach_Calibrator(CComCalier * pCalier)
{
    Calibrator = pCalier;
    connect(this, SIGNAL(send_ucp_pack(uint8_t, uint16_t)), Calibrator, SLOT(send_ucp_pack(uint8_t, uint16_t)));
    connect(Calibrator, SIGNAL(sync_dataVector(std::vector<uint16_t>)), this, SLOT(sync_dataVector(std::vector<uint16_t>)));
}

//
// void Update_Points()
//
void SignalChannel::Update_Points()
{
    if(Points_Pool == NULL)
    {
        std::cout << "NULL Points_Pool" << std::endl;
        return;
    }
    if(*UpdatePoints_Flag)
    {
        QPolygonF temp;
        temp = Points_Pool->GetPoints(channelNum_-1);
//        Points_Queue.append_trans(temp);
        Points_Queue.append_points(temp);

#if 0
        if(Points_Queue.points().size() > 0 && (channelNum_-1) == 0)
        {
            QPointF point = (Points_Queue.points())[Points_Queue.points().size()-1];
            std::cout << "signalchannel: "<<"t = "<<point.x()<< " y = "<< point.y()<<std::endl;
        }
#endif
        if(temp.size() > 0)
        {
            QPointF point = temp[temp.size()-1];
//            std::cout << "t = "<<point.x()<< " y = "<< point.y()<<std::endl;
            /* change Label */
            QString str;
            str = "value: \n";
            if(point.y() >= 0)
                str+= "+";
            str += QString::number((int32_t)point.y(), 10);
            CurrenValue_Label->setText(str);

        }

        if(Points_Queue.dispPoints().size() > 300)
        {
            Points_Queue.pop_front();
        }
    }
    Curve->setSamples(Points_Queue.dispPoints());
}

//
// void set_channelNum(uint8_t ch)
//
void SignalChannel::set_channelNum(uint8_t ch)
{
    if((ch > 0) && (ch < 5))
        channelNum_ = ch;
    switch(ch)
    {
    case 1:
        GroupBox->setTitle("- CH1 -");
        Curve->setPen(Qt::red, 1.5);
        break;
    case 2:
        GroupBox->setTitle("- CH2 -");
        Curve->setPen(Qt::yellow, 1.5);
        break;
    case 3:
        GroupBox->setTitle("- CH3 -");
        Curve->setPen(Qt::green, 1.5);
        break;
    case 4:
        GroupBox->setTitle("- CH4 -");
        Curve->setPen(Qt::blue, 1.5);
        break;
    default:
        break;
    }
}
//
//void SignalChannel::set_Gain(qreal value)
//
void SignalChannel::set_Gain(qreal value)
{
    SetGain_SpinBox->setValue(value);
}

//
//void SignalChannel::set_dataIdx(uint8_t idx)
//
void SignalChannel::set_dataIdx(uint8_t idx)
{
    SelectVariable_ComboBox->setCurrentIndex(idx);
}

//
// void set_channelNum(uint8_t ch)
//
uint8_t SignalChannel::get_channelNum() const
{
    return channelNum_;
}

//
//const QComboBox* SignalChannel::get_SelectVar_ComboBox() const
//
const QComboBox* SignalChannel::get_SelectVar_ComboBox() const
{
    return SelectVariable_ComboBox;
}
/*********************************************************
*@brief: this part for SigDataF
*********************************************************/
SigDataF::SigDataF():
    y_offset_(0),
    y_gain_(1)
{

}

//
//SigDataF::SigDataF(const SigDataF& value):
// constructor
//
SigDataF::SigDataF(const SigDataF& value):
    y_offset_(value.y_offset_),
    y_gain_(value.y_gain_),
    dispPoints_(value.dispPoints_)
{
}

/********************************************************************
 * void SigDataF::append_points(const QPolygonF& points)
 * @brief change the input points according to y_gain_ and y_offset_
 *      and then append to the end of dispPoints_.
 * @param points
 *******************************************************************/
void SigDataF::append_points(const QPolygonF& points)
{
    QPolygonF temp;
    temp = scaling_PolygonF(points, y_gain_);
    temp.translate(0, y_offset_);
    dispPoints_ << temp;
}

/********************************************************************
 * void SigDataF::append_trans(const QPointF& point)
 * @brief change the input point according to y_gain_ and y_offset_
 *      and then append to the end of dispPoints_.
 * @param point
 *******************************************************************/
//void SigDataF::append_trans(const QPointF& point)
//{
//    QPointF temp(point);
//    temp.setY(point.y()*y_gain_+y_offset_);
//    dispPoints_.append(temp);
//}

/********************************************************************
 * QPolygonF SigDataF::scaling_PolygonF(const QPolygonF& points, qreal gain)
 * @brief change the amplitude of the input QPolygonF points
 * @param points
 *******************************************************************/
QPolygonF SigDataF::scaling_PolygonF(const QPolygonF& points, qreal gain) const
{
    QPolygonF temp(points);
    if (gain == 0.0)
        return temp;

    register QPointF *p = temp.data();
    register int i = temp.size();

    while (i--) {
        p->setY(p->y()*gain);
        ++p;
    }
    return temp;
}

/********************************************************************
 * void SigDataF::scaling_PolygonF(QPolygonF& points, qreal gain)
 * @brief directly change the amplitude of "dispPoints_"
 * @param gain
 *******************************************************************/
void SigDataF::scaling_PolygonF(QPolygonF& points, qreal gain) const
{
    if (gain == 0.0)
        return ;

    register QPointF *p = points.data();
    register int i = points.size();

    while (i--) {
        p->setY(p->y()*gain);
        ++p;
    }
}

/********************************************************************
 * void SigDataF::set_offset_(qreal offset)
 * @brief change the y offset of "dispPoints_"
 * @param offset
 *******************************************************************/
void SigDataF::set_offset_(qreal offset)
{
    dispPoints_.translate(0, offset-y_offset_);
    y_offset_ = offset;
}

/********************************************************************
 * void SigDataF::set_gain_(qreal gain)
 * @brief change the signal gain of "dispPoints_",
 *      but keep the signal offset unchanged.
 * @param gain
 *******************************************************************/
void SigDataF::set_gain_(qreal gain)
{
    if(gain == 0.0)
        return;

    dispPoints_.translate(0, -y_offset_);
    scaling_PolygonF(dispPoints_, gain/y_gain_);
    dispPoints_.translate(0, y_offset_);

    y_gain_ = gain;
}

//
//QPolygonF SigDataF::rawPoints()
//
QPolygonF SigDataF::rawPoints() const
{
    QPolygonF temp(dispPoints_);
    temp.translate(0, -y_offset_);
    scaling_PolygonF(temp, 1/y_gain_);

    return temp;
}

//
//QPointF rawPoints_at(int i) const;
//
QPointF SigDataF::rawPoints_at(int i) const
{
    QPoint temp;
    temp.setX(dispPoints_.at(i).x());
    temp.setY((dispPoints_.at(i).y() - y_offset_)/y_gain_);

    return temp;
}

//end of file
