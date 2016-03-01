#include "screenplot.h"
#include "signalchannel.h"
#include "datapool.h"
#include <QDoubleSpinBox>
#include <qwt_plot.h>
#include <qscrollbar.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <QTime>
#include <qlayout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <iostream>
#include <qwt_plot_picker.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <fstream>
#include <qcombobox.h>

ScreenPlot::ScreenPlot(QWidget *parent) :
    QWidget(parent),
    UpdatePoints_Flag(true)
{
    QSizePolicy policy;

    /* set plot */
    Wave_Plot = new QwtPlot;
        /* set canvas */
    QwtPlotCanvas* canvas = new QwtPlotCanvas;
    canvas->setPalette(Qt::black);
    canvas->setBorderRadius(5);
    Wave_Plot->setCanvas(canvas);
        /* set x,y axis */
    Wave_Plot->setAxisTitle(QwtPlot::xBottom, "time(s)");
    Wave_Plot->setAxisTitle(QwtPlot::yLeft, "signal");
    Wave_Plot->setAxisScale(QwtPlot::xBottom, 0, 10.0);
    Wave_Plot->setAxisScale(QwtPlot::yLeft, -3000, 6000);
        /* set grid */
    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->enableX(true);
    grid->enableX(true);
    grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
    grid->attach(Wave_Plot);
        /* set plot picker */
    QwtPlotPicker* picker = new QwtPlotPicker(
                QwtPlot::xBottom,
                QwtPlot::yLeft,
                QwtPlotPicker::CrossRubberBand,
                QwtPicker::AlwaysOn,
                canvas );
    picker->setTrackerPen(QPen(Qt::white));

    /* set pushbutton */
    PauseUpdatePoints_PushButton = new QPushButton("pause");

    SetOffset_ScrollBar = new QScrollBar(Qt::Horizontal);
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    SetOffset_ScrollBar->setSizePolicy(policy);
    SetOffset_ScrollBar->setRange(-10000, 10000);
    SetOffset_ScrollBar->setValue(0);

    /* set time scaling spinbox */
    TimeScaling_SpinBox = new QDoubleSpinBox;
    TimeScaling_SpinBox->setValue(1);
    TimeScaling_SpinBox->setSingleStep(0.5);

    /* replot timer */
    Replot_Timer = new QTimer;
    Replot_Timer->start(50);

    /* saveData pushbutton */
    saveData_PushButton = new QPushButton(tr("save data"));

    /* chose dir filedialog */
    selectDir_FileDialog = new QFileDialog;
    selectDir_FileDialog->setFileMode(QFileDialog::AnyFile);
    selectDir_FileDialog->setNameFilter("text (*.txt)");
    selectDir_FileDialog->setDirectory("D:/Engneering/QT/proj/ucp_4Channels/data");
    selectDir_FileDialog->setLabelText(QFileDialog::Accept, tr("save"));
    selectDir_FileDialog->setDefaultSuffix(QString("txt"));

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* subLayout_l1 = new QVBoxLayout;
    QHBoxLayout* subLayout_b1 = new QHBoxLayout;

    mainLayout->addLayout(subLayout_l1);

    subLayout_l1->addWidget(Wave_Plot);
    subLayout_l1->addLayout(subLayout_b1);

    subLayout_b1->addWidget(PauseUpdatePoints_PushButton);
    subLayout_b1->addWidget(saveData_PushButton);
    subLayout_b1->addWidget(SetOffset_ScrollBar);
    subLayout_b1->addWidget(TimeScaling_SpinBox);

    setLayout(mainLayout);

    connect(Replot_Timer, SIGNAL(timeout()), this, SLOT(Update_WavePlot()));
    connect(PauseUpdatePoints_PushButton, SIGNAL(clicked()), this, SLOT(Change_UpdatePointsFlag()));
    connect(saveData_PushButton, SIGNAL(clicked()), selectDir_FileDialog, SLOT(show()));
    connect(selectDir_FileDialog, SIGNAL(fileSelected(QString)), this, SLOT(on_saveDir_selected(QString)));
}
//
//
//
void ScreenPlot::attach_DataPool(DataPool * pPool)
{
    DataPool1 = pPool;
}

