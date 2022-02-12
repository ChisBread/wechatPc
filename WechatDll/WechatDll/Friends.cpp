#include "stdafx.h"
#include "resource.h"
#include "Config.h"
#include "WechatOffset.h"
#include "Common.h"
#include "Base64.h"
#include "Struct.h"
#include "Package.h"
#include <stdio.h>
#include <windows.h>
#include <string>
#include <tchar.h> 
#include <sstream>
#include <iomanip>
#include <strstream>
#include <list>
#include <iostream>
#include <map>
#include <cmath>
#include "Sql.h"
#include "Send.h"
#include "Friends.h"

using namespace std;

typedef tuple <
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string,
	string
> USER_INFO;

// ����7000���û��б�
list<USER_INFO> userInfoList(100000);
// ��ȡ�����б������ǵ�call��ַ
DWORD friendsOverWritedCallAdd = 0;
// ��ȡ�����б����صĵ�ַ
DWORD friendsJumBackAddress = 0;
// ��ȡ�����б�����Ҫ��ȡ�ļĴ�������
DWORD friendsEsi = 0;
// ��ȡ�����б�Hook��ַ
DWORD friendsHookAddress = 0;
// �Ƿ���º����б� - ���µ�����
int updateUserListNumber = 0;  // ���µ�����
int updateUserListComparedNumber = 0;  // �Աȵ�����
// ��ҳ��ȡ�����б� - ÿҳ����
int friendPageSize = 100;

