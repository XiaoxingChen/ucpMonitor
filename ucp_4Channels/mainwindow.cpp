#include "mainwindow.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>
#include "signalchannel.h"
#include "ScreenPlot.h"
#include "ccomgroup.h"
#include "ccomcalier.h"
#include "datapool.h"
#include "cmotorparam.h"
#include <qcombobox.h>
#include "caliVar.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QSizePolicy policy;

    /* pushbutton */
    Quit_PushButton = new QPushButton("quit");
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    Quit_PushButton->setSizePolicy(policy);

    /* set screen */
    Screen = new ScreenPlot;
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    Screen->setSizePolicy(policy);

    /* datapool */
    data_Pool = new DataPool;
    Screen->attach_DataPool(data_Pool);

    /* com group */
    com_Group = new CComGroup;

    /* com calier */
    com_Calier = new CComCalier;
    com_Calier->attach_UartConfig(com_Group);
    com_Calier->attach_DataPool(data_Pool);

    /* motor parameters */
    motorParam = new CMotorParam;
    motorParam->attach_calibrator(com_Calier);

    /* set SigCH */
    SigCH1 = new SignalChannel(0, Screen);
    SigCH1->set_channelNum(1);
    SigCH1->attach_pointspool(data_Pool);
    SigCH1->attach_Calibrator(com_Calier);
    SigCH1->set_dataIdx(clv::MOTORVEL);
    SigCH1->set_Gain(20);

    SigCH2 = new SignalChannel(0, Screen);
    SigCH2->set_channelNum(2);
    SigCH2->attach_pointspool(data_Pool);
    SigCH2->attach_Calibrator(com_Calier);
    SigCH2->set_dataIdx(clv::PWMDUTY);
    SigCH2->set_Gain(0.5);

    SigCH3 = new SignalChannel(0, Screen);
    SigCH3->set_channelNum(3);
    SigCH3->attach_pointspool(data_Pool);
    SigCH3->attach_Calibrator(com_Calier);
    SigCH3->set_dataIdx(clv::TARVAL);
    SigCH3->set_Gain(20);

    SigCH4 = new SignalChannel(0, Screen);
    SigCH4->set_channelNum(4);
    SigCH4->attach_pointspool(data_Pool);
    SigCH4->attach_Calibrator(com_Calier);
    SigCH4->set_dataIdx(clv::COILCUR);

    Screen->attach_SignalChannel(SigCH1);
    Screen->attach_SignalChannel(SigCH2);
    Screen->attach_SignalChannel(SigCH3);
    Screen->attach_SignalChannel(SigCH4);

    /* set layout */
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* sigCHLayout = new QHBoxLayout;
    QVBoxLayout* subLayout_l1 = new QVBoxLayout;
    QVBoxLayout* subLayout_r1 = new QVBoxLayout;

    subLayout_l1->addWidget(Quit_PushButton);

    sigCHLayout->addWidget(SigCH1);
    sigCHLayout->addWidget(SigCH2);
    sigCHLayout->addWidget(SigCH3);
    sigCHLayout->addWidget(SigCH4);

    subLayout_r1->addWidget(com_Group);
    subLayout_r1->addWidget(motorParam);
    subLayout_r1->addLayout(sigCHLayout);

    mainLayout->addLayout(subLayout_l1);
    mainLayout->addWidget(Screen);
    mainLayout->addLayout(subLayout_r1);
    mainLayout->addWidget(com_Calier);

    setLayout(mainLayout);

    connect(Quit_PushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(com_Group, SIGNAL(connect_clicked()),
            com_Calier, SLOT(on_UartConnect_clicked()));
}
MainWindow::~MainWindow()
{

}
//end of file
