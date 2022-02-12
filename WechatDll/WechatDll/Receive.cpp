#include "stdafx.h"
#include "Common.h"
#include "OpCode.h"
#include "Package.h"
#include "WebSocket.h"
#include "Login.h"
#include "LoginInfo.h"
#include "Message.h"
#include "Friends.h"
#include "Transfer.h"
#include "Room.h"
#include "Send.h"
#include "Receive.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atlstr.h>

// �������˳���Ϣ
bool Receive::SendLogout(Package *package)
{
	// ��ʼ�����ݰ�
	Package tmp_package;
	if (!package) {
		package = &tmp_package;
	}
	// ���ò�������
	package->SetOpCode(OpCode::OPCODE_WECHAT_QUIT);
	// ����body
	rapidjson::Value newBody(rapidjson::kObjectType); // ����һ��Object���͵�Ԫ��
	// ����ID
	rapidjson::Value processId(rapidjson::kNumberType);
	processId.SetInt(GetCurrentProcessId());
	newBody.AddMember("processId", processId, package->json.GetAllocator());
	// ��¼״̬
	rapidjson::Value loginStatus(rapidjson::kNumberType);
	loginStatus.SetInt(CheckLoginStatus());
	newBody.AddMember("loginStatus", loginStatus, package->json.GetAllocator());
	package->SetBody(newBody);
	rapidjson::StringBuffer pack = package->GetConText();
	WsClientSend((char*)pack.GetString());
	return TRUE;
}

// ���͵�¼��Ϣ
bool Receive::GetLoginInfo(Package *package)
{
	// ��������
	SendWechatUser(package);

	return TRUE;
}

