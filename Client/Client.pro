QT -= gui
QT += network

CONFIG += c++11
CONFIG -= app_bundle


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        AES/qaesencryption.cpp \
        qautostart.cpp \
        transferclient.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    qautostart.h \
    transferclient.h \
    AES/qaesencryption.h \
    AES/aesni/aesni-key-exp.h \
    AES/aesni/aesni-enc-ecb.h \
    AES/aesni/aesni-enc-cbc.h

RC_ICONS = logo.ico

