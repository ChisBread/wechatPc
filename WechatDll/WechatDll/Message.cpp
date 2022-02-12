#include "stdafx.h"
#include "resource.h"
#include "Config.h"
#include "LoginInfo.h"
#include "WechatOffset.h"
#include "Common.h"
#include "Struct.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h> 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <strstream>
//#include "Download.h"
#include "Message.h"
#include "Send.h"

using namespace std;

// ��Ϣ����
typedef queue<WebsocketMessageStruct> DataQueue;
// ȫ�ֶ���
DataQueue *SendDataQueue;

//���ص�ַ
DWORD jumBackAddress = 0;
//����Ҫ��ȡ�ļĴ�������
DWORD r_esp = 0;
DWORD recvMessageCall = 0;

/*==================================��Ϣ������ʼ=======================================*/
VOID RecieveMsg(DWORD esp)
{
	WebsocketMessageStruct *message = new WebsocketMessageStruct;
	
	//[[esp]]
	//��Ϣ��λ��
	DWORD** msgAddress = (DWORD * *)esp;

	//��Ϣ����[[esp]]+0x30
	message->msgType = (int)*((DWORD*)(**msgAddress + 0x30));
	/*
	switch (message->msgType) {
		case 0x01:  // ����
			break;
		case 0x03:  // ͼƬ
			break;
		case 0x22:  // ����
			break;
		case 0x25:  // ����ȷ��
			break;
		case 0x28:  // POSSIBLEFRIEND_MSG
			break;
		case 0x2A:  // ��Ƭ
			break;
		case 0x2B:  // ��Ƶ
			break;
		case 0x2F:  // ʯͷ������ | ����ͼƬ
			break;
		case 0x30:  // λ��
			break;
		case 0x31:  // ����ʵʱλ�á��ļ���ת�ˡ�����
			break;
		case 0x32:  // VOIPMSG
			break;
		case 0x33:  // ΢�ų�ʼ��
			break;
		case 0x34:  // VOIPNOTIFY
			break;
		case 0x35:  // VOIPINVITE
			break;
		case 0x3E:  // С��Ƶ
			break;
		case 0x270F:  // SYSNOTICE
			break;
		case 0x2710:  // �����ϵͳ��Ϣ
			break;
		case 0x2712:  // ������Ϣ
			break;
		default:  // δ֪

			break;
	}
	*/

	//dc [[[esp]] + 0x16C]
	//�ж���Ⱥ��Ϣ���Ǻ�����Ϣ
	wstring msgSource = _T("");
	msgSource.append(GetMsgByAddress(**msgAddress + 0x16C));
	if (msgSource.find(L"</membercount>") == wstring::npos) {
		// �յ�������Ϣ
		message->msgSource = 0;
	} else {
		// �յ�Ⱥ��Ϣ
		message->msgSource = 1;
	}
	//������Ϣ
	if (message->msgSource == 0) {
		// ����΢��ID
		wstring wxid = GetMsgByAddress(**msgAddress + 0x40);
		sprintf_s(message->wxid, 0x100, "%s", UnicodeToUtf8((wchar_t*)wxid.c_str()));
	} else {
		// ȺID
		wstring roomId = GetMsgByAddress(**msgAddress + 0x40);
		sprintf_s(message->roomId, 0x100, "%s", UnicodeToUtf8((wchar_t*)roomId.c_str()));
		// ��Ϣ������
		wstring wxid = GetMsgByAddress(**msgAddress + 0x118);
		sprintf_s(message->wxid, 0x100, "%s", UnicodeToUtf8((wchar_t*)wxid.c_str()));
	}

	// �����Ϣ
	sprintf_s(message->msgSourceXml, 0x2048, "%s", UnicodeToUtf8((wchar_t*)msgSource.c_str()));

	// ��Ϣ����
	wstring content = GetMsgByAddress(**msgAddress + 0x68);
	sprintf_s(message->content, 0x8092, "%s", UnicodeToUtf8((wchar_t*)content.c_str()));

	// ��ϢID
	wstring msgId = GetMsgByAddress(**msgAddress + 0x12C);
	sprintf_s(message->msgId, 0x100, "%s", UnicodeToUtf8((wchar_t*)msgId.c_str()));

	// �Ƿ��Լ�������Ϣ��0=��1=��
	message->isOwner = (int)*((DWORD*)(**msgAddress + 0x34));
	
	//_WaitingSendData *a = new _WaitingSendData;
	WebsocketMessageStruct a = *message;
	SendDataQueue->push(a);

	// 0x140  ����ͼ���·��   supper-busy\FileStorage\Image\Thumb\2020-03\0adce5xxx.dat
	// 0x154  ��ͼ���·��     supper-busy\FileStorage\Image\2020-03\0adce5d47857xxxx.dat
}