// ��ȡ��¼״̬
bool Receive::GetLoginStatus(Package *package)
{
	// ����body
	rapidjson::Value newBody(rapidjson::kObjectType); // ����һ��Object���͵�Ԫ��
	// ��¼״̬
	rapidjson::Value loginStatus(rapidjson::kNumberType);
	loginStatus.SetInt(CheckLoginStatus());
	newBody.AddMember("loginStatus", loginStatus, package->json.GetAllocator());
	package->SetBody(newBody);
	rapidjson::StringBuffer pack = package->GetConText();
	WsClientSend((char*)pack.GetString());
	return TRUE;
}
// �˳���¼
bool Receive::Logout(Package *package)
{
	// �˳���¼����
	DoLogout();
	// �������˳���Ϣ
	Receive::SendLogout(package);
	
	return TRUE;
}
// �����ı���Ϣ
bool Receive::MessageSendText(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("content")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& content = body["content"];

	// ����
	SendTextMessage(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)content.GetString()));
	
	return TRUE;
}
// ����ͼƬ��Ϣ
bool Receive::MessageSendImage(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("imageUrl")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& imageUrl = body["imageUrl"];

	// ����
	SendImageMessage(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)imageUrl.GetString()));

	return TRUE;
}
// ���͸�����Ϣ
bool Receive::MessageSendFile(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("fileUrl")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& fileUrl = body["fileUrl"];

	// ����
	SendFileMessage(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)fileUrl.GetString()));

	return TRUE;
}
// ������Ƭ��Ϣ
bool Receive::MessageSendCard(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("xml")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& xml = body["xml"];

	// ����
	SendXmlCardMessage(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)xml.GetString()));

	return TRUE;
}
// ����xml��Ϣ
bool Receive::MessageSendXml(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("type") || !body.HasMember("wxid") || !body.HasMember("fromWxid") || !body.HasMember("imageUrl") || !body.HasMember("xml")) {
		return FALSE;
	}
	Value& type = body["type"];
	Value& wxid = body["wxid"];
	Value& fromWxid = body["fromWxid"];
	Value& imageUrl = body["imageUrl"];
	Value& xml = body["xml"];

	// ����
	SendXmlMessage(type.GetInt(), UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)fromWxid.GetString()), UTF8ToUnicode((char*)imageUrl.GetString()), UTF8ToUnicode((char*)xml.GetString()));

	return TRUE;
}
// wxid�Ӻ���
bool Receive::FriendAdd(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("message")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& message = body["message"];

	// ����
	AddFriend(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)message.GetString()));

	return TRUE;
}
// v1�Ӻ���
bool Receive::FriendAddFromV1(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("v1") || !body.HasMember("message")) {
		return FALSE;
	}
	Value& v1 = body["v1"];
	Value& message = body["message"];

	// ����
	AddUserFromCard(UTF8ToUnicode((char*)v1.GetString()), UTF8ToUnicode((char*)message.GetString()));

	return TRUE;
}
// ɾ������
bool Receive::FriendDelete(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];

	// ����
	DeleteUser(UTF8ToUnicode((char*)wxid.GetString()));

	return TRUE;
}
// v1+v2ͬ���������
bool Receive::FriendVerify(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("v1") || !body.HasMember("v2")) {
		return FALSE;
	}
	Value& v1 = body["v1"];
	Value& v2 = body["v2"];

	// ����
	VerifyFriend(UTF8ToUnicode((char*)v1.GetString()), UTF8ToUnicode((char*)v2.GetString()));

	return TRUE;
}
// �����б�
bool Receive::FriendList(Package *package)
{
	// ���ͺ����б�
	selectFriendList();
	return TRUE;
}
// ���ú��ѱ�ע
bool Receive::FriendRemark(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("remark")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& remark = body["remark"];
	
	SetRemark(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)remark.GetString()));
	return TRUE;
}
// ����Ⱥ��
bool Receive::RoomCreate(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid1") || !body.HasMember("wxid2")) {
		return FALSE;
	}
	Value& wxid1 = body["wxid1"];
	Value& wxid2 = body["wxid2"];

	// ����
	CreateRoom(UTF8ToUnicode((char*)wxid1.GetString()), UTF8ToUnicode((char*)wxid2.GetString()));

	return TRUE;
}
// �޸�Ⱥ����
bool Receive::RoomEditName(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId") || !body.HasMember("roomName")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];
	Value& roomName = body["roomName"];

	// ����
	SetRoomName(UTF8ToUnicode((char*)roomId.GetString()), UTF8ToUnicode((char*)roomName.GetString()));

	return TRUE;
}
// ����Ⱥ����
bool Receive::RoomAnnouncement(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId") || !body.HasMember("announcement")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];
	Value& announcement = body["announcement"];

	// ����
	SetRoomAnnouncement(UTF8ToUnicode((char*)roomId.GetString()), UTF8ToUnicode((char*)announcement.GetString()));

	return TRUE;
}
// ��ȡȺ��Ա�б�
bool Receive::RoomMemberList(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];

	// ����
	GetRoomUserList(UTF8ToUnicode((char*)roomId.GetString()));

	return TRUE;
}
// ��������Ⱥ
bool Receive::RoomAddMember(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId") || !body.HasMember("wxid")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];
	Value& wxid = body["wxid"];

	// ����
	AddRoomUser(UTF8ToUnicode((char*)roomId.GetString()), UTF8ToUnicode((char*)wxid.GetString()));

	return TRUE;
}
// ɾ��Ⱥ��Ա
bool Receive::RoomDeleteMember(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId") || !body.HasMember("wxid")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];
	Value& wxid = body["wxid"];

	// ����
	DelRoomUser(UTF8ToUnicode((char*)roomId.GetString()), UTF8ToUnicode((char*)wxid.GetString()));

	return TRUE;
}
// ����Ⱥ��Ա
bool Receive::RoomAtMember(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId") || !body.HasMember("wxid") || !body.HasMember("nickname") || !body.HasMember("message")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];
	Value& wxid = body["wxid"];
	Value& nickname = body["nickname"];
	Value& message = body["message"];

	// ����
	SendRoomAtMsg(UTF8ToUnicode((char*)roomId.GetString()), UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)nickname.GetString()), UTF8ToUnicode((char*)message.GetString()));

	return TRUE;
}
// �˳�Ⱥ��
bool Receive::RoomQuit(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("roomId")) {
		return FALSE;
	}
	Value& roomId = body["roomId"];

	// ����
	QuitRoom(UTF8ToUnicode((char*)roomId.GetString()));

	return TRUE;
}
// �տ�
bool Receive::TransferRecv(Package *package)
{
	Value *pBody = package->GetBody();
	Value &body = *pBody;
	if (!body.HasMember("wxid") || !body.HasMember("transferId")) {
		return FALSE;
	}
	Value& wxid = body["wxid"];
	Value& transferId = body["transferId"];

	// ����
	CollectMoney(UTF8ToUnicode((char*)wxid.GetString()), UTF8ToUnicode((char*)transferId.GetString()));

	return TRUE;
}

