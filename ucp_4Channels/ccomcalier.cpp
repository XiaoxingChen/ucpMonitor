/**
******************************************************************************
* @file    CComCalier.cpp
* @author  Chenxx
* @version V1.1
* @date    2016-02-02
* @brief   This file defines the calibrator that work with UART.
******************************************************************************/
/*******************************************************************************
* release information
* 2016/02/17 chenxx add the SYNC_MODE for the use of data synchronize.
*           Add mode judgement in Uart unpack.
*******************************************************************************/
#include "ccomcalier.h"
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <iostream>
#include <qabstractitemview.h>
#include "win_qextserialport.h"
#include "ccomgroup.h"
#include "datapool.h"
#include "caliVar.h"
#include <qlabel.h>
#include <qcheckbox.h>
#include <vector>
#include <stdint.h>

/******************************************************************************
* @brief   This part for CComCalier
******************************************************************************/
CComCalier::CComCalier(QWidget *parent) :
    QWidget(parent),
    Uart(NULL),
    syncDataThread(NULL),
    UartConfig(NULL),
    dataPool(NULL),
    receiveMode(PLOT_MODE)
{
    QSizePolicy policy;

    mainGroupBox = new QGroupBox;
    mainGroupBox->setTitle(tr("calibrator"));

    /* set slider */
    adjust_Slider = new QSlider;
    adjust_Slider->setRange(0, 65535);
    adjust_Slider->setValue(0);

    /* set spinbox */
    adjust_SpinBox = new QSpinBox;
    adjust_SpinBox->setRange(0, 65535);
    adjust_SpinBox->setValue(0);
    adjust_SpinBox->setSingleStep(1);
    adjust_SpinBox->setMaximumWidth(60);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    adjust_SpinBox->setSizePolicy(policy);

    /* selectSign_PushButton */
    selectSign_PushButton = new QPushButton(tr("to int16"));
    selectSign_PushButton->setMaximumWidth(60);

    /* select variables combobox */
    selectVar_ComboBox = new QComboBox;
//    selectVar_ComboBox->addItem("Idx0");
//    selectVar_ComboBox->addItem("Idx1");
//    selectVar_ComboBox->addItem("Idx2");
//    selectVar_ComboBox->addItem("Idx3");
//    selectVar_ComboBox->addItem("Cpu Usage");
//    selectVar_ComboBox->addItem("Pos of Motor");
//    selectVar_ComboBox->addItem("Vel of Motor");
//    selectVar_ComboBox->addItem("Acc of Motor");
//    selectVar_ComboBox->addItem("Cur of Coil");
//    selectVar_ComboBox->addItem("PWM Duty");
//    selectVar_ComboBox->addItem("Dir of Rotate");
//    selectVar_ComboBox->addItem("Mode of Control");
//    selectVar_ComboBox->addItem("Target Value");
//    selectVar_ComboBox->addItem("Kp of vControl");
//    selectVar_ComboBox->addItem("Ki of vControl");
//    selectVar_ComboBox->addItem("Kd of vControl");
    for(int i = 0; i < clv::VAR_NUM; i ++)
    {
        selectVar_ComboBox->addItem(clv::IdxText[i]);
    }
    selectVar_ComboBox->setMaximumWidth(60);
    selectVar_ComboBox->view()->setMinimumWidth(150);
    selectVar_ComboBox->setCurrentIndex(clv::PWMDUTY);

    /* send check box and label */
    send_CheckBox = new QCheckBox;
    send_CheckBox->setEnabled(false);
    send_Label = new QLabel(tr("send"));
    send_Label->setAlignment(Qt::AlignLeft);

    /* synchronize data pushbutton */
    syncData_PushButton = new QPushButton(tr("Sync"));
    syncData_PushButton->setMaximumWidth(60);

    /* synchronization thread */
    syncDataThread = new CSyncDataThread;
    syncDataThread->attach_calibrator(this);

    /* set layout */
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *sendbox_Layout = new QHBoxLayout;
    mainLayout->addWidget(adjust_Slider, 0, Qt::AlignHCenter);
    mainLayout->addWidget(adjust_SpinBox);
    mainLayout->addWidget(selectSign_PushButton);
    mainLayout->addWidget(selectVar_ComboBox);
    mainLayout->addLayout(sendbox_Layout);
    mainLayout->addWidget(syncData_PushButton);

    sendbox_Layout->addWidget(send_CheckBox);
    sendbox_Layout->addWidget(send_Label);
    sendbox_Layout->addStretch();

    mainGroupBox->setLayout(mainLayout);

    QVBoxLayout *groupBox_Layout = new QVBoxLayout;
    groupBox_Layout->addWidget(mainGroupBox);
    groupBox_Layout->setMargin(0);
    groupBox_Layout->setSpacing(0);
    setLayout(groupBox_Layout);

    this->setMaximumWidth(80);

    connect(selectSign_PushButton, SIGNAL(clicked()), this, SLOT(change_Sign()));
    connect(adjust_SpinBox, SIGNAL(valueChanged(int)), adjust_Slider, SLOT(setValue(int)));
    connect(adjust_Slider, SIGNAL(valueChanged(int)), adjust_SpinBox, SLOT(setValue(int)));
    connect(adjust_SpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_adjustValue_changed(int)));
    connect(send_CheckBox, SIGNAL(clicked(bool)), this, SLOT(send_clicked(bool)));
    connect(syncData_PushButton, SIGNAL(clicked()), this, SLOT(on_syncData_clicked()));
}