/**
 * ��д�뵽hook��Ľ�����Ϣ�㺯��
 */
__declspec(naked) void RecieveMsgHook()
{
	__asm {
		// ���䱻���ǵĴ���
		// 0F4B28C3    B9 20FA7810     mov ecx,WeChatWi.1078FA20
		mov ecx, recvMessageCall

		//��ȡesp�Ĵ������ݣ�����һ��������
		//����ʹ��ȫ�ֱ��� r_esp�������¶�ʧ��Ϣ��ԭ�򣺶��߳�����£������ݸ��Ǿ�����
		//��������Խ�espֱ��ѹ���ջ(esp�䶯��)��Ȼ����RecieveMsg�����н��м���
		mov r_esp, esp

		//����Ĵ���
		pushad
		pushf
	}

	//���ý�����Ϣ�ĺ���
	RecieveMsg(r_esp);

	//�ָ��ֳ�
	__asm
	{
		popf
		popad

		//���ر�HOOKָ�����һ��ָ��
		jmp jumBackAddress
	}
}
/*
���Ѷ��� - �����Ͷ��е���Ϣ�ط����
*/
DWORD WINAPI QueueSendMesssage()
{
	while (TRUE) {
		if (SendDataQueue->size() <= 0) {
			Sleep(200);
			continue;
		}
		WebsocketMessageStruct message = SendDataQueue->front();
		SendDataQueue->pop();
		Send::SendWxMessage(&message);
	}
	
	return TRUE;
}
/*
ע����Ϣ����
*/
VOID ListenMessage()
{
	// ����һ���̣߳������Ͷ��е���Ϣ
	SendDataQueue = new DataQueue;  // ��ʼ������
	// �����߳�
	HANDLE cThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)QueueSendMesssage, NULL, NULL, 0);
	if (cThread != 0) {
		CloseHandle(cThread);
	}

	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	//WeChatWin.dll + LISTEN_MESSAGE_CALL
	int hookAddress = winAddress + LISTEN_MESSAGE_HOOK;
	recvMessageCall = winAddress + LISTEN_MESSAGE_CALL;

	//���صĵ�ַ
	jumBackAddress = hookAddress + 5;

	//��װ��ת����
	BYTE jmpCode[5] = { 0 };
	jmpCode[0] = 0xE9;

	//����תָ���е�����=��ת�ĵ�ַ-ԭ��ַ��HOOK�ĵ�ַ��-��תָ��ĳ���
	*(DWORD*)& jmpCode[1] = (DWORD)RecieveMsgHook - hookAddress - 5;

	//����ָ�� 
	// 5A665373   .  B9 B8716D5B   mov ecx,WeChatWi.5B6D71B8
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookAddress, jmpCode, 5, 0);
}
/*==================================��Ϣ��������=======================================*/

/*==================================�����ı���Ϣ��ʼ=======================================*/
VOID SendTextMessage(wchar_t *wxid, wchar_t *message)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	DWORD sendCallAddress = winAddress + SEND_MESSAGE_CALL;

	// �����˵�΢��ID
	wxMessageStruct wxWxid = { 0 };
	wxWxid.text = wxid;
	wxWxid.textLength = wcslen(wxid);
	wxWxid.textLen = wcslen(wxid) * 2;

	// ΢����Ϣ
	wxMessageStruct wxMessage = {0};
	wxMessage.text = message;
	wxMessage.textLength = wcslen(message);
	wxMessage.textLen = wcslen(message) * 2;
	
	// @���Ǹ���
	//wxMessageStruct wxToWxid = {0};
	//wxMessage.text = toWxid;
	//wxMessage.textLength = wcslen(toWxid);
	//wxMessage.textLen = wcslen(toWxid) * 2;

	// ȡ΢��ID��΢����Ϣ���ڴ��ַ
	char *pWxid = (char *)&wxWxid.text;
	char *pMsg = (char *)&wxMessage.text;
	//char *pToWxId = (char *)&wxToWxid.text;

	char buff[0x87C] = {0};

	__asm {
		pushad
		mov edx, pWxid
		mov eax, 0x0
		push 0x1
		push eax
		mov ebx, pMsg
		push ebx
		lea ecx, buff
		call sendCallAddress
		add esp, 0xC
		popad
	}
}
/*==================================�����ı���Ϣ����=======================================*/