/*==================================��ȡ�����б�ʼ=======================================*/
INT GetContactCallBack(void* para, int nColumn, char** colValue, char** colName)
{
	char data[0x3A98] = { 0 };

	string Wxid = { 0 };
	string EncryptUsername = { 0 };
	string Alias = { 0 };
	string Type = { 0 };
	string Sex = { 0 };
	string Nickname = { 0 };
	string RoomType = { 0 };
	string Remark = { 0 };
	string BigHeadImgUrl = { 0 };
	string SmallHeadImgUrl = { 0 };
	string Ticket = { 0 };
	string RoomWxidList = { 0 };
	string RoomOwner = { 0 };
	string RoomAnnouncement = { 0 };
	string RoomAnnouncementEditor = { 0 };
	string RoomAnnouncementPublishTime = { 0 };
	string RoomStatus = { 0 };
	string BizType = { 0 };
	string BizAcceptType = { 0 };
	string BizBrandList = { 0 };
	string BizBrandFlag = { 0 };
	string BizBrandIconURL = { 0 };
	string BizExtInfo = { 0 };
	string CBigHeadImgUrl = { 0 };
	string CSmallHeadImgUrl = { 0 };
	string DelFlag = { 0 };
	string VerifyFlag = { 0 };
	string RoomNotify = { 0 };
	string ExtraBuf = { 0 };

	for (int i = 0; i < nColumn; i++) {
		memset(data, 0x0, sizeof(data));
		sprintf_s(data, sizeof(data), "%s", colValue[i]);
		if (strcmp(*(colName + i), "Wxid") == 0 && strlen(data) > 0) Wxid = data;
		else if (strcmp(*(colName + i), "EncryptUsername") == 0 && strlen(data) > 0) EncryptUsername = data;
		else if (strcmp(*(colName + i), "Alias") == 0 && strlen(data) > 0) Alias = data;
		else if (strcmp(*(colName + i), "Type") == 0 && strlen(data) > 0) Type = data;
		else if (strcmp(*(colName + i), "Sex") == 0 && strlen(data) > 0)  Sex = data;
		else if (strcmp(*(colName + i), "Nickname") == 0 && strlen(data) > 0) Nickname = data;
		else if (strcmp(*(colName + i), "RoomType") == 0 && strlen(data) > 0) RoomType = data;
		else if (strcmp(*(colName + i), "Remark") == 0 && strlen(data) > 0) Remark = data;
		else if (strcmp(*(colName + i), "BigHeadImgUrl") == 0 && strlen(data) > 0) BigHeadImgUrl = data;
		else if (strcmp(*(colName + i), "SmallHeadImgUrl") == 0 && strlen(data) > 0) SmallHeadImgUrl = data;
		else if (strcmp(*(colName + i), "Ticket") == 0 && strlen(data) > 0) Ticket = data;
		else if (strcmp(*(colName + i), "RoomWxidList") == 0 && strlen(data) > 0) RoomWxidList = data;
		else if (strcmp(*(colName + i), "RoomOwner") == 0 && strlen(data) > 0) RoomOwner = data;
		else if (strcmp(*(colName + i), "RoomAnnouncement") == 0 && strlen(data) > 0) RoomAnnouncement = data;
		else if (strcmp(*(colName + i), "RoomAnnouncementEditor") == 0 && strlen(data) > 0) RoomAnnouncementEditor = data;
		else if (strcmp(*(colName + i), "RoomAnnouncementPublishTime") == 0 && strlen(data) > 0) RoomAnnouncementPublishTime = data;
		else if (strcmp(*(colName + i), "RoomStatus") == 0 && strlen(data) > 0) RoomStatus = data;
		else if (strcmp(*(colName + i), "BizType") == 0 && strlen(data) > 0) BizType = data;
		else if (strcmp(*(colName + i), "BizAcceptType") == 0 && strlen(data) > 0) BizAcceptType = data;
		else if (strcmp(*(colName + i), "BizBrandList") == 0 && strlen(data) > 0) BizBrandList = data;
		else if (strcmp(*(colName + i), "BizBrandFlag") == 0 && strlen(data) > 0) BizBrandFlag = data;
		else if (strcmp(*(colName + i), "BizBrandIconURL") == 0 && strlen(data) > 0) BizBrandIconURL = data;
		else if (strcmp(*(colName + i), "BizExtInfo") == 0 && strlen(data) > 0) BizExtInfo = data;
		else if (strcmp(*(colName + i), "CBigHeadImgUrl") == 0 && strlen(data) > 0) CBigHeadImgUrl = data;
		else if (strcmp(*(colName + i), "CSmallHeadImgUrl") == 0 && strlen(data) > 0) CSmallHeadImgUrl = data;
		else if (strcmp(*(colName + i), "DelFlag") == 0 && strlen(data) > 0) DelFlag = data;
		else if (strcmp(*(colName + i), "VerifyFlag") == 0 && strlen(data) > 0) VerifyFlag = data;
		else if (strcmp(*(colName + i), "CRoomNotify") == 0 && strlen(data) > 0) RoomNotify = data;
		else if (strcmp(*(colName + i), "ExtraBuf") == 0 && strlen(data) > 0) ExtraBuf = data;

		/*
		// ExtraBuf
		const void * pReadBolbData = sqlite3_column_blob(colValue[i], 1);
		len = sqlite3_column_bytes(stmt, 1);

		memcpy(&tempStudent, pReadBolbData, len);
		cout << "����=" << tempStudent.name << endl;
		cout << "����=" << tempStudent.age << endl;
		*/
	}
	
	if (Wxid.length() <= 0) {
		return 0;
	}

	// ����Ѿ����ڣ��򲻼ӽ�ȥ
	for (auto& userInfoOld : userInfoList) {
		string WxidOld = get<0>(userInfoOld);
		if (Wxid == WxidOld) {
			return 0;
		}
	}
	// ����ͷ��
	if (BigHeadImgUrl.length() <= 0 || strcmp(BigHeadImgUrl.c_str(), "(null)") == 0) {
		BigHeadImgUrl = CBigHeadImgUrl;
	}
	if (SmallHeadImgUrl.length() <= 0 || strcmp(SmallHeadImgUrl.c_str(), "(null)") == 0) {
		SmallHeadImgUrl = CSmallHeadImgUrl;
	}
	
	USER_INFO userInfo(Wxid, EncryptUsername, Alias, Type, Sex, Nickname,
		RoomType, Remark, BigHeadImgUrl, SmallHeadImgUrl, Ticket, RoomWxidList,
		RoomOwner, RoomAnnouncement, RoomAnnouncementEditor, RoomAnnouncementPublishTime, RoomStatus,
		BizType, BizAcceptType, BizBrandList, BizBrandFlag, BizBrandIconURL, BizExtInfo, DelFlag,
		VerifyFlag, RoomNotify, ExtraBuf);

	userInfoList.push_front(userInfo);

	return 0;
}
// ��ѯ�б������ķ���
INT GetContactCountCallBack(void* para, int nColumn, char** colValue, char** colName) 
{
	int friendsTotal = 0;
	for (int i = 0; i < nColumn; i++) {
		if (strcmp(*(colName + i), "count") == 0) {
			friendsTotal = std::atoi(colValue[i]);
		}
	}
	
	// ������ҳ����Ȼ��һҳҳ��ѯ���ҷ���
	if (friendsTotal > 0) {
		Package *package = new Package();
		// ����Ψһ��
		string uniqueId = CreateUniqueId();
		// ����ҳ����ѭ��ȡÿһҳ
		int pageTotal = (int)ceil((double)friendsTotal / (double)friendPageSize);
		for (int page = 1; page <= pageTotal; page++) {
			package->SetUniqueId((char*)uniqueId.c_str());
			// ����б�
			userInfoList.clear();
			// �����ҳ����
			int offset = (page - 1) * friendPageSize;
			// ��װ��ѯ��SQL
			char sql[0x555] = {0};
			sprintf_s(sql, "select Contact.UserName as Wxid,Contact.ExtraBuf,Contact.EncryptUserName as EncryptUsername,Contact.Alias,Contact.Type,Contact.DelFlag,Contact.VerifyFlag,Contact.BigHeadImgUrl as CBigHeadImgUrl,Contact.SmallHeadImgUrl AS CSmallHeadImgUrl,Contact.ChatRoomNotify as CRoomNotify,Contact.NickName as Nickname,Contact.ChatRoomType as RoomType,Contact.Remark,BizInfo.Type as BizType,BizInfo.AcceptType as BizAcceptType,BizInfo.BrandList as BizBrandList,BizInfo.BrandFlag as BizBrandFlag,BizInfo.BrandIconURL as BizBrandIconURL,BizInfo.ExtInfo as BizExtInfo,ContactHeadImgUrl.bigHeadImgUrl as BigHeadImgUrl,ContactHeadImgUrl.smallHeadImgUrl as SmallHeadImgUrl,TicketInfo.Ticket,ChatRoom.UserNameList as RoomWxidList,ChatRoom.Owner as RoomOwner,ChatRoomInfo.Announcement as RoomAnnouncement,ChatRoomInfo.AnnouncementEditor as RoomAnnouncementEditor,ChatRoomInfo.AnnouncementPublishTime as RoomAnnouncementPublishTime,ChatRoomInfo.ChatRoomStatus as RoomStatus from Contact LEFT OUTER JOIN BizInfo ON Contact.UserName = BizInfo.UserName LEFT OUTER JOIN ContactHeadImgUrl ON Contact.UserName = ContactHeadImgUrl.usrName LEFT OUTER JOIN TicketInfo ON Contact.UserName = TicketInfo.UserName LEFT OUTER JOIN ChatRoom ON Contact.UserName = ChatRoom.ChatRoomName LEFT OUTER JOIN ChatRoomInfo ON ChatRoom.ChatRoomName = ChatRoomInfo.ChatRoomName limit %d offset %d", friendPageSize, offset);
			// ִ�в�ѯSQL
			string sqlString = sql;
			runSql("MicroMsg.db", sqlString, GetContactCallBack); //ʵ������д����userInfoList
			// ����������ͺ����б�
			
			SendList(package, page, friendPageSize, friendsTotal);
			
		}
	}

	return 0;
}
// ��ʼ��ѯ�б�
void selectFriendList()
{
	runSql("MicroMsg.db", "select count(*) as count from Contact limit 1", GetContactCountCallBack);
}
DWORD WINAPI selectFriendListMonitor()
{
	// �ȱ������е��������������Աȣ�һ����������û�䶯�������б�
	updateUserListComparedNumber = updateUserListNumber;
	
	while (TRUE) {
		Sleep(1500);  // �ȵȴ�1.5��
		if (updateUserListComparedNumber != updateUserListNumber) {
			updateUserListComparedNumber = updateUserListNumber;
			// �����б䶯��˵�����ڸ��£�������һ�ε�һ��
			continue;
		}
		// �ٵ�2���ٷ�������
		Sleep(2000);
		// ִ�в�ѯ���ݿ�Ȼ�����б�
		selectFriendList();
		// ���ü�����
		updateUserListComparedNumber = 0;
		updateUserListNumber = 0;
		break;
	}
	
	return FALSE;
}
// ���º����б����ҷ��ͻط�����
void UpdateUserList() 
{
	// �Ѿ��ڵȴ�������
	if (updateUserListNumber > 0) {
		updateUserListNumber += 1;
		return;
	}
	// ���ÿ�ʼ�ȴ�����
	updateUserListNumber = 1;
	// ��һ���̵߳ȴ����ҷ���
	HANDLE cThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)selectFriendListMonitor, NULL, NULL, 0);
	if (cThread != 0) {
		CloseHandle(cThread);
	}
}
//��ת������������Լ�������Ϣ
__declspec(naked) VOID RecieveUserInfoHook()
{
	//�����ֳ�
	__asm
	{
		//��ȡeax�Ĵ������ݣ�����һ��������
		//mov friendsEsi, esi

		//����Ĵ���
		pushad
		pushf
	}

	// ��ѯ���ݿⲢ�ҷ��ͺ����б��ȥ
	UpdateUserList();

	//�ָ��ֳ�
	__asm
	{
		popf
		popad

		//���䱻���ǵĴ���
		call friendsOverWritedCallAdd

		//���ر�HOOKָ�����һ��ָ��
		jmp friendsJumBackAddress
	}
}

