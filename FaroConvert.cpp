#include "FaroConvert.h"
#include <windows.h>

FaroConvert* FaroConvert::instance = nullptr;

bool FaroConvert::initIiQLibInternal() {
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
            std::cout << "No license for iQOpen interface provided";
            return false;
        }
    }
    catch (...) {
        std::cout << "Cannot access iQOpen";
        return false;
    }

    if (iQLibIfPtr == NULL) {
        std::cout << "Failed to initialize iQOpen";
        return false;
    }

    return true;
}

// convertFlsToPly?????????????????????จน??ทฺ??????
bool FaroConvert::convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath) {
    return convertFlsToPly(inFlsFilePath, outPlyFilePath, 6, 3);
}

// ???จน???convertFlsToPly???????????????
bool FaroConvert::convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath, int xyCropDist, int zCropDist) {
    if (!iQLibIfPtr) {
        bool re = initIiQLibInternal();
        if (!re) return re;
    }
    HRESULT result;
    std::cout << "path: " << inFlsFilePath << std::endl;
    std::string stdStr = inFlsFilePath;
    const char* cStr = stdStr.c_str();
    _bstr_t bstr(cStr);
    std::cout << "start convert" << std::endl;
    result = iQLibIfPtr->load(cStr);
    std::cout << "in process ..." << std::endl;
    std::cout << "result: " << result << std::endl;
    int scans = iQLibIfPtr->getNumScans();
    std::cout << "scans: " << scans << std::endl;
    if(scans == 0 || result != 0){
        std::cerr << "*** data error *** " << std::endl;;
        iQLibIfPtrDisconnect();
        return false;
    }

    int row = iQLibIfPtr->getScanNumRows(0);
    int col = iQLibIfPtr->getScanNumCols(0);
    std::cout << "row: " << row << std::endl;
    std::cout << "col: " << col << std::endl;
    int scanNo = 0;
    double Rx, Ry, Rz, angle;
    iQLibIfPtr->getScanOrientation(scanNo, &Rx, &Ry, &Rz, &angle);
    std::cout << Rx << "  " << Ry << "  " << Rz << "  " << angle << std::endl;

    double cosAngle = cos(angle);
    double sinAngle = sin(angle);

    std::vector<CartesianPoint> allPoints(row * col);

    auto processRow = [&](int i) {
        std::vector<CartesianPoint> rowPoints(col);
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

    std::vector<std::future<std::vector<CartesianPoint>>> futures;
    for (int i = 0; i < row; ++i) {
        futures.emplace_back(std::async(std::launch::async, processRow, i));
    }

    int index = 0;
    for (auto &future : futures) {
        future.wait();
        for (auto &point : future.get()) {
            allPoints[index++] = point;
        }
    }

    syncPlyApi.myXYZData = allPoints;
    syncPlyApi.downSamplePoint(xyCropDist, zCropDist);
    std::ofstream outfile(outPlyFilePath);
    syncPlyApi.SavePly(outfile, syncPlyApi.myXYZData);
    syncPlyApi.myXYZData.clear();
    iQLibIfPtrDisconnect();
    std::cout << "****** complete !!! ******" << std::endl;
    return true;
}

void FaroConvert::iQLibIfPtrDisconnect()
{
    if (iQLibIfPtr) {
        iQLibIfPtr = nullptr;
    }
}
