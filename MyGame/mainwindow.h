#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>

#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "qserialport.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:

    QPushButton *open_serial;
    QPushButton *push_send;
    QPushButton *auto_send;
    QPushButton *update_serialport;
    QTimer *readTimer;
    QTimer *sendTimer;
    QSerialPort *serial;

    QComboBox *MyBox_Port;
    QComboBox *MyBox_Baudrate;
    QComboBox *MyBox_DataBit;
    QComboBox *MyBox_StopBit;
    QComboBox *MyBox_Jiou;
    QLineEdit *MyEdit_Peiod;
    QComboBox *MyBox_SendData;
    QTextEdit *MyTextEdit_data;

    QCheckBox *Mylabel_HexDisplay;
    QCheckBox *Mylabel_HexSend;
    QCheckBox *MylabelStopDisplaySendData;
    QCheckBox *MylabelStopDisplayReceivedData;


private:

    void send_button(void);

    void readCom(void);
    void open_button(void);

    void autosend_button(void);
    void update_serial(void);

    QByteArray GetHexValue(QString str);
    static char ConvertHexChar(char ch);
    QString ByteArrayToHexString(QByteArray data);
public:
    void setSerialPort(void);
    QString getHexStr(QByteArray temp);
};

#endif // MAINWINDOW_H
