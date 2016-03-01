/**
******************************************************************************
* @file    ccomcalier.cpp
* @author  Chenxx
* @version V1.1
* @date    2016-02-03
* @brief   This file provides the UART calibrator functions.
******************************************************************************/
/*******************************************************************************
* release information
* 2016/02/17 chenxx add the SYNC_MODE for the use of data synchronize.
*           Add enum SYNC_MODE and PLOT_MODE in class defination.
*           Define sync_buffer that used for UART unpack.
*******************************************************************************/
#ifndef CCOMCALIER_H
#define CCOMCALIER_H

#include <QWidget>
#include <qthread.h>
class QGroupBox;
class QSpinBox;
class QPushButton;
class QSlider;
class QLabel;
class QCheckBox;
class QComboBox;
class CComGroup;
class Win_QextSerialPort;
class DataPool;
//template<typename T>
//class vector;
class CSyncDataThread;
/******************************************************************************
* @brief   This part for CComCalier
******************************************************************************/
class CComCalier : public QWidget
{
    Q_OBJECT
public:
    struct PkgTyp
    {
        uint16_t PackHead;
        uint8_t ErrorCode;
        uint8_t TimeStamp_H;
        uint16_t TimeStamp_L;
        uint8_t DataIdx[4];
        uint16_t Data[4];
        uint8_t DataSign;			//[0..3] sign of data
        uint8_t PackEnd;
    };

    enum ReceiveMode
    {
        PLOT_MODE = 0,
        SYNC_MODE = 1
    };

    explicit CComCalier(QWidget *parent = 0);
    ~CComCalier();
    void attach_UartConfig(CComGroup*);
    void attach_DataPool(DataPool*);

private:
    QGroupBox* mainGroupBox;
    QSlider* adjust_Slider;
    QSpinBox* adjust_SpinBox;
    QPushButton* selectSign_PushButton;     //select singed or unsigned
    QComboBox* selectVar_ComboBox;          //select vaiables
    Win_QextSerialPort *Uart;
    QLabel* send_Label;
    QCheckBox* send_CheckBox;
    QPushButton* syncData_PushButton;
    CSyncDataThread* syncDataThread;
    /* need attach */
    CComGroup* UartConfig;
    DataPool* dataPool;
    int receiveMode;

signals:
    void sync_dataVector(std::vector<uint16_t>);
    
public slots:
    void on_UartConnect_clicked();
    void send_ucp_pack(uint8_t, uint16_t);
    void on_dataVector_synced(std::vector<uint16_t>);

private slots:
    void change_Sign();
    void on_adjustValue_changed(int);
    void unpack_Uart();
    void send_clicked(bool);
    void on_syncData_clicked();
    
};

/******************************************************************************
* @brief   This part for CSyncDataThread
******************************************************************************/
class CSyncDataThread: public QThread
{
    Q_OBJECT
    friend class CComCalier;
signals:
    void send_ucp_pack(uint8_t, uint16_t);
    void sync_dataVector(std::vector<uint16_t>);

private:
    void attach_calibrator(CComCalier *);
    CComCalier *calibrator;
    uint16_t sync_buff[4];

protected:
    void run();
};

#endif // CCOMCALIER_H
