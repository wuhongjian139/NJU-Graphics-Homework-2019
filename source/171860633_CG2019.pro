# ！！！
# 源码使用GBK编码，编译前请调整你的IDE前端为GBK编码
# 对于 qt creator，设置 工具-选项-文本编辑器-行为-文件编码 为GBK
# 对于 vs2015/2017/2019，无需修改默认配置
# ！！！
# 动态编译的时候注释掉  main.cpp  里面的
#     #include <QtCore/QtPlugin>
#     Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
# ！！！
# 在非windows平台编译的时候注释掉下面的 RC_FILE += 171860633_CG2019.rc
# ！！！

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/cli.cpp  src/cmd.cpp  src/main.cpp  src/proc.cpp  src/scribblearea.cpp

HEADERS += src/cli.h  src/cmd.h  src/proc.h  src/scribblearea.h

FORMS +=

# RC_ICONS = 171860633_CG2019.ico

RC_FILE += 171860633_CG2019.rc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += src/mainwidget.qrc

TARGET = index

DESTDIR = $${PWD}/../binary
