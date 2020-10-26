#include"common.h"
#include "rules.h"

PROCESS_RULES_TABLE gProcessRulesTable={0};
IP_RULES_LIST       gIpRulesList = {0};
DNS_RULES_LIST      gDnsRulesList = {0};

NTSTATUS
InitProcessRules()
/*++
--*/
{
    LOG("into\n");

    RtlZeroMemory( &gProcessRulesTable,sizeof(PROCESS_RULES_TABLE ));

    return STATUS_SUCCESS;
}

NTSTATUS
AddProcessRule( IN UINT32 crcPath,IN UINT32 rule )
/*++
--*/
{
    UINT8   xorsum = 0;
    UINT32  key,i;

    LOG("into\n");

    if( gProcessRulesTable.count >= MAX_PROCESS_RULES_NUM )
        return STATUS_PROCESS_RULES_FULL;

    if ( IsProcessRuleExist( crcPath ))
        return STATUS_PROCESS_RULES_EXISTED;

    if( crcPath == 0 )crcPath = ZERO_CRC_VALUE;
    key = crcPath;

    for( i = 0;i < 32;i++)
    {
        xorsum ^= key & 0xff;
        key >>= 1;
    }

    for( i = xorsum;;i = (i + 1 ) % MAX_PROCESS_RULES_NUM )
    {
        if( gProcessRulesTable.rules[i].crcPath == 0 )
            break;
    }
    gProcessRulesTable.rules[i].crcPath = crcPath;
    gProcessRulesTable.rules[i].rule = rule;
    gProcessRulesTable.count++;

    return STATUS_SUCCESS;
}

NTSTATUS
DeleteProcessRule( IN UINT32 crcPath )
/*++
--*/
{
    UINT8   xorsum = 0;
    UINT32  key,i;

    LOG("into\n");

    if( gProcessRulesTable.count == 0 )
        return STATUS_PROCESS_RULES_EMPTY;

    if( !IsProcessRuleExist( crcPath ) )
        return STATUS_PROCESS_RULES_NOT_EXISTED;

    if( crcPath == 0 )crcPath = ZERO_CRC_VALUE;
    key = crcPath;

    for( i = 0;i < 32;i++)
    {
        xorsum ^= key & 0xff;
        key >>= 1;
    }

    for( i = xorsum;;i = (i + 1 ) % MAX_PROCESS_RULES_NUM )
    {
        if( gProcessRulesTable.rules[i].crcPath == crcPath )
            break;
    }
    gProcessRulesTable.rules[i].crcPath = 0;
    gProcessRulesTable.rules[i].rule = 0;
    gProcessRulesTable.count--;

    return STATUS_SUCCESS;
}

NTSTATUS
GetProcessRule( IN UINT32 crcPath, IN OUT UINT32 *rule )
/*++
--*/
{
    UINT8   xorsum = 0;
    UINT32  key,i;

    LOG("into\n");

    if( gProcessRulesTable.count == 0 )
        return STATUS_PROCESS_RULES_EMPTY;

    if( !IsProcessRuleExist( crcPath ) )
        return STATUS_PROCESS_RULES_NOT_EXISTED;

    if( crcPath == 0 )crcPath = ZERO_CRC_VALUE;
    key = crcPath;

    for( i = 0;i < 32;i++)
    {
        xorsum ^= key & 0xff;
        key >>= 1;
    }

    for( i = xorsum;;i = (i + 1 ) % MAX_PROCESS_RULES_NUM )
    {
        if( gProcessRulesTable.rules[i].crcPath == crcPath )
            break;
    }
    
    *rule = gProcessRulesTable.rules[i].rule;

    return STATUS_SUCCESS;
}

BOOLEAN
IsProcessRuleExist( IN UINT32 crcPath )
/*++
--*/
{
    UINT8   xorsum = 0;
    UINT32  key,i;

    LOG("into\n");

    if( crcPath == 0 )crcPath = ZERO_CRC_VALUE;
    key = crcPath;

    for( i = 0;i < 32;i++)
    {
        xorsum ^= key & 0xff;
        key >>= 1;
    }

    for( i = xorsum;;)
    {
        if( gProcessRulesTable.rules[i].crcPath == crcPath)
            return TRUE;
        if( gProcessRulesTable.rules[i].crcPath == 0 )
            return FALSE;

        i = (i + 1 ) % MAX_PROCESS_RULES_NUM;
        if( i == xorsum )break;
    }

    return FALSE;
}