CComCalier::~CComCalier()
{
//    delete adjust_Slider;
    delete mainGroupBox;
}

//
// void change_Sign()
//
void CComCalier::change_Sign()
{
    static bool Signed = false;
    uint16_t temp;
//    int16_t signed_temp;
//    uint16_t unsigned_temp;
    Signed = !Signed;
    if(Signed)
    {
        selectSign_PushButton->setText(tr("to uint16"));
        temp = adjust_SpinBox->value();                     //read data(unsigned)
        adjust_SpinBox->setRange(-32769, 65536);            //now range is big enough
        adjust_Slider->setRange(-32769, 65536);             //now range is big enough
        adjust_SpinBox->setValue((int16_t)temp);            //write data(signed)
        adjust_SpinBox->setRange((int16_t)0x8000, 0x7FFF);  //now range is signed
        adjust_Slider->setRange((int16_t)0x8000, 0x7FFF);   //now range is signed
    }
    else
    {
        selectSign_PushButton->setText(tr("to int16"));
        temp = adjust_SpinBox->value();                     //read data(signed)
        adjust_SpinBox->setRange(-32769, 65536);            //now range is big enough
        adjust_Slider->setRange(-32769, 65536);             //now range is big enough
        adjust_SpinBox->setValue(temp);                     //write data(unsigned)
        adjust_SpinBox->setRange(0, 0xFFFF);                //now range is unsigned
        adjust_Slider->setRange(0, 0xFFFF);                 //now range is unsigned
    }
    on_adjustValue_changed(temp);
}

//
//void on_UartConnect_clicked()
//
void CComCalier::on_UartConnect_clicked()
{
    static bool connected = false;
    connected = !connected;

    send_CheckBox->setEnabled(connected);
    if(connected)
    {
        if(UartConfig == NULL)
            std::cout << "UartConfig == NULL" <<std::endl;

        QString portName = UartConfig->Uart_COM_port();
        Uart = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);

        Uart ->open(QIODevice::ReadWrite);

        //设置波特率
        if(UartConfig->Uart_Baudrate()==tr("9600")) //根据组合框内容对串口进行设置
            Uart->setBaudRate(BAUD9600);
        else if(UartConfig->Uart_Baudrate()==tr("115200"))
            Uart->setBaudRate(BAUD115200);
        else
            std::cout<< "error1!"<< std::endl;

        //设置数据位
        if(UartConfig->Uart_DataBits()==tr("8"))
            Uart->setDataBits(DATA_8);
        else if(UartConfig->Uart_DataBits()==tr("7"))
            Uart->setDataBits(DATA_7);
        else
            std::cout<< "error2!"<< std::endl;

        //设置奇偶校验
        if(UartConfig->Uart_Check()==tr("None"))
            Uart->setParity(PAR_NONE);
        else if(UartConfig->Uart_Check()==tr("Odd"))
            Uart->setParity(PAR_ODD);
        else if(UartConfig->Uart_Check()==tr("Even"))
            Uart->setParity(PAR_EVEN);
        else
            std::cout<< "error3!"<< std::endl;

        //设置停止位
        if(UartConfig->Uart_StopBits()==tr("1"))
            Uart->setStopBits(STOP_1);
        else if(UartConfig->Uart_StopBits()==tr("2"))
            Uart->setStopBits(STOP_2);
        else
            std::cout<< "error3!"<< std::endl;

        Uart->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
        Uart->setTimeout(500); //设置延时

        connect(Uart,SIGNAL(readyRead()),this,SLOT(unpack_Uart()));
    }
    else
    {
        Uart->close();
    }
}

