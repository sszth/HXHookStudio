// CHXInjectionShowMessage.cpp: 实现文件
//

#include "stdafx.h"
#include "CHXInjectionShowMessage.h"
#include "afxdialogex.h"


// CHXInjectionShowMessage 对话框

IMPLEMENT_DYNAMIC(CHXInjectionShowMessage, CDialogEx)

CHXInjectionShowMessage::CHXInjectionShowMessage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CHXInjectionShowMessage::~CHXInjectionShowMessage()
{
}

void CHXInjectionShowMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHXInjectionShowMessage, CDialogEx)
END_MESSAGE_MAP()


// CHXInjectionShowMessage 消息处理程序
