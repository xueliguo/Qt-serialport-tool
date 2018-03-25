#include "mainwindow.h"

#include <QDebug>
#include <QBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{    
    this->setWindowTitle("Serial Tool");
    this->resize( QSize( 800, 600 ));
    this->setFixedSize(800,600);

    QWidget *Mywindow = new QWidget();

    QLabel *Mylabel_Port = new QLabel("端口号");
    QLabel *Mylabel_Baudrate = new QLabel("波特率");
    QLabel *Mylabel_DataBit= new QLabel("数据位");
    QLabel *Mylabel_StopBit = new QLabel("停止位");
    QLabel *Mylabel_Jiou = new QLabel("奇偶位");

    Mylabel_HexDisplay = new QCheckBox("十六进制显示");
    Mylabel_HexSend = new QCheckBox("十六进制发送");
    MylabelStopDisplaySendData = new QCheckBox("停止显示发送");
    MylabelStopDisplayReceivedData = new QCheckBox("停止显示接收数据");

    QLabel *Mylabel_Period = new QLabel("周期");
    QLabel *Mylabel_Ms = new QLabel("ms");

    MyBox_Port = new QComboBox();
    MyBox_Port->addItem("ttyS0");
    MyBox_Port->addItem("ttyS1");
    MyBox_Port->addItem("ttyS2");
    MyBox_Port->addItem("ttyS3");
    MyBox_Baudrate = new QComboBox();
    MyBox_Baudrate->addItem(tr("57600"));
    MyBox_DataBit = new QComboBox();
    MyBox_DataBit->addItem(tr("8"));

    MyBox_StopBit = new QComboBox();
    MyBox_StopBit->addItem(tr("1"));

    MyBox_Jiou = new QComboBox();
    MyBox_Jiou->addItem(tr("NUll"));

    MyEdit_Peiod = new QLineEdit();

    MyBox_SendData = new QComboBox();

    MyBox_SendData->setEditable(true);

    MyTextEdit_data = new QTextEdit();


    open_serial = new QPushButton("打开串口",this);
    open_serial->setFixedSize(120,40);
    push_send = new QPushButton("发送",this);
    push_send->setFixedSize(120,40);
    auto_send = new QPushButton("自动发送");
    auto_send->setFixedSize(120,40);

    update_serialport = new QPushButton("更新串口");
    update_serialport->setFixedSize(120,40);

   QGridLayout *layout = new QGridLayout();

    layout->addWidget(Mylabel_Port,0,0,1,1);
    layout->addWidget(MyBox_Port,0,1,1,2);

    layout->addWidget(Mylabel_Baudrate,1,0,2,1);
    layout->addWidget(MyBox_Baudrate,1,1,2,2);

    layout->addWidget(Mylabel_DataBit,2,0,3,1);
    layout->addWidget(MyBox_DataBit,2,1,3,2);

    layout->addWidget(Mylabel_StopBit,3,0,4,1);
    layout->addWidget(MyBox_StopBit,3,1,4,2);

    layout->addWidget(Mylabel_Jiou,4,0,5,1);
    layout->addWidget(MyBox_Jiou,4,1,5,2);

    layout->addWidget(open_serial,6,1,8,3);

    layout->addWidget(Mylabel_HexDisplay,8,0,9,2);
    layout->addWidget(Mylabel_HexSend,9,0,10,2);
    layout->addWidget(MylabelStopDisplaySendData,10,0,11,2);
    layout->addWidget(MylabelStopDisplayReceivedData,11,0,12,2);

    layout->addWidget(Mylabel_Period,13,0,14,1);
    layout->addWidget(MyEdit_Peiod,13,1,14,2);
    layout->addWidget(Mylabel_Ms,13,3,14,4);


    layout->addWidget(MyTextEdit_data,0,4,26,14);

    layout->addWidget(MyBox_SendData,17,4,18,14);


    layout->addWidget(push_send,15,1,16,2);
    layout->addWidget(auto_send,17,1,18,2);

    layout->addWidget(update_serialport,19,1,20,2);

    Mywindow->setLayout(layout);
    this->setCentralWidget(Mywindow);

    readTimer = new QTimer(this);
    connect(readTimer,&QTimer::timeout,this,&MainWindow::readCom);
    //readTimer->start(10);
    sendTimer = new QTimer(this);
    connect(sendTimer,&QTimer::timeout,this,&MainWindow::send_button);


    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        MyTextEdit_data->append("Serialport is not available!");



    }else MyTextEdit_data->append("Serialport is available!");

    foreach (QSerialPortInfo info, infos) {
    MyBox_Port->addItem(info.portName());
   }
    serial = new QSerialPort(this);
    connect(open_serial,&QPushButton::clicked,this,&MainWindow::open_button);
    connect(update_serialport,&QPushButton::clicked,this,&MainWindow::update_serial);
     connect(push_send,&QPushButton::clicked,this,&MainWindow::send_button);

}

MainWindow::~MainWindow()
{
    serial->close();
    delete serial;
}

