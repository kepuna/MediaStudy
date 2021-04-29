 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ffmpegs.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    qDebug() << "MainWindow" << QThread::currentThread();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    if (!_audioThread) {
        // 点击开始 录音
        //  开启线程
        _audioThread = new AudioThread(this);
        _audioThread ->start();
         ui->pushButton->setText("结束录音");
    } else {
        // 点击结束录音
        // 结束线程

//        _audioThread->setStop(true);
        _audioThread->requestInterruption(); // QT 中提供的类似stop的功能
        _audioThread = nullptr;
         ui->pushButton->setText("开始录音");
    }
}


// SDL 播放音频有两种模式
//    push（推）：程序 主动推送数据给 音频设备
//    pull（拉）：音频设备 主动向 程序拉取数据 （用的较多）

void MainWindow::on_pushButton_2_clicked()
{
   if(!_playThread) {
       _playThread = new PlayThread(this);
       _playThread->start();

       // 监听线程结束
       connect(_playThread, &PlayThread::finished,[this]() {
            _playThread = nullptr;
             ui->pushButton_2->setText("开始播放");
       });
        ui->pushButton_2->setText("结束播放");

   } else {
       _playThread->requestInterruption(); // QT 中提供的类似stop的功能
       _playThread = nullptr;
       ui->pushButton_2->setText("开始播放");
   }
}

//Stream #0:0: Audio: pcm_f32le, 48000 Hz, mono, flt, 1536 kb/s
void MainWindow::on_pushButton_3_clicked()
{
    //  封装WAV 的头部
    WAVHeader header;
    header.numChannels = 2;
    header.sampleRate = 44100;
    header.bitsPerSample = 16;

    FFmpegs::pcm2wav(header, "/Users/momo/Desktop/Resource/009.pcm", "/Users/momo/Desktop/Resource/004_out.wav");
}

void MainWindow::on_playWavButton_clicked()
{
    if(!_wavPlayThread) {
        _wavPlayThread = new WAVPlayThread(this);
        _wavPlayThread->start();

         // 监听线程结束
        connect(_wavPlayThread, &WAVPlayThread::finished,[this]() {
             _wavPlayThread = nullptr;
              ui->playWavButton->setText("开始播放");
        });

        ui->playWavButton->setText("结束播放");
    } else {
        _wavPlayThread->requestInterruption();
        _wavPlayThread = nullptr;
          ui->playWavButton->setText("开始播放");
    }
}