//
//void on_adjustValue_changed(int)
//
void CComCalier::on_adjustValue_changed(int value)
{
    uint8_t idx = selectVar_ComboBox->currentIndex();
    if(send_CheckBox->isChecked())
    {
        send_ucp_pack(idx, value);
    }
}

//
//void send_ucp_pack(uint8_t idx, uint16_t value)
//
void CComCalier::send_ucp_pack(uint8_t idx, uint16_t value)
{
    if(Uart != NULL && Uart->isOpen())
    {
        QString tempQString;
        tempQString = "55aa00";
        tempQString += QString("%1").arg(idx,2,16,QLatin1Char('0'));
        tempQString += QString("%1").arg((uint8_t)(value&0xff),2,16,QLatin1Char('0'));
        tempQString += QString("%1").arg((uint8_t)(value>>8),2,16,QLatin1Char('0'));
        tempQString += "0000";

        QByteArray tempByteArray;
        tempByteArray.append(tempQString);
        Uart->write(tempByteArray.fromHex(tempByteArray));
    }
}

//
//void on_dataVector_synced(std::vector<uint16_t>);
//
void CComCalier::on_dataVector_synced(std::vector<uint16_t> d)
{
    receiveMode = PLOT_MODE;
    emit sync_dataVector(d);
}

//
//void send_clicked(bool clicked)
//
void CComCalier::send_clicked(bool clicked)
{
    if(clicked == true)
    {
        uint8_t idx = selectVar_ComboBox->currentIndex();
        uint16_t value = adjust_SpinBox->value();
        send_ucp_pack(idx, value);
    }
}

//
//void unpack_Uart()
//
void CComCalier::unpack_Uart()
{

    static QByteArray temp;
    PkgTyp tempPack;
    uint8_t packCount = 0;
    if(dataPool == NULL)
    {
        std::cout<< "datapool point is NULL" <<std::endl;
        return;
    }

    /* init parameter*/
    temp += Uart->readAll();

    if(temp.size() < sizeof(tempPack))
        return;
    while(1)
    {
        /* check the pack head and pack end */
        if((uint8_t)temp[0] == 0xa5 &&
                (uint8_t)temp[1] == 0x5a &&
                (uint8_t)temp[sizeof(tempPack)-1] == 0x00)
        {
            /* copy full pack data */
            for(uint16_t i = 0; i < sizeof(tempPack); i++)
            {
                ((uint8_t*)&tempPack)[i] = temp[i];
            }
            packCount++;
            temp.remove(0, sizeof(tempPack));

            /* calculate time stamp */
            uint32_t timeStamp;
            timeStamp = ((uint32_t)tempPack.TimeStamp_H<<16) + tempPack.TimeStamp_L;
            qreal timeStampF = (qreal)timeStamp/1000;

            /* read the data field */
            if(receiveMode == PLOT_MODE)    //transmit data to datapool
            {
                for(int i = 0; i < 4; i++)
                {
                    if((tempPack.DataSign>>i)&1)
                        dataPool->WritePoints(i, QPointF(timeStampF,(int16_t)tempPack.Data[i]));
                    else
                        dataPool->WritePoints(i, QPointF(timeStampF,(uint16_t)tempPack.Data[i]));
                }
            }
            else if(receiveMode == SYNC_MODE && syncDataThread != NULL)//transmit data to sync_buff
            {
                for(int i = 0; i < 4; i++)
                {
                    syncDataThread->sync_buff[i] = tempPack.Data[i];
                }
            }
        }

        if(temp.size() <= sizeof(tempPack))
        {
//            std::cout <<"length = " << temp.size() <<std::endl;
            break;
        }
        temp.remove(0, 1);
    }

#if 0
    if(packCount > 0)
    {
        std::cout << " data0 = " << (int)tempPack.Data[0];
        std::cout << " data1 = " << (int)tempPack.Data[1];
        std::cout << " data2 = " << (int)tempPack.Data[2];
        std::cout << " data3 = " << (int)tempPack.Data[3]<<std::endl;
    }
#endif
}

