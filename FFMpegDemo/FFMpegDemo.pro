QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audiothread.cpp \
    ffmpegs.cpp \
    main.cpp \
    mainwindow.cpp \
    playthread.cpp \
    wavplaythread.cpp

HEADERS += \
    audiothread.h \
    ffmpegs.h \
    mainwindow.h \
    playthread.h \
    wavplaythread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


macx {
    FFMPEG_HOME =  /usr/local/Cellar/ffmpeg/4.4
     SDL_HOME =  /usr/local/Cellar/sdl2/2.0.14_1
    QMAKE_INFO_PLIST = mac/Info.plist



#    # 头文件路径
#    INCLUDEPATH += $${FFMPEG_HOME}/include

#    # 设置库文件路径
#    LIBS += -L $${FFMPEG_HOME}/lib \
#        -lavcodec \
#        -lavdevice \
#        -lavfilter \
#        -lavformat \
#        -lavutil \
#        -lpostproc \
#        -lswscale \
#        -lswresample \
#        -lavresample

}

# 头文件路径
INCLUDEPATH +=  $${FFMPEG_HOME}/include

# 设置库文件路径
LIBS += -L$${FFMPEG_HOME}/lib \
    -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswscale \
    -lswresample \
    -lavresample

INCLUDEPATH += $${SDL_HOME}/include
LIBS += -L$${SDL_HOME}/lib \
        -lSDL2


DISTFILES += \
    mac/Info.plist

# message（） 可以用来打印
# $${ } 可以用来取值： .pro 中定义的变量
# $$( ) 可以用来取值：系统环境变量中的值

# ⚠️ 修改完pro文件需要重新构建一下