void MainWindow::setSerialPort()
{

    QString serialname ="/dev/" + MyBox_Port->currentText();

    MyTextEdit_data->append("set name is :"+serialname);

    serial->setPortName(serialname);

    MyTextEdit_data->append("serial port name is :"+serial->portName());

    serial->setBaudRate((QSerialPort::BaudRate)MyBox_Baudrate->currentText().toInt());
    serial->setDataBits((QSerialPort::DataBits)(MyBox_DataBit->currentText().toInt()));

    switch(MyBox_StopBit->currentIndex()){
    case 0:
        serial->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        break;
    case 2:
        serial->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        serial->setStopBits(QSerialPort::UnknownStopBits);
        //qDebug("set to default : STOP_1");
        break;
    }

    switch(MyBox_Jiou->currentIndex()){
    case 0:
         serial->setParity(QSerialPort::NoParity);
         break;
    case 1:
        serial->setParity(QSerialPort::OddParity);
        break;
    case 2:
        serial->setParity(QSerialPort::EvenParity);
        break;
    default:
        serial->setParity(QSerialPort::UnknownParity);

        //qDebug("set to default : PAR_NONE");
        break;
    }

    serial->setFlowControl(QSerialPort::NoFlowControl);

}
void MainWindow::update_serial()
{
    MyBox_Port->clear();

    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
    MyBox_Port->addItem(info.portName());
    }
    MyBox_Port->addItem("ttyS0");
    MyBox_Port->addItem("ttyS1");
    MyBox_Port->addItem("ttyS2");
    MyBox_Port->addItem("ttyS3");
}

char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

QByteArray MainWindow::GetHexValue(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

void MainWindow::send_button(void)
{

    QString str=MyBox_SendData->currentText();
    QByteArray outData=str.toLatin1();
    QDateTime m_time = QDateTime::currentDateTime();
    int size=outData.size();

    //如果串口没有打开
    if (!serial->isOpen())
    {
        return;
    }
    //如果发送数据为空
    if (outData.isEmpty())
    {
        MyBox_SendData->setFocus();
        return;
    }

    //linux下的回车符为一个字符\r
    if (outData[size-1]!='\r')
    {
        outData.resize(size+1);
        outData[size]='\r';
    }

    //默认16进制发送不加入回车符
    if (Mylabel_HexSend->isChecked())//转化为16进制发送
    {

        outData = GetHexValue(str);
        size=outData.size();
        serial->write(outData);
        QByteArray temp=outData;
        QString strHex;//16进制数据
        QDataStream out(&temp,QIODevice::ReadWrite);
        while (!out.atEnd())
        {
            qint8 outChar=0;
            out>>outChar;
            QString str=QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));

            if (str.length()>1)
            {
                strHex+=str+" ";
            }
            else
            {
                strHex+="0"+str+" ";
            }
        }
        if(!Mylabel_HexDisplay->isChecked())

            MyTextEdit_data->append(m_time.toString(tr("[yyyy-MM-dd hh:mm:ss][发送]"))+"[HEX]"+strHex.toUpper());
    }
    else
    {

        size=outData.size();
        serial->write(outData);

        if(!MylabelStopDisplaySendData->isChecked())
            MyTextEdit_data->append(m_time.toString(tr("[yyyy-MM-dd hh:mm:ss][发送]"))+"[ASCII]"+outData);
    }

}

QString MainWindow::ByteArrayToHexString(QByteArray data){
    QString ret(data.toHex().toUpper());
    int len = ret.length()/2;
    qDebug()<<len;
    for(int i=1;i<len;i++)
    {
        qDebug()<<i;
        ret.insert(2*i+i-1," ");
    }

    return ret;
}

void MainWindow::readCom(void)
{
    QDateTime m_time = QDateTime::currentDateTime();
    QString strHex;//16进制数据
    QString strNomal;//正常数据
    if(MylabelStopDisplayReceivedData->isChecked())
    {

    }
    else
    {
    if(serial)
    {
        if( serial->bytesAvailable() >= 4)
        {
            QByteArray temp=serial->readAll();
            if (!temp.isEmpty() /*&& IsShow*/)
            {

                strHex = ByteArrayToHexString(temp);
                strNomal = temp;

                    if (Mylabel_HexDisplay->isChecked())
                    {
                        MyTextEdit_data->append(m_time.toString(tr("[yyyy-MM-dd hh:mm:ss][接收]"))+"[Hex]"+strHex.toUpper());
                    }
                    else
                    {
                        MyTextEdit_data->append(m_time.toString(tr("[yyyy-MM-dd hh:mm:ss][接收]"))+"[ASCII]"+strNomal);
                    }

                qDebug()<<m_time.toString("[yyyy-MM-dd hh:mm:ss][接收]")<<strHex;
            }
        }
    }
    else
    {
        qDebug()<<"Com is null.";
    }
    }





}
void MainWindow::open_button()
{

    setSerialPort();

    if(open_serial->text() == "打开串口")
    {
          MyTextEdit_data->append("点击打开串口");
        if(serial->open(QIODevice::ReadWrite))
        {
            MyTextEdit_data->append("打开串口中。。。。");
            open_serial->setText("关闭串口");
            auto_send->setDisabled(false);
            push_send->setDisabled(false);

            MyBox_Baudrate->setDisabled(true);
            MyBox_DataBit->setDisabled(true);
            MyBox_Jiou->setDisabled(true);
            MyBox_Port->setDisabled(true);
            MyBox_StopBit->setDisabled(true);
            MyBox_SendData->setDisabled(false);
            update_serialport->setDisabled(true);
            readTimer->start(10);

        }else{

                QMessageBox::critical(this,tr("open Failed"),tr("can't open ")+serial->portName());
                serial->close();

             }

    }else {

                MyTextEdit_data->append("点击关闭串口");
                readTimer->stop();
                serial->close();
                if(serial->isOpen() == true) serial->close();

                open_serial->setText("打开串口");
                auto_send->setDisabled(true);
                push_send->setDisabled(true);

                MyBox_Baudrate->setDisabled(false);
                MyBox_DataBit->setDisabled(false);
                MyBox_Jiou->setDisabled(false);
                MyBox_Port->setDisabled(false);
                MyBox_StopBit->setDisabled(false);
                MyBox_SendData->setDisabled(true);
                update_serialport->setDisabled(false);

        }

}

void MainWindow::autosend_button()
{

}