//�����ͺ����б�����
void SendList(Package *package, int page, int pageSize, int total)
{
	FriendList *list, *normal, *next;

	char buff[0x3A98] = { 0 };

	next = new FriendList;

	/*�����ڴ�*/
	list = next;

	//�к�++
	DWORD index = 0;
	
	for (auto& userInfoOld : userInfoList) {
		
			string Wxid = get<0>(userInfoOld);
			string EncryptUsername = get<1>(userInfoOld);
			string Alias = get<2>(userInfoOld);
			string Type = get<3>(userInfoOld);
			string Sex = get<4>(userInfoOld);
			string Nickname = get<5>(userInfoOld);
			string RoomType = get<6>(userInfoOld);
			string Remark = get<7>(userInfoOld);
			string BigHeadImgUrl = get<8>(userInfoOld);
			string SmallHeadImgUrl = get<9>(userInfoOld);
			string Ticket = get<10>(userInfoOld);
			string RoomWxidList = get<11>(userInfoOld);
			string RoomOwner = get<12>(userInfoOld);
			string RoomAnnouncement = get<13>(userInfoOld);
			string RoomAnnouncementEditor = get<14>(userInfoOld);
			string RoomAnnouncementPublishTime = get<15>(userInfoOld);
			string RoomStatus = get<16>(userInfoOld);
			string BizType = get<17>(userInfoOld);
			string BizAcceptType = get<18>(userInfoOld);
			string BizBrandList = get<19>(userInfoOld);
			string BizBrandFlag = get<20>(userInfoOld);
			string BizBrandIconURL = get<21>(userInfoOld);
			string BizExtInfo = get<22>(userInfoOld);
			string DelFlag = get<23>(userInfoOld);
			string VerifyFlag = get<24>(userInfoOld);
			string RoomNotify = get<25>(userInfoOld);
			string ExtraBuf = get<26>(userInfoOld);

			normal = new FriendList;

			sprintf_s(normal->wxid, "%s", Wxid.c_str());
			sprintf_s(normal->username, "%s", Alias.c_str());
			sprintf_s(normal->v1, "%s", EncryptUsername.c_str());
			sprintf_s(normal->v2, "%s", Ticket.c_str());
			normal->type = atoi(Type.c_str());
			normal->sex = atoi(Sex.c_str());
			sprintf_s(normal->nickname, "%s", Nickname.c_str());
			normal->roomType = atoi(RoomType.c_str());
			sprintf_s(normal->remark, "%s", Remark.c_str());
			sprintf_s(normal->bigHeadImgUrl, "%s", BigHeadImgUrl.c_str());
			sprintf_s(normal->smallHeadImgUrl, "%s", SmallHeadImgUrl.c_str());
			sprintf_s(normal->roomWxidList, "%s", RoomWxidList.c_str());
			normal->roomOwner = atoi(RoomOwner.c_str());
			sprintf_s(normal->roomAnnouncement, "%s", RoomAnnouncement.c_str());
			sprintf_s(normal->roomAnnouncementEditor, "%s", RoomAnnouncementEditor.c_str());
			sprintf_s(normal->roomAnnouncementPublishTime, "%s", RoomAnnouncementPublishTime.c_str());
			normal->roomStatus = atoi(RoomStatus.c_str());
			normal->bizType = atoi(BizType.c_str());
			normal->bizAcceptType = atoi(BizAcceptType.c_str());
			sprintf_s(normal->bizBrandList, "%s", BizBrandList.c_str());
			normal->bizBrandFlag = atoi(BizBrandFlag.c_str());
			sprintf_s(normal->bizBrandIconURL, "%s", BizBrandIconURL.c_str());
			sprintf_s(normal->bizExtInfo, "%s", BizExtInfo.c_str());
			normal->delFlag = atoi(DelFlag.c_str());
			normal->verifyFlag = atoi(VerifyFlag.c_str());
			normal->roomNotify = atoi(RoomNotify.c_str());

			// ����key
			memset(buff, 0x0, sizeof(buff));
			base64_encode((const unsigned char*)ExtraBuf.c_str(), ExtraBuf.length(), buff);
			sprintf_s(normal->extraBuf, "%s", buff);  // �����Ƶļ��ܴ�

			// �Ӻ����������油�䲿������
			userInfo info = GetUserInfoByWxid(UTF8ToUnicode((char*)Wxid.c_str()));
			if (strlen(normal->username) <= 0 || strcmp(normal->username, "(null)") == 0) {
				sprintf_s(normal->username, "%s", UnicodeToUtf8(info.username));
			}
			if (strlen(normal->v1) <= 0 || strcmp(normal->v1, "(null)") == 0) {
				sprintf_s(normal->v1, "%s", UnicodeToUtf8(info.v1));
			}
			if (strlen(normal->nickname) <= 0 || strcmp(normal->nickname, "(null)") == 0) {
				sprintf_s(normal->nickname, "%s", UnicodeToUtf8(info.nickname));
			}

			next->next = normal;
			next = next->next;

			index++;
	}
	next->next = NULL;/*��������ָ��һ���µ�ַ*/
	// ����
	if (index > 0) {
		Send::SendFriendList(list, package, page, pageSize, total);
		//char out_msg[0x555] = { 0 };

	}
	
	FriendList* p = list;
	do {
		FriendList* tmp = p;
		p = p->next;
		delete tmp;
	} while (p);
}

