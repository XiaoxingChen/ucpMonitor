#ifndef CMOTORPARAM_H
#define CMOTORPARAM_H

#include <QWidget>
class QPushButton;
class QSpinBox;
class QComboBox;
class QGroupBox;
class QCheckBox;
class QLabel;
class CComCalier;

class CMotorParam : public QWidget
{
    Q_OBJECT
public:
    struct K_Typ
    {
        uint16_t Kp;
        uint16_t Ki;
        uint16_t Kd;
    };
    enum
    {
        KP = 0,
        KI = 1,
        KD = 2
    };

    explicit CMotorParam(QWidget *parent = 0);
    void attach_calibrator(CComCalier*);
    
signals:
    void send_ucp_pack(uint8_t, uint16_t);
    
public slots:
    void sync_dataVector(std::vector<uint16_t>);

private slots:
    void on_Kp_changed(int);
    void on_Ki_changed(int);
    void on_Kd_changed(int);
    void on_paramGroup_changed(int);
    void on_controlMode_changed(int);
    void on_sendBox_clicked(bool);


private:
    void on_Kparam_changed(int param, int value);
    QGroupBox* main_GroupBox;
    QComboBox* selectMode_ComboBox;
    QLabel* Mode_Label;
    QComboBox* selectLoop_ComboBox;
    QLabel* ParamGroup_Label;
    QSpinBox* setKp_SpinBox;
    QLabel* setKp_Label;
    QSpinBox* setKi_SpinBox;
    QLabel* setKi_Label;
    QSpinBox* setKd_SpinBox;
    QLabel* setKd_Label;
    QCheckBox* send_CheckBox;
    QLabel* send_Label;
    CComCalier* Calibrator;

    K_Typ Pos_;
    K_Typ Vel_;
    K_Typ Cur_;

};

#endif // CMOTORPARAM_H
