// FaroScannerController.cpp
#include "faroscannercontroller.h"
#include <QMutex>

QPointer<FaroScannerController> FaroScannerController::instancePtr = nullptr;
QMutex FaroScannerController::mutex;

FaroScannerController* FaroScannerController::instance() {
    QMutexLocker locker(&mutex);
    if (!instancePtr) {
        instancePtr = new FaroScannerController();
    }
    return instancePtr;
}

FaroScannerController::FaroScannerController(QObject *parent) : QObject(parent)
{
    iQLibIfPtr = nullptr;
}


bool FaroScannerController::initIiQLibInternal()
{
    try {
        const wchar_t* licenseText1 =
                L"FARO Open Runtime License\n"
                L"Key: 434ELNNRTCTXXMKT8KVUSPUPS\n"
                L"\n"
                L"The software is the registered property of "
                L"FARO Scanner Production GmbH, Stuttgart, Germany.\n"
                L"All rights reserved.\n"
                L"This software may only be used with written permission "
                L"of FARO Scanner Production GmbH, Stuttgart, Germany.";
        BSTR licenseCode1 = SysAllocString(licenseText1);
        IiQLicensedInterfaceIfPtr licPtr1(__uuidof(iQLibIf));
        try {
            licPtr1->License = licenseCode1;
            iQLibIfPtr = static_cast<IiQLibIfPtr>(licPtr1);
        }
        catch (...) {
            qDebug() << "No license for iQOpen interface provided";
            return false;
        }
    }
    catch (...) {
        qDebug() << "Cannot access iQOpen";
        return false;
    }

    if (iQLibIfPtr == NULL) {
        qDebug() << "Failed to initialize iQOpen";
        return false;
    }

    return true;
}








void FaroScannerController::iQLibIfPtrDisconnect()
{
    if (iQLibIfPtr) {
        iQLibIfPtr = nullptr;
    }
}

void FaroScannerController::convertFlsToPly(const QString& inFlsFilePath,const QString& outPlyFilePath)
{
    convertFlsToPly(inFlsFilePath,outPlyFilePath,6,3);
}

void FaroScannerController::convertFlsToPly(const QString &inFlsFilePath,
                                            const QString &outPlyFilePath,
                                            int xyCropDist,
                                            int zCropDist)
{
    initIiQLibInternal();
    HRESULT result;
    std::string stdStr = inFlsFilePath.toStdString();
    const char* cStr = stdStr.c_str();
    _bstr_t bstr(cStr);
    result = iQLibIfPtr->load(cStr);
    qDebug() << "result: " << result;

    int scans = iQLibIfPtr->getNumScans();
    qDebug() << "scans: " << scans;
    int row = iQLibIfPtr->getScanNumRows(0);
    int col = iQLibIfPtr->getScanNumCols(0);
    qDebug() << "row: " << row;
    qDebug() << "col: " << col;

    QString m_ScanNo = "0";
    int scanNo = m_ScanNo.toInt();
    double Rx, Ry, Rz, angle;
    iQLibIfPtr->getScanOrientation(scanNo, &Rx, &Ry, &Rz, &angle);
    qDebug() << Rx << "  " << Ry << "  " << Rz << "  " << angle;

    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    QVector<CartesianPoint> allPoints(row * col);

    auto processRow = [&](int i) {
        QVector<CartesianPoint> rowPoints(col);
        for (int j = 0; j < col; ++j) {
            double x, y, z;
            int refl;
            iQLibIfPtr->getScanPoint(scanNo, i, j, &x, &y, &z, &refl);

            CartesianPoint cartesian;
            cartesian.x = (Rx * Rx * (1 - cosAngle) + cosAngle) * x
                    + (Ry * Rx * (1 - cosAngle) - Rz * sinAngle) * y
                    + (Rz * Rx * (1 - cosAngle) + Ry * sinAngle) * z;

            cartesian.y = (Rx * Ry * (1 - cosAngle) + Rz * sinAngle) * x
                    + (Ry * Ry * (1 - cosAngle) + cosAngle) * y
                    + (Rz * Ry * (1 - cosAngle) - Rx * sinAngle) * z;

            cartesian.z = (Rx * Rz * (1 - cosAngle) - Ry * sinAngle) * x
                    + (Ry * Rz * (1 - cosAngle) + Rx * sinAngle) * y
                    + (Rz * Rz * (1 - cosAngle) + cosAngle) * z;

            cartesian.intensity = refl / 255.0;
            rowPoints[j] = cartesian;
        }
        return rowPoints;
    };

    QVector<QFuture<QVector<CartesianPoint>>> futures;
    for (int i = 0; i < row; ++i) {
        futures.append(QtConcurrent::run(processRow, i));
    }

    int index = 0;
    for (auto &future : futures) {
        future.waitForFinished();
        for (auto &point : future.result()) {
            allPoints[index++] = point;
        }
    }

    syncPlyApi.myXYZData = allPoints.toStdVector();
    syncPlyApi.downSamplePoint(xyCropDist, zCropDist);
    std::ofstream outfile(outPlyFilePath.toStdString());
    syncPlyApi.SavePly(outfile, syncPlyApi.myXYZData);
    syncPlyApi.myXYZData.clear();
    iQLibIfPtrDisconnect();
}