// ���ݰ�����
int Receive::Handle(Package *package)
{
	int opCode = package->GetOpCode();

	int result = 0;  // 0=����,1=��֧�ֵ�ָ��,2=�쳣
	bool doResult = FALSE;  // ִ�н��

	// ��ȡ��¼����
	if (opCode == OpCode::OPCODE_LOGIN_INFO) {
		doResult = this->GetLoginInfo(package);
	}
	// ��ȡ��¼״̬
	else if (opCode == OpCode::OPCODE_WECHAT_GET_LOGIN_STATUS) {
		doResult = this->GetLoginStatus(package);
	}
	// �˳���¼
	else if (opCode == OpCode::OPCODE_WECHAT_QUIT) {
		doResult = this->Logout(package);
	}
	// �����ı���Ϣ
	else if (opCode == OpCode::OPCODE_MESSAGE_SEND_TEXT) {
		doResult = this->MessageSendText(package);
	}
	// ����ͼƬ��Ϣ
	else if (opCode == OpCode::OPCODE_MESSAGE_SEND_IMAGE) {
		doResult = this->MessageSendImage(package);
	}
	// ���͸�����Ϣ
	else if (opCode == OpCode::OPCODE_MESSAGE_SEND_FILE) {
		doResult = this->MessageSendFile(package);
	}
	// ������Ƭ��Ϣ
	else if (opCode == OpCode::OPCODE_MESSAGE_SEND_CARD) {
		doResult = this->MessageSendCard(package);
	}
	// ����xml��Ϣ
	else if (opCode == OpCode::OPCODE_MESSAGE_SEND_XML) {
		doResult = this->MessageSendXml(package);
	}
	// ��ȡ�����б�
	else if (opCode == OpCode::OPCODE_FRIEND_LIST) {
		doResult = this->FriendList(package);
	}
	// ��ȡ�����б�
	else if (opCode == OpCode::OPCODE_FRIEND_REMARK) {
		doResult = this->FriendRemark(package);
	}
	// wxid�Ӻ���
	else if (opCode == OpCode::OPCODE_FRIEND_ADD) {
		doResult = this->FriendAdd(package);
	}
	// v1�Ӻ���
	else if (opCode == OpCode::OPCODE_FRIEND_ADD_FROM_V1) {
		doResult = this->FriendAddFromV1(package);
	}
	// ɾ������
	else if (opCode == OpCode::OPCODE_FRIEND_DELETE) {
		doResult = this->FriendDelete(package);
	}
	// v1+v2ͬ���������
	else if (opCode == OpCode::OPCODE_FRIEND_VERIFY) {
		doResult = this->FriendVerify(package);
	}
	// ����Ⱥ��
	else if (opCode == OpCode::OPCODE_ROOM_CREATE) {
		doResult = this->RoomCreate(package);
	}
	// �޸�Ⱥ����
	else if (opCode == OpCode::OPCODE_ROOM_EDIT_NAME) {
		doResult = this->RoomEditName(package);
	}
	// ����Ⱥ����
	else if (opCode == OpCode::OPCODE_ROOM_ANNOUNCEMENT) {
		doResult = this->RoomAnnouncement(package);
	}
	// ��ȡȺ��Ա�б�
	else if (opCode == OpCode::OPCODE_ROOM_MEMBER_LIST) {
		doResult = this->RoomMemberList(package);
	}
	// ��������Ⱥ
	else if (opCode == OpCode::OPCODE_ROOM_ADD_MEMBER) {
		doResult = this->RoomAddMember(package);
	}
	// ɾ��Ⱥ��Ա
	else if (opCode == OpCode::OPCODE_ROOM_DELETE_MEMBER) {
		doResult = this->RoomDeleteMember(package);
	}
	// ����Ⱥ��Ա
	else if (opCode == OpCode::OPCODE_ROOM_AT_MEMBER) {
		doResult = this->RoomAtMember(package);
	}
	// �˳�Ⱥ��
	else if (opCode == OpCode::OPCODE_ROOM_QUIT) {
		doResult = this->RoomQuit(package);
	}
	// �տ�
	else if (opCode == OpCode::OPCODE_TRANSFER_RECV) {
		doResult = this->TransferRecv(package);
	}
	else {
		result = 1;   // 0=����,1=��֧�ֵ�ָ��,2=�쳣
	}

	// ����������ľ�ֱ�ӷ���
	if (doResult == TRUE) {
		return result;
	}

	// ����ʧ�ܵ�
	if (result == 0) {  // doResult == FALSE && result == 0
		result = 2;  // 0=����,1=��֧�ֵ�ָ��,2=�쳣
	}

	// �쳣ָ��
	package->SetOpCode(OpCode::OPCODE_FAILURE);
	// ����body
	rapidjson::Value newBody(rapidjson::kObjectType); // ����һ��Object���͵�Ԫ��
	// Դ����ָ��
	rapidjson::Value oldOpCode(rapidjson::kNumberType);
	oldOpCode.SetInt(opCode);
	newBody.AddMember("opCode", opCode, package->json.GetAllocator());
	// ������
	rapidjson::Value errorCode(rapidjson::kNumberType);
	errorCode.SetInt(result);
	newBody.AddMember("errorCode", errorCode, package->json.GetAllocator());
	// ������Ϣ
	rapidjson::Value errorMessage(rapidjson::kStringType);
	if (result == 1) {
		char str[] = "��֧�ֵ�ָ��";
		errorMessage.SetString(GB2312ToUTF8(str), strlen(GB2312ToUTF8(str)));
		newBody.AddMember("errorMessage", errorMessage, package->json.GetAllocator());
	}
	else if (result == 2) {
		char str[] = "ִ���쳣������ҵ�������";
		errorMessage.SetString(GB2312ToUTF8(str), strlen(GB2312ToUTF8(str)));
		newBody.AddMember("errorMessage", errorMessage, package->json.GetAllocator());
	}
	else {
		char str[] = "δ֪�Ĵ���";
		errorMessage.SetString(GB2312ToUTF8(str), strlen(GB2312ToUTF8(str)));
		newBody.AddMember("errorMessage", errorMessage, package->json.GetAllocator());
	}
	package->SetBody(newBody);
	rapidjson::StringBuffer pack = package->GetConText();
	WsClientSend((char*)pack.GetString());

	return result;
}