//Hook�����б���Ϣ
void ListFriendsList()
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	// ���ǵ���
	friendsHookAddress = winAddress + LISTEN_FRIENDS_HOOK;

	//�����ǵ�CALLĿ��
	friendsOverWritedCallAdd = winAddress + LISTEN_FRIENDS_CALL;

	//���صĵ�ַ
	friendsJumBackAddress = friendsHookAddress + 5;

	//��װ��ת����
	BYTE jmpCodes[5] = { 0 };
	jmpCodes[0] = 0xE9;

	//����תָ���е�����=��ת�ĵ�ַ-ԭ��ַ��HOOK�ĵ�ַ��-��תָ��ĳ���
	*(DWORD*)& jmpCodes[1] = (DWORD)RecieveUserInfoHook - friendsHookAddress - 5;

	//����ָ��
	//5A9D1894    E8 0790C1FF     call WeChatWi.5A5EA8A0
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)friendsHookAddress, jmpCodes, 5, 0);
}
/*==================================��ȡ�����б����=======================================*/

/*==================================ͬ���������ʼ=======================================*/
/*
ͬ��Ӻ�������
*/
void VerifyFriend(PWCHAR v1, PWCHAR v2)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	DWORD call1 = winAddress + WX_VERIFY_USER_CALL1;
	DWORD call2 = winAddress + WX_VERIFY_USER_CALL2;
	DWORD call3 = winAddress + WX_VERIFY_USER_CALL3;
	DWORD call4 = winAddress + WX_VERIFY_USER_CALL4;
	DWORD param = winAddress + WX_VERIFY_USER_PARAM;
	DWORD *asmP = (DWORD*)param;
	v1Info userInfoV1 = {0};
	v2Info userInfoV2 = {0};

	userInfoV1.v2 = (DWORD)&userInfoV2.fill;
	userInfoV1.v1 = v1;
	userInfoV1.v1Len = wcslen(v1);
	userInfoV1.maxV1Len = wcslen(v1) * 2;
	userInfoV2.v2 = v2;
	userInfoV2.v2Len = wcslen(v2);
	userInfoV2.maxV2Len = wcslen(v2) * 2;

	char *asmUser = (char*)&userInfoV1.fill;
	char buff[0x14] = {0};
	char buff2[0x48] = {0};
	char *asmBuff = buff2;

	__asm {
		pushad
		mov ecx, asmUser
		push 0x6
		sub esp, 0x14
		push esp
		call call1
		mov ecx, asmUser
		lea eax, buff
		push eax
		call call2
		mov esi, eax
		sub esp, 0x8
		mov ecx, asmP
		call call3
		mov ecx, asmBuff
		mov edx, ecx
		push eax
		push edx
		push esi
		call call4
		popad
	}
}
/*==================================ͬ������������=======================================*/

