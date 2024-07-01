#ifndef FAROSCANNERCONTROLLER_H
#define FAROSCANNERCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <atlcomcli.h>
#include <comutil.h>
#include <cstdlib>
//#include "filemanager.h"
//#include "networkhelper.h"
#include <QTimer>
#include <QStorageInfo>
#include <QMutex>
#include <QPointer>
#include "syncplyapi.h"
#include <cmath>
#include <QThread>
#include <QtConcurrent>
#include <QFuture>
#include <functional>

#ifdef _WIN64
// Yes - type is 'win32' even on WIN64!
#pragma comment(linker, "\"/manifestdependency:type='win32' name='FARO.LS' version='1.1.0.0' processorArchitecture='amd64' publicKeyToken='1d23f5635ba800ab'\"")
#else
#pragma comment(linker, "\"/manifestdependency:type='win32' name='FARO.LS' version='1.1.0.0' processorArchitecture='x86' publicKeyToken='1d23f5635ba800ab'\"")
#endif
//#import "C:\Windows\WinSxS\amd64_faro.ls_1d23f5635ba800ab_1.1.702.0_none_3590af4b356bcd81\iQOpen.dll" no_namespace
//#import "C:\Windows\WinSxS\amd64_faro.ls_1d23f5635ba800ab_1.1.702.0_none_3590af4b356bcd81\FARO.LS.SDK.dll" no_namespace
#import "thirdparty/iQOpen.dll" no_namespace
#import "thirdparty/FARO.LS.SDK.dll" no_namespace
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

class FaroScannerController : public QObject
{
    Q_OBJECT

public:
    static FaroScannerController* instance();



    // 转换fls文件为ply文件~含下采样
    Q_INVOKABLE void convertFlsToPly(const QString& inFlsFilePath,const QString& outPlyFilePath);
    Q_INVOKABLE void convertFlsToPly(const QString& inFlsFilePath,const QString& outPlyFilePath,int xyCropDist,int zCropDist);

private:

    FaroScannerController(QObject *parent = nullptr);
    FaroScannerController(const FaroScannerController&) = delete;
    FaroScannerController& operator=(const FaroScannerController&) = delete;
    static QPointer<FaroScannerController> instancePtr;
    static QMutex mutex;
    IiQLibIfPtr iQLibIfPtr;

    bool initIiQLibInternal();
    void iQLibIfPtrDisconnect();
    SyncPlyApi syncPlyApi;
};

#endif // FAROSCANNERCONTROLLER_H