/*==================================����XML��Ƭ��ʼ=======================================*/
void SendXmlCardMessage(wchar_t* recverWxid, wchar_t* xmlData)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();
	
	SendXmlCardWxString pWxid = { 0 };
	pWxid.pStr = recverWxid;
	pWxid.strLen = wcslen(recverWxid);
	pWxid.strMaxLen = wcslen(recverWxid) * 2;

	SendXmlCardWxString pXml = { 0 };
	pXml.pStr = xmlData;
	pXml.strLen = wcslen(xmlData);
	pXml.strMaxLen = wcslen(xmlData) * 2;

	char* asmWxid = (char *)&pWxid.pStr;
	char* asmXml = (char *)&pXml.pStr;
	char buff[0x20C] = { 0 };
	DWORD callAdd = winAddress + WX_SEND_XML_CARD;
	
	__asm {
		pushad
		mov eax, asmXml
		push 0x2A
		mov edx, asmWxid
		push 0x0
		push eax
		lea ecx, buff
		call callAdd
		add esp, 0xC
		popad
	}
}
/*==================================����XML��Ƭ����=======================================*/


/*==================================����ͼƬ��ʼ=======================================*/
void SendImageMessage(wchar_t* wxid, wchar_t* filepath)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	//��װ΢��ID�����ݽṹ
	SendImageWxidStruct imagewxid;
	imagewxid.pWxid = wxid;
	imagewxid.length = wcslen(wxid);
	imagewxid.maxLength = wcslen(wxid) * 2;

	//��װ�ļ�·�������ݽṹ
	SendImageFileStruct imagefilepath;
	imagefilepath.filePath = filepath;
	imagefilepath.length = wcslen(filepath);
	imagefilepath.maxLength = wcslen(filepath) * 2;

	char buff[0x45C] = { 0 };

	DWORD dwCall1 = winAddress + WX_SEND_IMAGE_CALL1;
	DWORD dwCall2 = winAddress + WX_SEND_IMAGE_CALL2;

	//΢�ŷ���ͼƬ������GidCreateBimapFileCM ֮��ͼƬһֱ��ռ�� �޷�ɾ�� patch������Ĵ���
	unsigned char oldcode[5] = { 0 };
	// {0x31,0xC0} = xor eax,eax
	// {0xC3} = return
	unsigned char fix[5] = { 0x31,0xC0,0xC3,0x90,0x90 };
	DWORD dwPathcAddr = winAddress + WX_SEND_IMAGE_PATCH_ADDR;
	//�޸Ĵ��������
	DWORD dwOldAttr = 0;
	VirtualProtect((LPVOID)dwPathcAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldAttr);
	//�ȱ���ԭ��ָ��
	memcpy(oldcode, (LPVOID)dwPathcAddr, 5);
	//��Patch��
	memcpy((LPVOID)dwPathcAddr, fix, 5);
	__asm
	{
		lea ebx, imagefilepath;
		push ebx;
		lea eax, imagewxid;
		push eax;
		lea eax, buff;
		push eax;
		push 0x0;
		call dwCall1;
		add esp, 0x4;
		mov ecx, eax;
		call dwCall2;
	}
	//������֮��ָ�
	memcpy((LPVOID)dwPathcAddr, oldcode, 5);
	//�ָ�����
	VirtualProtect((LPVOID)dwPathcAddr, 5, dwOldAttr, &dwOldAttr);
}
/*==================================����ͼƬ����=======================================*/