/*==================================�Ӻ��ѿ�ʼ=======================================*/
void AddFriend(PWCHAR wxid, PWCHAR message)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	DWORD param = winAddress + WX_ADD_USER_PARAM;
	DWORD call1 = winAddress + WX_ADD_USER_CALL1;
	DWORD call2 = winAddress + WX_ADD_USER_CALL2;
	DWORD call3 = winAddress + WX_ADD_USER_CALL3;
	DWORD call4 = winAddress + WX_ADD_USER_CALL4;
	DWORD call5 = winAddress + WX_ADD_USER_CALL5;
	DWORD call6 = winAddress + WX_ADD_USER_CALL6;

	wechatText pWxid = {0};
	pWxid.pStr = wxid;
	pWxid.strLen = wcslen(wxid);
	pWxid.strMaxLen = wcslen(wxid) * 2;

	wechatText msg = { 0 };
	msg.pStr = message;
	msg.strLen = wcslen(message);
	msg.strMaxLen = wcslen(message) * 2;

	char *asmWxid = (char *)&pWxid.pStr;
	char *asmMsg = (char *)&msg.pStr;
	DWORD asmMsgText = (DWORD)msg.pStr;
	char buff3[0x100] = {0};
	char *buff = buff3;

	__asm {
		pushad
		sub esp, 0x18
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xDC], esp
		push param
		call call1
		sub esp, 0x18
		mov eax, buff
		mov dword ptr ss : [ebp - 0xE4], esp
		mov ecx, esp
		push eax
		call call2
		push 0x6  // Ⱥ�ڼӺ�����E��Ⱥ����6
		sub esp, 0x14
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xE8], esp
		push - 0x1
		mov edi, asmMsgText
		push edi
		call call3
		mov eax, asmMsg
		push 0x2
		sub esp, 0x14
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xE0], esp
		mov ebx, asmWxid
		push ebx
		call call4
		call call5
		mov ecx, eax
		call call6
		popad
	}
}
/*==================================�Ӻ��ѽ���=======================================*/