NTSTATUS
InitIpRules()
/*++
--*/
{
    LOG("into\n");

    RtlZeroMemory( &gIpRulesList,sizeof( gIpRulesList) );

    InitializeListHead( &gIpRulesList.list );
    KeInitializeSpinLock( &gIpRulesList.lock );
    gIpRulesList.count = 0;

    return STATUS_SUCCESS;
}

NTSTATUS
AddIpRule( IN PIP_RULES_ELEM elem )
/*++
--*/
{
    PIP_RULES_ELEM rule = NULL;

    LOG("into\n");

    rule = MyExAllocatePool( sizeof( IP_RULES_ELEM ));
    ASSERT( rule != NULL );
    RtlZeroMemory( rule,sizeof(IP_RULES_ELEM));
    *rule = *elem;

    if( rule->rule.Bits.RemoteAddrType == AnyAddr ||
        rule->rule.Bits.RemoteAddrType == RangeAddr ||
        rule->rule.Bits.LocalAddrType == AnyAddr ||
        rule->rule.Bits.LocalAddrType == RangeAddr ||
        rule->rule.Bits.RemotePortType == AnyAddr ||
        rule->rule.Bits.RemotePortType == RangeAddr ||
        rule->rule.Bits.LocalPortType == AnyAddr ||
        rule->rule.Bits.LocalPortType == RangeAddr )
    {
        ExInterlockedInsertHeadList( &gIpRulesList.list,
                                    &rule->list,
                                    &gIpRulesList.lock );
    }
    else
    {
        ExInterlockedInsertTailList( &gIpRulesList.list,
                                    &rule->list,
                                    &gIpRulesList.lock );

    }
    
    InterlockedIncrement( &gIpRulesList.count );

    return STATUS_SUCCESS;
}

