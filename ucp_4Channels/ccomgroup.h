#ifndef CCOMGROUP_H
#define CCOMGROUP_H

#include <QWidget>
class QGroupBox;
class QComboBox;
class QPushButton;
class CComGroup : public QWidget
{
    Q_OBJECT
public:
    explicit CComGroup(QWidget *parent = 0);
    ~CComGroup();
    QString Uart_COM_port() const;
    QString Uart_Baudrate() const;
    QString Uart_Check() const;
    QString Uart_DataBits() const;
    QString Uart_StopBits() const;

private:
    QGroupBox* mainGroupBox;
    QComboBox* selectCOM_ComboBox;
    QComboBox* selectBaudrate_ComboBox;
    QComboBox* selectCheck_ComboBox;
    QComboBox* selectDataBits_ComboBox;
    QComboBox* selectStopBits_ComboBox;
    QPushButton* connect_PushButton;

signals:
    void connect_clicked();
public slots:
private slots:
    void connect_COM();
    
};

#endif // CCOMGROUP_H
