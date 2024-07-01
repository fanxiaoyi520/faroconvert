
#include <memory>  
#include <string>
#include <stdio.h>
#include <iostream>
#include <atlcomcli.h>
#include <comutil.h>
#include <cstdlib>
#include "syncplyapi.h"
#include <vector>  
#include <cmath>  
#include <future> 
#include <thread>


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

#pragma once
class FaroConvert
{
public:
	// 获取单例的静态方法  
	static FaroConvert& getInstance() {
		if (!instance) {
			instance = new FaroConvert();
		}
		return *instance;
	}

	// 公开的方法  
    bool convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath);
    bool convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath, int xyCropDist, int zCropDist);

private:
	FaroConvert() {} // 私有的构造函数，防止外部实例化  
	FaroConvert(const FaroConvert&) = delete; // 删除拷贝构造函数  
	FaroConvert& operator=(const FaroConvert&) = delete; // 删除赋值操作符  
	static FaroConvert* instance;

	// 初始化iQLib的方法  
	bool initIiQLibInternal();
    void iQLibIfPtrDisconnect();
	// 成员变量  
	SyncPlyApi syncPlyApi;
	// 这是从FARO SDK中导入的指针类型  
	IiQLibIfPtr iQLibIfPtr;    
};

