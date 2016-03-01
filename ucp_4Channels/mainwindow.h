#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QPushButton;
class SignalChannel;
class QwtPlot;
class QTimer;
class ScreenPlot;
class CComGroup;
class CComCalier;
class DataPool;
class CMotorParam;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    ScreenPlot* Screen;
    CComGroup* com_Group;
    CComCalier* com_Calier;
    DataPool* data_Pool;
    CMotorParam *motorParam;
    SignalChannel* SigCH1;
    SignalChannel* SigCH2;
    SignalChannel* SigCH3;
    SignalChannel* SigCH4;

    QPushButton* Quit_PushButton;
    QTimer* Plot_Timer;

private slots:
};

#endif // MAINWINDOW_H
