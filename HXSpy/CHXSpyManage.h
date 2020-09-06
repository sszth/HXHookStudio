#pragma once


class CHXSpyManage
{
	DECLARE_SINGLETON(CHXSpyManage);
public:
	void Init();

	MapProcess GetProcess() const;
	MapThread GetThread() const;
	MapHeapList GetHeapList() const;
	MapModuleEntry GetModuleEntry() const;


	static LRESULT WINAPI InjectLib(DWORD dwProcessId, PWCHAR pszLibFile);

private:
	MapProcess m_MapProcess;
	MapThread m_MapThread;
	MapHeapList m_MapHeapList;
	MapModuleEntry m_MapModuleEntry;
};