NTSTATUS
DeleteIpRule( IN PIP_RULES_ELEM rule )
/*++
--*/
{
    KIRQL       oldIrql;
    BOOLEAN     bSuccess;

    LOG("into\n");

    KeAcquireSpinLock( &gIpRulesList.lock,&oldIrql );
    bSuccess = RemoveEntryList( &rule->list );
    KeReleaseSpinLock( &gIpRulesList.lock,oldIrql );

    if( bSuccess )
    {
        MyExFreePool( rule );
        rule = NULL;
        InterlockedDecrement( &gIpRulesList.count );

        return STATUS_SUCCESS;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
}


NTSTATUS
DeleteIpRuleByCrcName( IN UINT32 crcName )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gIpRulesList.list.Flink;
         list != &gIpRulesList.list;
         list = list->Flink )
    {
        if( ((PIP_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gIpRulesList.list )
    {
        return DeleteIpRule( (PIP_RULES_ELEM)list );
    }
    else
    {
        return STATUS_IP_RULES_NOT_EXISTED;
    }
}


NTSTATUS
GetIpRuleByCrcName( IN UINT32 crcName,IN OUT PIP_RULES_ELEM *rule )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gIpRulesList.list.Flink;
         list != &gIpRulesList.list;
         list = list->Flink )
    {
        if( ((PIP_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gIpRulesList.list )
    {
        *rule = (PIP_RULES_ELEM)list;
        return STATUS_SUCCESS;
    }
    else
    {
        *rule = NULL;
        return STATUS_IP_RULES_NOT_EXISTED;
    }
}


BOOLEAN
IsIpRuleExist( IN UINT32 crcName )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gIpRulesList.list.Flink;
         list != &gIpRulesList.list;
         list = list->Flink )
    {
        if( ((PIP_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gIpRulesList.list )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

VOID    ClearIpRulesList()
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    LOG("into\n");

    while( (list = ExInterlockedRemoveHeadList( &gIpRulesList.list,&gIpRulesList.lock))
        != NULL )
    {
        MyExFreePool( list );
        list = NULL;
        InterlockedDecrement( &gIpRulesList.count );

    }
    ASSERT( IsListEmpty( &gIpRulesList.list ));
    ASSERT( gIpRulesList.count == 0 );

    return;
}



NTSTATUS
InitDnsRules()
/*++
--*/
{
    LOG("into\n");

    RtlZeroMemory( &gDnsRulesList,sizeof( gDnsRulesList) );

    InitializeListHead( &gDnsRulesList.list );
    KeInitializeSpinLock( &gDnsRulesList.lock );
    gDnsRulesList.count = 0;

    return STATUS_SUCCESS;

}

NTSTATUS
AddDnsRule( IN WCHAR dnsName[],IN ULONG Length,IN BOOLEAN bAllow ,IN UINT32 crcName )
/*++

NOTE:dnsName为以0结尾的字符串，Length是其长度,单位字节，包括结尾0
--*/
{
    PDNS_RULES_ELEM rule = NULL;

    LOG("into\n");

    rule = MyExAllocatePool( sizeof( DNS_RULES_ELEM ));
    ASSERT( rule != NULL );
    RtlZeroMemory( rule,sizeof(DNS_RULES_ELEM));

    rule->crcRuleName = crcName;
    if( bAllow )
        rule->rule |= DNS_RULE_FLAG_ALLOW_ACCESS;
    rule->dnsName = (PMY_UNICODE_STRING)MyExAllocatePool( sizeof(MY_UNICODE_STRING)+Length );
    ASSERT( rule->dnsName != NULL );
    RtlCopyMemory( rule->dnsName->buffer,dnsName,Length );
    rule->dnsName->str.Buffer = rule->dnsName->buffer;
    rule->dnsName->str.Length = 
        rule->dnsName->str.MaximumLength = (USHORT)Length;

    ExInterlockedInsertHeadList( &gDnsRulesList.list,
                                &rule->list,
                                &gDnsRulesList.lock );

    InterlockedIncrement( &gDnsRulesList.count );

    return STATUS_SUCCESS;
}

NTSTATUS
DeleteDnsRule( IN PDNS_RULES_ELEM rule )
/*++
--*/
{
    KIRQL       oldIrql;
    BOOLEAN     bSuccess;

    LOG("into\n");

    KeAcquireSpinLock( &gIpRulesList.lock,&oldIrql );
    bSuccess = RemoveEntryList( &rule->list );
    KeReleaseSpinLock( &gIpRulesList.lock,oldIrql );

    if( bSuccess )
    {
        if( rule->dnsName != NULL )
        {
            MyExFreePool( rule->dnsName );
            rule->dnsName = NULL;
        }
        MyExFreePool( rule );
        rule = NULL;
        InterlockedDecrement( &gDnsRulesList.count );

        return STATUS_SUCCESS;
    }
    else
    {
        return STATUS_UNSUCCESSFUL;
    }
}

NTSTATUS
DeleteDnsRuleByCrcName( IN UINT32 crcName )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gDnsRulesList.list.Flink;
         list != &gDnsRulesList.list;
         list = list->Flink )
    {
        if( ((PDNS_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gDnsRulesList.list )
    {
        return DeleteDnsRule( (PDNS_RULES_ELEM)list );
    }
    else
    {
        return STATUS_DNS_RULES_NOT_EXISTED;
    }
}

NTSTATUS
GetDnsRuleByCrcName( IN UINT32 crcName,IN OUT PDNS_RULES_ELEM *rule )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gDnsRulesList.list.Flink;
         list != &gDnsRulesList.list;
         list = list->Flink )
    {
        if( ((PDNS_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gDnsRulesList.list )
    {
        *rule = (PDNS_RULES_ELEM)list;
        return STATUS_SUCCESS;
    }
    else
    {
        *rule = NULL;
        return STATUS_DNS_RULES_NOT_EXISTED;
    }
}

BOOLEAN
IsDnsRuleExist( IN UINT32 crcName )
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    for( list = gDnsRulesList.list.Flink;
         list != &gDnsRulesList.list;
         list = list->Flink )
    {
        if( ((PDNS_RULES_ELEM)list)->crcRuleName == crcName )
            break;
    }

    if( list != &gDnsRulesList.list )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

VOID
ClearDnsRulesList()
/*++
--*/
{
    PLIST_ENTRY list = NULL;

    LOG("into\n");

    while( (list = ExInterlockedRemoveHeadList( &gDnsRulesList.list,&gDnsRulesList.lock))
        != NULL )
    {
        if(((PDNS_RULES_ELEM)list)->dnsName != NULL )
        {
            MyExFreePool( ((PDNS_RULES_ELEM)list)->dnsName);
            ((PDNS_RULES_ELEM)list)->dnsName = NULL;
        }
        MyExFreePool( list );
        list = NULL;
        InterlockedDecrement( &gDnsRulesList.count );
    }
    ASSERT( IsListEmpty( &gDnsRulesList.list ));
    ASSERT( gDnsRulesList.count == 0 );

    return;
}