//
//void on_syncData_clicked()
//
void CComCalier::on_syncData_clicked()
{
    receiveMode = SYNC_MODE;
    syncDataThread->start();
}

//
//void attach_UartConfig(CComGroup*);
//
void CComCalier::attach_UartConfig(CComGroup * config)
{
    UartConfig = config;
}

//
//void attach_DataPool(DataPool*)
//
void CComCalier::attach_DataPool(DataPool *pPool)
{
    dataPool = pPool;
}

/******************************************************************************
* @brief   This part for CSyncDataThread
******************************************************************************/
//
//void attach_calibrator(CComCalier*p)
//
void CSyncDataThread::attach_calibrator(CComCalier *p)
{
    if (p == NULL) return;
    calibrator = p;
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    connect(this, SIGNAL(send_ucp_pack(uint8_t, uint16_t)),
            calibrator, SLOT(send_ucp_pack(uint8_t, uint16_t)),
            Qt::QueuedConnection);

    qRegisterMetaType< std::vector<uint16_t> >("std::vector<uint16_t>");
    connect(this, SIGNAL(sync_dataVector(std::vector<uint16_t>)),
            calibrator, SLOT(on_dataVector_synced(std::vector<uint16_t>)),
            Qt::QueuedConnection);
}

//
//void run()
//
void CSyncDataThread::run()
{
#if 0
    std::cout << "run into thread" <<std::endl;
#endif
    const uint8_t VAR_NUM = 24; //ucp::REG_SIZE
    std::vector<uint16_t> dataVector(VAR_NUM);



    for(int i = 1; i < 4; i++)
    {
        /* give up the current value of Idx0 */
        emit send_ucp_pack(clv::IDX0, clv::IDX0);
        while(sync_buff[0] != clv::IDX0) msleep(1);
        /* use Idx0 to read the current value of Idx1 */
        /* in most situation, the value won't be 0 */
        emit send_ucp_pack(clv::IDX0, i);   //read the value of Idxi
        while(sync_buff[0] == clv::IDX0) msleep(1);
        dataVector[i] = sync_buff[0];
    }

    emit send_ucp_pack(clv::IDX0, clv::IDX2);
    emit send_ucp_pack(clv::IDX1, clv::IDX3);
    for(int i = 4; i < (VAR_NUM - VAR_NUM%2); i+= 2)
    {
        emit send_ucp_pack(clv::IDX2, i);
        emit send_ucp_pack(clv::IDX3, i+1);

        while(sync_buff[0] != i || sync_buff[1] != i+1)
        {
#if 0
            std::cout <<"i = " << i;
            std::cout <<" buff0 = " << (int)sync_buff[0];
            std::cout <<" buff1 = " << (int)sync_buff[1];
            std::cout << std::endl;
#endif
            msleep(1);
        }

        dataVector[i] = sync_buff[2];
        dataVector[i+1] = sync_buff[3];

    }
    dataVector[0] = clv::MOTORVEL;
    emit sync_dataVector(dataVector);
    emit send_ucp_pack(clv::IDX0, dataVector[0]);
    emit send_ucp_pack(clv::IDX1, dataVector[1]);
    emit send_ucp_pack(clv::IDX2, dataVector[2]);
    emit send_ucp_pack(clv::IDX3, dataVector[3]);
    std::cout <<"sync finished " <<std::endl;
}
//end of file