//
//void attach_SignalChannel(uint8_t, SignalChannel *)
//
void ScreenPlot::attach_SignalChannel(SignalChannel * p)
{
//    SigCH1
    switch(p->get_channelNum())
    {
        case 1:
            SigCH1 = p;
            break;
        case 2:
            SigCH2 = p;
            break;
        case 3:
            SigCH3 = p;
            break;
        case 4:
            SigCH4 = p;
            break;
        default:
            break;
    }
}
void ScreenPlot::Update_Timestamp()
{
    latestTimestamp = (DataPool1->GetTimeStampF());
}
void ScreenPlot::Update_WavePlot()
{
    static qreal TimeWindowCenter = 5;
    const int BASE_WIDTH = 5;
    qreal HalfWindowWidth;


    SigCH1->Update_Points();
    SigCH2->Update_Points();
    SigCH3->Update_Points();
    SigCH4->Update_Points();
    if(UpdatePoints_Flag)
    {
        Update_Timestamp();

    }
    /* calculate WindowWidth */
    if(TimeScaling_SpinBox->value() > 0.01)
    {
        HalfWindowWidth = (qreal)BASE_WIDTH / TimeScaling_SpinBox->value();
    }

    /* calculate window position */
    if(latestTimestamp <= 10)
    {
        TimeWindowCenter = BASE_WIDTH + (qreal)SetOffset_ScrollBar->value()/1000;
    }
    else
    {
        TimeWindowCenter = latestTimestamp - BASE_WIDTH + (qreal)SetOffset_ScrollBar->value()/1000;
    }
    Wave_Plot->setAxisScale(QwtPlot::xBottom, TimeWindowCenter - HalfWindowWidth, TimeWindowCenter + HalfWindowWidth);
    Wave_Plot->replot();
}
void ScreenPlot::Change_UpdatePointsFlag()
{
    if(UpdatePoints_Flag)
    {
       UpdatePoints_Flag = false;
       PauseUpdatePoints_PushButton->setText("resume");
    }
    else
    {
        UpdatePoints_Flag = true;
        PauseUpdatePoints_PushButton->setText("pause");
        SigCH1->Points_Queue.clear();
        SigCH2->Points_Queue.clear();
        SigCH3->Points_Queue.clear();
        SigCH4->Points_Queue.clear();
    }
}

//
//
//
bool ScreenPlot::fout_SigCH_xy(std::ofstream& fout, SignalChannel* sigCH, uint16_t idx)
{
    if(idx >= sigCH->Points_Queue.dispPoints().size()) return false;

    fout << sigCH->Points_Queue.rawPoints_at(idx).x() << "\t";
    fout << sigCH->Points_Queue.rawPoints_at(idx).y() << "\t";
    return true;
}

//
//void ScreenPlot::on_saveDir_selected()
//
void ScreenPlot::on_saveDir_selected(QString dir)
{
    std::string stddir = dir.toStdString();
    const char* ch_dir = stddir.c_str();
    std::ofstream fout(ch_dir);

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");

    fout << "Signal Data File" <<std::endl;
    fout << "generated by ucp_4ch.exe -chenxx" <<std::endl;
    fout << "time = " << str.toStdString()<< std::endl<<std::endl;
    fout << "CH1 data is: " << SigCH1->get_SelectVar_ComboBox()->currentText().toStdString()<< std::endl;
    fout << "CH2 data is: " << SigCH2->get_SelectVar_ComboBox()->currentText().toStdString()<< std::endl;
    fout << "CH3 data is: " << SigCH3->get_SelectVar_ComboBox()->currentText().toStdString()<< std::endl;
    fout << "CH4 data is: " << SigCH4->get_SelectVar_ComboBox()->currentText().toStdString()<< std::endl;

    fout<<std::endl;
    fout << "CH1-t\tCH1-y\tCH2-t\tCH2-y\tCH3-t\tCH3-y\tCH4-t\tCH4-y" <<std::endl;

    uint16_t i = 0;
    while(true)
    {
        bool endFlag = false;

        endFlag = endFlag||fout_SigCH_xy(fout, SigCH1, i);
        endFlag |= fout_SigCH_xy(fout, SigCH2, i);
        endFlag |= fout_SigCH_xy(fout, SigCH3, i);
        endFlag |= fout_SigCH_xy(fout, SigCH4, i);
        fout << std::endl;

        if(endFlag == false) break;
        else
        {
            i++;
            endFlag = false;
        }
    }

    fout.close();

}
//end of file