/*==================================��ȡ�������鿪ʼ=======================================*/
userInfo GetUserInfoByWxid(wchar_t* userwxid)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	DWORD call1 = winAddress + WX_GET_USER_INFO_CALL1;
	DWORD call2 = winAddress + WX_GET_USER_INFO_CALL2;
	DWORD call3 = winAddress + WX_GET_USER_INFO_CALL3;
	//DWORD call4 = winAddress + 0x4F510;

	char buff[0xBF8] = { 0 };  // 0x3D8
	char *asmBuff = buff;
	wechatText pWxid = { 0 };
	pWxid.pStr = userwxid;
	pWxid.strLen = wcslen(userwxid);
	pWxid.strMaxLen = wcslen(userwxid) * 2;

	char *asmWxid = (char *)&pWxid.pStr;

	DWORD userData = 0;		//�û����ݵĵ�ַ

	__asm {
		pushad
		mov edi, asmBuff
		push edi
		sub esp, 0x14
		mov eax, asmWxid
		mov ecx, esp
		push eax
		call call1
		call call2
		call call3
		//mov esi, edi
		//call call4
		mov userData, edi
		popad
	}

	//��װ�ṹ��
	userInfo user = { 0 };

	/*
	8 ΢��ID
	1C ΢�ź�
	30 v1
	48 ���������ͣ�1=���ѣ�3=Ⱥ��
	50 ��ע
	64 �ǳ�
	108 ͷ��
	15C �Ա�1=�У�2=Ů������=δ֪
	194 ����ǩ��
	1A8 ����
	1BC ʡ��
	1D0 ����
	274 ����Ȧͷ��
	*/
	LPVOID textWxid = *((LPVOID*)(userData + 0x8));
	LPVOID textUsername = *((LPVOID*)(userData + 0x1C));
	LPVOID textNickname = *((LPVOID*)(userData + 0x64));
	LPVOID textRemark = *((LPVOID*)(userData + 0x50));
	LPVOID textAvatar = *((LPVOID*)(userData + 0x108));
	LPVOID textV1 = *((LPVOID*)(userData + 0x30));

	swprintf_s(user.wxid, L"%s", (wchar_t*)textWxid);
	swprintf_s(user.username, L"%s", (wchar_t*)textUsername);
	swprintf_s(user.nickname, L"%s", (wchar_t*)textNickname);
	swprintf_s(user.remark, L"%s", (wchar_t*)textRemark);
	swprintf_s(user.avatar, L"%s", (wchar_t*)textAvatar);
	swprintf_s(user.v1, L"%s", (wchar_t*)textV1);

	return user;
}
/*==================================��ȡ�����������=======================================*/

