
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
	// ��ȡ�����ľ�̬����  
	static FaroConvert& getInstance() {
		if (!instance) {
			instance = new FaroConvert();
		}
		return *instance;
	}

	// �����ķ���  
    bool convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath);
    bool convertFlsToPly(const std::string& inFlsFilePath, const std::string& outPlyFilePath, int xyCropDist, int zCropDist);

private:
	FaroConvert() {} // ˽�еĹ��캯������ֹ�ⲿʵ����  
	FaroConvert(const FaroConvert&) = delete; // ɾ���������캯��  
	FaroConvert& operator=(const FaroConvert&) = delete; // ɾ����ֵ������  
	static FaroConvert* instance;

	// ��ʼ��iQLib�ķ���  
	bool initIiQLibInternal();
    void iQLibIfPtrDisconnect();
	// ��Ա����  
	SyncPlyApi syncPlyApi;
	// ���Ǵ�FARO SDK�е����ָ������  
	IiQLibIfPtr iQLibIfPtr;    
};

