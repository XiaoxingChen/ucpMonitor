/*******************************************************************************
* release information
* 2016/02/19 chenxx add pctrl parameter update in sync_dataVector()
*******************************************************************************/
#include "cmotorparam.h"
#include "ccomcalier.h"
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <iostream>
#include "caliVar.h"


CMotorParam::CMotorParam(QWidget *parent) :
    QWidget(parent), Calibrator(NULL)
{
    main_GroupBox = new QGroupBox;
    main_GroupBox->setTitle(tr("Motor Parameters"));

    selectMode_ComboBox = new QComboBox;
    selectMode_ComboBox->addItem("open loop");
    selectMode_ComboBox->addItem("position control");
    selectMode_ComboBox->addItem("velocity control");
    selectMode_ComboBox->addItem("torque control");
    selectMode_ComboBox->addItem("pinc control");

    Mode_Label = new QLabel(tr("Mode:"));

    selectLoop_ComboBox = new QComboBox;
    selectLoop_ComboBox->addItem("current loop");
    selectLoop_ComboBox->addItem("velocity loop");
    selectLoop_ComboBox->addItem("position loop");

    ParamGroup_Label = new QLabel(tr("Parameter\nGroup:"));

    setKp_SpinBox = new QSpinBox;
    setKp_SpinBox->setRange(0,65535);
    setKp_Label = new QLabel(tr("Kp"));

    setKi_SpinBox = new QSpinBox;
    setKi_SpinBox->setRange(0,65535);
    setKi_Label = new QLabel(tr("Ki"));

    setKd_SpinBox = new QSpinBox;
    setKd_SpinBox->setRange(0,65535);
    setKd_Label = new QLabel(tr("Kd"));

    send_CheckBox = new QCheckBox;
    send_Label = new QLabel(tr("send"));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* LineLayout_1 = new QHBoxLayout;
    QHBoxLayout* LineLayout_2 = new QHBoxLayout;

    mainLayout->addLayout(LineLayout_1);
    mainLayout->addLayout(LineLayout_2);

    LineLayout_1->addWidget(Mode_Label);
    LineLayout_1->addWidget(selectMode_ComboBox);
    LineLayout_1->addWidget(ParamGroup_Label);
    LineLayout_1->addWidget(selectLoop_ComboBox);

    LineLayout_2->addWidget(setKp_Label);
    LineLayout_2->addWidget(setKp_SpinBox);
    LineLayout_2->addWidget(setKi_Label);
    LineLayout_2->addWidget(setKi_SpinBox);
    LineLayout_2->addWidget(setKd_Label);
    LineLayout_2->addWidget(setKd_SpinBox);
    LineLayout_2->addStretch();
    LineLayout_2->addWidget(send_Label);
    LineLayout_2->addWidget(send_CheckBox);

    main_GroupBox->setLayout(mainLayout);

    QHBoxLayout* GroupBox_Layout = new QHBoxLayout;
    GroupBox_Layout->addWidget(main_GroupBox);
    GroupBox_Layout->setMargin(0);
    GroupBox_Layout->setSpacing(0);
    setLayout(GroupBox_Layout);

    this->setMaximumWidth(350);

    connect(selectLoop_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_paramGroup_changed(int)));
    connect(setKp_SpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_Kp_changed(int)));
    connect(setKi_SpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_Ki_changed(int)));
    connect(setKd_SpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_Kd_changed(int)));
    connect(send_CheckBox, SIGNAL(clicked(bool)), this, SLOT(on_sendBox_clicked(bool)));
    connect(selectMode_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_controlMode_changed(int)));

    Cur_.Kp = 11;
    Cur_.Ki = 12;
    Cur_.Kd = 13;

    Vel_.Kp = 21;
    Vel_.Ki = 22;
    Vel_.Kd = 23;

    Pos_.Kp = 31;
    Pos_.Ki = 32;
    Pos_.Kd = 33;

    on_paramGroup_changed(clv::KP);
}


//
//void attach_calibrator(CComCalier*)
//
void CMotorParam::attach_calibrator(CComCalier *p)
{
    if (p == NULL) return;
    Calibrator = p;
    connect(this, SIGNAL(send_ucp_pack(uint8_t, uint16_t)), Calibrator, SLOT(send_ucp_pack(uint8_t, uint16_t)));
    connect(Calibrator, SIGNAL(sync_dataVector(std::vector<uint16_t>)), this, SLOT(sync_dataVector(std::vector<uint16_t>)));
}

