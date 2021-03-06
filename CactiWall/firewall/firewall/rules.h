#pragma once

#include<ntddk.h>


#define ZERO_CRC_VALUE          (0x87326698);



////////////////////////////////进程规则声明/////////////////////////////////

#define MAX_PROCESS_RULES_NUM   255


#define STATUS_PROCESS_RULES_FULL \
        (0x60000001L)
#define STATUS_PROCESS_RULES_EXISTED \
        (0x60000002L)
#define STATUS_PROCESS_RULES_EMPTY \
        (0x60000003L)
#define STATUS_PROCESS_RULES_NOT_EXISTED \
        (0x60000004L)

typedef struct _PROCESS_RULES_ELEM
{
    UINT32   crcPath;
    UINT32   rule;      //32位值，各个位的功能参看下边的宏定义
}PROCESS_RULES_ELEM,*PPROCESS_RULES_ELEM;

#define PROCESS_RULE_FLAG_ALLOW_ACCESS  ((UINT32)(1<<0))


typedef struct _PROCESS_RULES_TABLE
{
    UINT8                count;
    PROCESS_RULES_ELEM  rules[ MAX_PROCESS_RULES_NUM ];
}PROCESS_RULES_TABLE,*PPROCESS_RULES_TABLE;

NTSTATUS
InitProcessRules();

NTSTATUS
AddProcessRule( IN UINT32 crcPath,IN UINT32 rule );

NTSTATUS
DeleteProcessRule( IN UINT32 crcPath );

NTSTATUS
GetProcessRule( IN UINT32 crcPath,IN OUT UINT32 *rule );

BOOLEAN
IsProcessRuleExist( IN UINT32 crcPath );

/////////////////////////////////////////////////////////////////////////////



//////////////////////////////IP规则声明/////////////////////////////////////



#define STATUS_IP_RULES_FULL \
        (0x60000001L)
#define STATUS_IP_RULES_EXISTED \
        (0x60000002L)
#define STATUS_IP_RULES_EMPTY \
        (0x60000003L)
#define STATUS_IP_RULES_NOT_EXISTED \
        (0x60000004L)

enum
{
    AnyAddr=0,
    UniqueAddr,
    RangeAddr,
    UnknownAddr
};

enum
{
    RulesDirectionAny=0,
    RulesDirectionUp,
    RulesDirectionDown,
    RulesDirectionUnknown
};

enum
{
    RulesProtocolAny = 0
};

typedef struct _IP_RULES_ELEM
{
    LIST_ENTRY  list;

    UINT32   crcRuleName;   //IP规则名称的32位crc值（对应注册表中相应的项）
    
    union{
        UINT32   u32;
        struct
        {
            UINT32  RemoteAddrType :2;  //取值为AnyAddr,UniqueAddr,RangeAddr
            UINT32  LocalAddrType :2;   //取值为AnyAddr,UniqueAddr,RangeAddr
            UINT32  RemotePortType :2;  //取值为AnyAddr,UniqueAddr,RangeAddr
            UINT32  LocalPortType :2;   //取值为AnyAddr,UniqueAddr,RangeAddr
            UINT32  ProtocolType :8;//网络协议类型
            UINT32  Direction :2;//00：任意方向 01：上行  10:下行  11：未定义
            UINT32  Access  :1;//是否允许访问，1为允许
            UINT32  IcmpType :5;
            UINT32  IcmpCode :5;
            UINT32  Reserved :3;
        }Bits;
    }rule;

    UINT32   LocalAddr;
    UINT32   LocalAddr2;
    UINT32   RemoteAddr;
    UINT32   RemoteAddr2;
    UINT16   LocalPort;
    UINT16   LocalPort2;
    UINT16   RemotePort;
    UINT16   RemotePort2;

}IP_RULES_ELEM,*PIP_RULES_ELEM;

typedef struct _IP_RULES_LIST
{
    LIST_ENTRY          list;
    LONG                count;
    KSPIN_LOCK          lock;
}IP_RULES_LIST,*PIP_RULES_LIST;

NTSTATUS
InitIpRules();

NTSTATUS
AddIpRule( IN PIP_RULES_ELEM elem );

NTSTATUS
DeleteIpRule( IN PIP_RULES_ELEM rule );

NTSTATUS
DeleteIpRuleByCrcName( IN UINT32 crcName );

NTSTATUS
GetIpRuleByCrcName( IN UINT32 crcName,IN OUT PIP_RULES_ELEM *rule );

BOOLEAN
IsIpRuleExist( IN UINT32 crcName );

VOID
ClearIpRulesList();


///////////////////////////////END///////////////////////////////////////////




////////////////////////////////DNS规则声明/////////////////////////////////


#define STATUS_DNS_RULES_FULL \
        (0x60000001L)
#define STATUS_DNS_RULES_EXISTED \
        (0x60000002L)
#define STATUS_DNS_RULES_EMPTY \
        (0x60000003L)
#define STATUS_DNS_RULES_NOT_EXISTED \
        (0x60000004L)

typedef struct _DNS_RULES_ELEM
{
    LIST_ENTRY          list;
    UINT32              crcRuleName;
    PMY_UNICODE_STRING  dnsName;
    UINT32              rule;      //32位值，各个位的功能参看下边的宏定义
}DNS_RULES_ELEM,*PDNS_RULES_ELEM;

#define DNS_RULE_FLAG_ALLOW_ACCESS  ((UINT32)(1<<0))


typedef struct _DNS_RULES_LIST
{
    LIST_ENTRY          list;
    LONG                count;
    KSPIN_LOCK          lock;
}DNS_RULES_LIST,*PDNS_RULES_LIST;

NTSTATUS
InitDnsRules();

NTSTATUS
AddDnsRule( IN WCHAR dnsName[],IN ULONG Length,IN BOOLEAN bAllow,IN UINT32 crcName );

NTSTATUS
DeleteDnsRule( IN PDNS_RULES_ELEM rule );

NTSTATUS
DeleteDnsRuleByCrcName( IN UINT32 crcName );

NTSTATUS
GetDnsRuleByCrcName( IN UINT32 crcName,IN OUT PDNS_RULES_ELEM *rule );

BOOLEAN
IsDnsRuleExist( IN UINT32 crcName );

VOID
ClearDnsRulesList();                                                                                                                                                                                                                                                                                                                                                                                                                                                                        