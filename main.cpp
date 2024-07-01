#include <QCoreApplication>
#include "FaroConvert.h"
#include <QGuiApplication>
#include "faroscannercontroller.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: your_program input.fls output.ply" << std::endl;
        return 1;
    }

    std::string inFlsFilePath = argv[1];
    std::string outPlyFilePath = argv[2];

    QGuiApplication app(argc, argv);

    bool success = FaroConvert::getInstance().convertFlsToPly(inFlsFilePath, outPlyFilePath);
    if (!success) {
        std::cerr << "Error: Conversion failed." << std::endl;
        return 1;
    }
       return 0;
}