//
//void CMotorParam::sync_dataVector(std::vector<uint16_t> datav)
//
void CMotorParam::sync_dataVector(std::vector<uint16_t> datav)
{
    Vel_.Kp = datav[clv::VCTRL_KP];
    Vel_.Ki = datav[clv::VCTRL_KI];
    Vel_.Kd = datav[clv::VCTRL_KD];
    Pos_.Kp = datav[clv::PCTRL_KP];
    Pos_.Ki = datav[clv::PCTRL_KI];
    Pos_.Kd = datav[clv::PCTRL_KD];

    selectMode_ComboBox->setCurrentIndex(datav[clv::CTRLMODE]);
    on_paramGroup_changed(selectLoop_ComboBox->currentIndex());
}

//
//void on_Kp_changed(uint16_t)
//
void CMotorParam::on_Kp_changed(int value)
{
    on_Kparam_changed(clv::KP, value);
}

//
//void on_Ki_changed(uint16_t)
//
void CMotorParam::on_Ki_changed(int value)
{
    on_Kparam_changed(clv::KI, value);
}

//
//void on_Kd_changed(uint16_t)
//
void CMotorParam::on_Kd_changed(int value)
{
    on_Kparam_changed(clv::KD, value);
}

//
//void on_paramGroup_changed(int)
//
void CMotorParam::on_paramGroup_changed(int)
{
    bool send_state = send_CheckBox->isChecked();
    if(send_state == true)
        send_CheckBox->setChecked(false);

#if 0
    std::cout << "send_state = " << (int)send_state <<std::endl;
    std::cout << "check box = " << (int)(send_CheckBox->isChecked()) <<std::endl;
#endif

    if(selectLoop_ComboBox->currentText() == "current loop")
    {
        setKp_SpinBox->setValue(Cur_.Kp);
        setKi_SpinBox->setValue(Cur_.Ki);
        setKd_SpinBox->setValue(Cur_.Kd);
    }
    else if(selectLoop_ComboBox->currentText() == "velocity loop")
    {
        setKp_SpinBox->setValue(Vel_.Kp);
        setKi_SpinBox->setValue(Vel_.Ki);
        setKd_SpinBox->setValue(Vel_.Kd);
    }
    else if(selectLoop_ComboBox->currentText() == "position loop")
    {
        setKp_SpinBox->setValue(Pos_.Kp);
        setKi_SpinBox->setValue(Pos_.Ki);
        setKd_SpinBox->setValue(Pos_.Kd);
    }
    else
    {}

    /* recover the send check box state */
    if(send_state == true)
        send_CheckBox->setChecked(true);
}

//
//void on_controlMode_changed(int)
//
void CMotorParam::on_controlMode_changed(int value)
{
    if(send_CheckBox->isChecked())
        emit send_ucp_pack(clv::CTRLMODE, value);
}

//
//void on_K_changed(uint16_t)
//
void CMotorParam::on_Kparam_changed(int param, int value)
{
    if(selectLoop_ComboBox->currentText() == "current loop")
    {
#if 0
        std::cout<< "param" <<param <<std::endl;
#endif
        ((uint16_t*)&Cur_)[param] = value;
    }
    else if(selectLoop_ComboBox->currentText() == "velocity loop")
    {
        ((uint16_t*)&Vel_)[param] = value;
        if(send_CheckBox->isChecked())
            emit send_ucp_pack(clv::VCTRL_BASE + param, value);
    }
    else if(selectLoop_ComboBox->currentText() == "postion loop")
    {
        ((uint16_t*)&Pos_)[param] = value;
    }
    else
    {}
}

//
//void on_sendBox_clicked(bool)
//
void CMotorParam::on_sendBox_clicked(bool on)
{
    if(on)
    {
        emit send_ucp_pack(clv::CTRLMODE, selectMode_ComboBox->currentIndex());    //mode
        if(selectLoop_ComboBox->currentText() == "velocity loop")
        {
            emit send_ucp_pack(clv::VCTRL_BASE + clv::KP, setKp_SpinBox->value());         //v kp
            emit send_ucp_pack(clv::VCTRL_BASE + clv::KI, setKi_SpinBox->value());         //v ki
            emit send_ucp_pack(clv::VCTRL_BASE + clv::KD, setKd_SpinBox->value());         //v kd
        }
        else
        {}
    }
}

//end of file