/*==================================ɾ�����ѿ�ʼ=======================================*/
void DeleteUser(wchar_t* wxid)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();
	//��������
	DeleteUserStructWxid structWxid = { 0 };
	structWxid.pWxid = wxid;
	structWxid.length = wcslen(wxid);
	structWxid.maxLength = wcslen(wxid) * 2;

	DWORD* asmMsg = (DWORD*)&structWxid.pWxid;
	DWORD dwCallAddr = winAddress + WX_DELETE_USER;

	__asm {
		pushad
		mov ecx, 0;
		push ecx;
		mov edi, asmMsg;
		push edi;
		call  dwCallAddr;
		popad
	}
}
/*==================================ɾ�����ѽ���=======================================*/

/*==================================ͨ���Ƽ���Ƭ��Ӻ��ѿ�ʼ=======================================*/
void AddUserFromCard(wchar_t* v1, wchar_t* msg)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	DWORD addParam = winAddress + WX_ADD_CARD_USER_PARAM;
	DWORD addCall1 = winAddress + WX_ADD_CARD_USER_CALL1;
	DWORD addCall2 = winAddress + WX_ADD_CARD_USER_CALL2;
	DWORD addCall3 = winAddress + WX_ADD_CARD_USER_CALL3;
	DWORD addCall4 = winAddress + WX_ADD_CARD_USER_CALL4;
	DWORD addCall5 = winAddress + WX_ADD_CARD_USER_CALL5;


	AddUserFromCardStruct pV1 = { 0 };
	pV1.pStr = v1;
	pV1.strLen = wcslen(v1) + 1;
	pV1.strMaxLen = (wcslen(v1) + 1) * 2;


	char* asmV1 = (char*)&pV1.pStr;
	char buff3[0x100] = { 0 };
	char* buff = buff3;
	__asm
	{
		pushad
		sub esp, 0x18;
		mov ecx, esp;
		mov dword ptr ss : [ebp - 0xDC], esp;
		push  addParam;
		call addCall1;
		sub esp, 0x18;
		mov eax, buff;
		mov dword ptr ss : [ebp - 0xE4], esp;
		mov ecx, esp;
		push eax;
		call addCall2;
		push 0x11;
		sub esp, 0x14;
		mov ecx, esp;
		mov dword ptr ss : [ebp - 0xE8], esp;
		push - 0x1;
		mov edi, msg;
		push edi;
		call addCall3;
		push 0x2;
		sub esp, 0x14;
		mov ecx, esp;
		mov dword ptr ss : [ebp - 0xE0], esp;
		mov ebx, asmV1;
		push ebx;
		call addCall4;
		mov ecx, eax;
		call addCall5;
		popad
	}
}
/*==================================ͨ���Ƽ���Ƭ��Ӻ��ѽ���=======================================*/

