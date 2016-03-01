#include "ccomgroup.h"
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

CComGroup::CComGroup(QWidget *parent) :
    QWidget(parent)
{
    mainGroupBox = new QGroupBox;
    mainGroupBox->setTitle(tr("- COM -"));

    selectCOM_ComboBox = new QComboBox;
    selectCOM_ComboBox->addItem("COM1");
    selectCOM_ComboBox->addItem("COM2");
    selectCOM_ComboBox->addItem("COM3");
    selectCOM_ComboBox->addItem("COM4");
    selectCOM_ComboBox->addItem("COM5");
    selectCOM_ComboBox->addItem("COM6");
    selectCOM_ComboBox->addItem("COM7");

    selectBaudrate_ComboBox = new QComboBox;
    selectBaudrate_ComboBox->addItem("115200");
    selectBaudrate_ComboBox->addItem("9600");
    selectBaudrate_ComboBox->addItem("4800");

    selectCheck_ComboBox = new QComboBox;
    selectCheck_ComboBox->addItem("None");
    selectCheck_ComboBox->addItem("Even");
    selectCheck_ComboBox->addItem("Odd");

    selectDataBits_ComboBox = new QComboBox;
    selectDataBits_ComboBox->addItem("9");
    selectDataBits_ComboBox->addItem("8");
    selectDataBits_ComboBox->addItem("7");
    selectDataBits_ComboBox->setCurrentIndex(1);

    selectStopBits_ComboBox = new QComboBox;
    selectStopBits_ComboBox->addItem("1");
    selectStopBits_ComboBox->addItem("2");

    connect_PushButton = new QPushButton(tr("connect"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(selectCOM_ComboBox);
    mainLayout->addWidget(selectBaudrate_ComboBox);
    mainLayout->addWidget(selectCheck_ComboBox);
    mainLayout->addWidget(selectDataBits_ComboBox);
    mainLayout->addWidget(selectStopBits_ComboBox);
    mainLayout->addWidget(connect_PushButton);

    mainGroupBox->setLayout(mainLayout);

    QHBoxLayout *groupBox_Layout = new QHBoxLayout;
    groupBox_Layout->addWidget(mainGroupBox);
    groupBox_Layout->setMargin(0);
    groupBox_Layout->setSpacing(0);
    setLayout(groupBox_Layout);

    this->setMaximumWidth(350);

    connect(connect_PushButton, SIGNAL(clicked()), this, SLOT(connect_COM()));
    connect(connect_PushButton, SIGNAL(clicked()), this, SIGNAL(connect_clicked()));

}

CComGroup::~CComGroup()
{
    delete selectCheck_ComboBox;
    delete selectBaudrate_ComboBox;
    delete selectCOM_ComboBox;
    delete mainGroupBox;

}

void CComGroup::connect_COM()
{
    static bool connected = false;
    connected = !connected;
    if(connected == false)
    {
        connect_PushButton->setText(tr("connect"));
    }
    else
    {
        connect_PushButton->setText(tr("disconnect"));
    }
    selectCOM_ComboBox->setDisabled(connected);
    selectBaudrate_ComboBox->setDisabled(connected);
    selectCheck_ComboBox->setDisabled(connected);
    selectDataBits_ComboBox->setDisabled(connected);
    selectStopBits_ComboBox->setDisabled(connected);

}

QString CComGroup::Uart_COM_port() const
{
    return selectCOM_ComboBox->currentText();
}
QString CComGroup::Uart_Baudrate() const
{
    return selectBaudrate_ComboBox->currentText();
}
QString CComGroup::Uart_Check() const
{
    return selectCheck_ComboBox->currentText();
}
QString CComGroup::Uart_DataBits() const
{
    return selectDataBits_ComboBox->currentText();
}
QString CComGroup::Uart_StopBits() const
{
    return selectStopBits_ComboBox->currentText();
}