/*==================================����xml���¿�ʼ=======================================*/
void SendXmlMessage(DWORD type, wchar_t* recverWxid, wchar_t* fromWxid, wchar_t* filePath, wchar_t* xmlData)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	SendXmlWxidStruct wxid = { 0 };
	wxid.wxid = recverWxid;
	wxid.wxidLen = wcslen(recverWxid);
	wxid.wxidMaxLen = wcslen(recverWxid) * 2;

	SendXmlFromWxidStruct fWxid = { 0 };
	fWxid.wxid = fromWxid;
	fWxid.wxidLen = wcslen(fromWxid);
	fWxid.wxidMaxLen = wcslen(fromWxid) * 2;

	SendXmlImageStruct file = { 0 };
	file.filePath = filePath;
	file.pathLen = wcslen(filePath);
	file.pathMaxLen = wcslen(filePath) * 2;

	SendXmlStruct xml = { 0 };
	xml.content = xmlData;
	xml.strLen = wcslen(xmlData);
	xml.strMaxLen = wcslen(xmlData) * 2;

	char buff1[0x88] = { 0 };
	char buff2[0x2D8] = { 0 };
	char buff3[0xF4] = { 0 };
	char buff4[0x60] = { 0 };

	DWORD call1 = winAddress + WX_SEND_XML_CALL1;
	DWORD call2 = winAddress + WX_SEND_XML_CALL2;
	DWORD call3 = winAddress + WX_SEND_XML_CALL3;
	DWORD call4 = winAddress + WX_SEND_XML_CALL4;
	DWORD call5 = winAddress + WX_SEND_XML_CALL5;
	DWORD call6 = winAddress + WX_SEND_XML_CALL6;
	DWORD param = winAddress + WX_SEND_XML_PARAM;
	DWORD call7 = winAddress + WX_SEND_XML_CALL7;
	int typeInt = type;
	__asm {
		pushad
		lea ebx, wxid
		push typeInt  // 0x5�����ӷ���0x21��С�������
		lea eax, buff1
		push eax
		lea eax, file
		push eax
		lea eax, xml
		push eax
		push ebx
		lea edx, fWxid
		lea ecx, buff2
		call call1
		add esp, 0x14
		lea eax, buff3
		xor bl, bl
		push eax
		lea ecx, buff2
		call call2
		mov edx, eax
		lea ecx, buff4
		call call3
		lea ecx, buff3
		call call4
		lea ecx, buff4
		call call5
		mov ecx, 0x1
		cmovne ebx, ecx
		lea ecx, buff1
		call call6
		lea ecx, file
		call call6
		push param
		push param
		mov dl, bl
		lea ecx, buff2
		call call7
		add esp, 0x8
		popad
	}
}
/*==================================����xml���½���=======================================*/

/*==================================���͸�����ʼ=======================================*/
void SendFileMessage(wchar_t* wxid, wchar_t* filepath)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();
	
	DWORD dwCall1 = winAddress + WX_SEND_ATTACH_CALL1;
	DWORD dwCall2 = winAddress + WX_SEND_ATTACH_CALL2;
	DWORD dwCall3 = winAddress + WX_SEND_ATTACH_CALL3;
	DWORD dwCall4 = winAddress + WX_SEND_ATTACH_CALL4;
	DWORD dwParams = winAddress + WX_SEND_ATTACH_PARAM;

	char buff[0x45C] = { 0 };
	SendAttachWxidStruct wxidStruct = { 0 };
	wxidStruct.str = wxid;
	wxidStruct.strLen = wcslen(wxid);
	wxidStruct.maxLen = wcslen(wxid) * 2;

	SendAttachFileStruct filePathStruct = { 0 };
	filePathStruct.str = filepath;
	filePathStruct.strLen = wcslen(filepath);
	filePathStruct.maxLen = wcslen(filepath) * 2;

	char* pFilePath = (char*)&filePathStruct.str;
	char* pWxid = (char*)&wxidStruct.str;

	__asm {
		pushad
		push dword ptr ss : [ebp - 0x5C];
		sub esp, 0x14;
		mov ecx, esp;
		push - 0x1;
		push dwParams;
		call dwCall1;
		sub esp, 0x14;
		mov ecx, esp;
		push pFilePath;
		call dwCall2;
		sub esp, 0x14;
		mov ecx, esp;
		push pWxid;
		call dwCall2;
		lea eax, buff;
		push eax;
		call dwCall3;
		mov ecx, eax;
		call dwCall4;
		popad
	}
}
/*==================================���͸�������=======================================*/