/*==================================���ú��ѱ�ע��ʼ=======================================*/
void SetRemark(wchar_t *wxid, wchar_t *remark)
{
	// ��ȡ΢�Ż�ַ
	DWORD winAddress = GetWechatWinAddress();

	// ���ñ�ע��ƫ��
	DWORD dwCallAddr = winAddress + WX_SET_USER_REMARK_CALL;
	DWORD dwParamAddr = winAddress + WX_SET_USER_REMARK_PARAM;

	// ���챸ע�ṹ
	SetRemarkStruct remarkStruct = { 0 };
	remarkStruct.remark = remark;
	remarkStruct.remarkLen = wcslen(remark);
	remarkStruct.maxRemarkLen = wcslen(remark) * 2;

	// ��ʼ��ȡ������������
	DWORD userData = 0;
	DWORD call1 = winAddress + WX_GET_USER_INFO_CALL1;
	DWORD call2 = winAddress + WX_GET_USER_INFO_CALL2;
	DWORD call3 = winAddress + WX_GET_USER_INFO_CALL3;
	char buff[0xBF8] = { 0 };  // 0x3D8
	char *asmBuff = buff;
	wechatText pWxid = { 0 };
	pWxid.pStr = wxid;
	pWxid.strLen = wcslen(wxid);
	pWxid.strMaxLen = wcslen(wxid) * 2;
	char *asmWxid = (char *)&pWxid.pStr;
	__asm {
		pushad
		mov edi, asmBuff
		push edi
		sub esp, 0x14
		mov eax, asmWxid
		mov ecx, esp
		push eax
		call call1
		call call2
		call call3
		mov userData, edi
		popad
	}

	// ��ֵ��ע
	memcpy((LPVOID*)(userData + 0x50), &remarkStruct, sizeof(remarkStruct));
	DWORD buf = 0x800;
	memcpy((LPVOID*)userData, &buf, sizeof(buf));
	DWORD buf2 = 0x1;  // 0x1�޸ı�ע��0x3ɾ������
	memcpy((LPVOID*)(userData + 0x48), &buf2, sizeof(buf2));
	DWORD buf3 = 0x3;  // 0x1ɾ�����ѣ�0x3�޸ı�ע
	memcpy((LPVOID*)(userData + 0x158), &buf3, sizeof(buf3));
	DWORD buf4 = 0x3;  // 0x1ɾ�����ѣ�0x3�޸ı�ע
	memcpy((LPVOID*)(userData + 0x1E4), &buf4, sizeof(buf4));
	DWORD buf5 = 0x1;  // 0x1ɾ�����ѣ�0x3�޸ı�ע
	memcpy((LPVOID*)(userData + 0x228), &buf5, sizeof(buf5));
	DWORD buf6 = 0x1;  // 0x1ɾ�����ѣ�0x3�޸ı�ע
	memcpy((LPVOID*)(userData + 0x230), &buf6, sizeof(buf6));

	// ��ʼ���ñ�ע
	DWORD *asmP = (DWORD*)dwParamAddr;
	__asm {
		pushad
		push 0x0
		mov eax, userData
		push eax
		call dwCallAddr
		popad
	}
}
/*==================================���ú��ѱ�ע����=======================================*/
