#include "mainwindow.h"

#include <QApplication>
#include <QtDebug>

extern "C" {
    #include <libavcodec/avcodec.h>

// 格式相关API
    #include <libavformat/avformat.h>
// 设备相关API
    #include<libavdevice/avdevice.h>
// 工具相关API （比如错误处理）
    #include<libavutil/avutil.h>
}

int main(int argc, char *argv[])
{
    // 打印ffmpeg 版本号
    qDebug() << av_version_info();

      // 注册设备 （整个程序允许过程中，只需要注册一次 ）
    avdevice_register_all();

    // FFmpeg
//    av_log_set_level(AV_LOG_DEBUG);
//    av_log(nullptr, AV_LOG_ERROR, "av_log---✅");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
