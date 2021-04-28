#include "audiothread.h"
#include <QFile>
#include <QThread>
#include <QtDebug>

extern "C" {

// 格式相关API
    #include <libavformat/avformat.h>
// 设备相关API
    #include<libavdevice/avdevice.h>
// 工具相关API （比如错误处理）
    #include<libavutil/avutil.h>
}


#ifdef Q_OS_WIN
    // PCM文件的文件名
    #define FILENAME "F:/out.pcm"
#else
    #define FILENAME "/Users/momo/Desktop/out.pcm"
#endif

AudioThread::AudioThread(QObject *parent) : QThread(parent)
{
    // 当监听到线程结束时（fnished） 调用 deleteLater 自动回收线程的内存
       connect(this, &AudioThread::finished,
               this, &AudioThread::deleteLater);
}

AudioThread::~AudioThread() {

    // 这个对象关联的信号 都给我断开
    disconnect();

    // 内存回收之前， 使正常结束线程
    requestInterruption();

    // 安全退出
    quit();
    wait();
      qDebug() << this  <<  "析构内存被回收" ;
}
// 当线程启动的时候（start）， 就会自动调用run函数
// run函数中的代码是在子线程中执行的
void AudioThread::run() {

    // 根据输入格式名称 获取输入格式对象
    AVInputFormat *fmt = av_find_input_format("avfoundation");
    if(!fmt) {
        qDebug() << "找不到输入格式" <<  "avfoundation" ;
        return;
    }

    // 设备名称
    // :0 代表第0个音频设备
    // 0 代表第0个视频设备
    // 0:0 第0个视频设备 和 第0个音频设备
    const  char *deviceName = ":0";

    // 格式上下文，后面通过格式上下文操作设备
    AVFormatContext *ctx = nullptr;

    //  open device
    AVDictionary *options = nullptr;
    int ret = avformat_open_input(&ctx, deviceName,fmt,&options);

    // 如果打开失败
    if (ret < 0 ) {
        char errorbuf[1024] = {0};
        // 根据函数返回的错误码 获取错误信息
        av_strerror(ret, errorbuf, sizeof (errorbuf) );
        qDebug() << "打开设备失败" << errorbuf;
        return;
    }

  // 文件
    QFile file(FILENAME);

    // WriteOnly :只写模式， 如果文件不存在 ，就创建文件； 如果文件存在，就删除文件内容
    if( !file.open(QFile::WriteOnly) ) {
         qDebug() << "文件打开失败" << FILENAME;
         // 关闭设备
         avformat_close_input(&ctx);
         return;
    }

    AVPacket pkt;
    // 可以通过isInterruptionRequested判断是否要结束线程
    // 当调用过线程的requestInterruption时，isInterruptionRequested返回值就为true，否则为false
    while (!isInterruptionRequested() && av_read_frame(ctx, &pkt) == 0 ) {
        file.write((const char *) pkt.data, pkt.size);
    }

    file.close();
    avformat_close_input(&ctx);
    qDebug() << this <<  "+++ 线程正常结束 +++" ;

    /*
    //  AVPacket 代表read出来的数据包
    // 采集的数据在 pkt.data 里面
    AVPacket *pkt = av_packet_alloc();

      // 采集一次数据   av_read_frame(ctx, &pkt);
    qDebug() << "🆕开始录音中======";

    // 不断采集
    while (!_stop) {

         // 从设备中采集数据，返回值为0，代表采集数据成功
        ret = av_read_frame(ctx, pkt);
         qDebug() << "结果" << ret;
//        if(ret == 0) { // 读取成功
//            // 将数据写入文件
//            file.write((const char *)pkt->data, pkt->size);
//        } else if (ret == AVERROR(EAGAIN)) {
//            // 资源临时不可用
//             qDebug() << "资源临时不可用";
//            continue;
//        } else {
//            // 其他错误
//            char errbuf[1024];
//            av_strerror(ret, errbuf, sizeof (errbuf));
//             qDebug() << "❌av_read_frame error" << errbuf << ret;
//             break;
//        }
//        // 释放资源
//        av_packet_unref(pkt);
//         qDebug() << "录音中======";
    }

    qDebug() << "+++ 线程正常结束 +++" ;

    // 直接播放PCM时播放不了，因为不知道采样率，声道数，大小端模式等无法解析

    // 关闭文件
    file.close();

    // 释放资源
    av_packet_free(&pkt);

     // 关闭设备
    avformat_close_input(&ctx);

    */
}

void AudioThread::setStop(bool stop) {
    qDebug() << "+++ stop +++" << stop;
    _stop = stop;
}
