#pragma once
#include "framework.h"

#define FILENAME_LENGTH 40
#define PATH_LENGTH (MAX_PATH - FILENAME_LENGTH)


//ʵ�ֳ�������,��ȡ/д�����õ���
class CAppConfig
{
private:
	WORD m_cfg_nKey[2];
	HKEY m_cfg_hKey;
	TCHAR m_cfg_tszPath[PATH_LENGTH];

protected:		//�������������,���ƴ�������,���󴴽�ʱ,Ŀ¼�Ͱ���ΪĬ�ϵ�
	CAppConfig()
	{
		memset(m_cfg_tszPath, 0, sizeof(m_cfg_tszPath));
		m_cfg_hKey = NULL;
		LPITEMIDLIST dirID;
		if (SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &dirID) == S_OK)  //�������·��
		{
			if (SHGetPathFromIDList(dirID, m_cfg_tszPath))
			{
				wcscat_s(m_cfg_tszPath, PATH_LENGTH, _T("\\screenshots\\"));	//��ʼ���洢·�����ȼ�
				m_cfg_nKey[0] = VK_NUMPAD9;
				m_cfg_nKey[1] = HOTKEYF_CONTROL;
				return;
			}
		}
		MessageBox(NULL, TEXT("�����ʼ��ʧ��, �ó��������BUG���ʺϴ�ϵͳ�����л���, ������ͨ���������\"����\"���ڻ�ȡ���ߵ���ϵ��ʽ, ��������"), TEXT("����"), MB_OK | MB_ICONERROR);
	}

public:
	BOOL __fastcall GetSavePathCfg(const PTCHAR _Out_ strBuf)
	{
		if (_stprintf_s(strBuf, PATH_LENGTH, _T("%s"), m_cfg_tszPath) > 0)
			return TRUE;
		return FALSE;
	}

	BOOL __fastcall GetHotKeyCfg(PWORD _Out_ nKey)
	{
		nKey[0] = m_cfg_nKey[0];
		nKey[1] = m_cfg_nKey[1];
		return TRUE;
	}

	BOOL __fastcall SetHotKeyCfg(const PWORD _In_ nNewKey)
	{
		m_cfg_nKey[0] = nNewKey[0];
		m_cfg_nKey[1] = nNewKey[1];
		return TRUE;
	}

	BOOL __fastcall SetSavePathCfg(const PTCHAR _In_ strNewPath)
	{
		if (_stprintf_s(m_cfg_tszPath, PATH_LENGTH, _T("%s"), strNewPath) > 0)
			return TRUE;
		return FALSE;
	}

	//��ȡ������Ϣ
	BOOL ReadConfig()		//��ע����ж�ȡ������Ϣ����Ա����,���û������Ĭ��������Ϣ���浽ע���
	{
		DWORD size_keyPath = sizeof(m_cfg_tszPath);
		DWORD size_keyKeys = sizeof(m_cfg_nKey);
		TCHAR szPathSize[9];
		TCHAR szKeySize[9];
		if (RegCreateKey(HKEY_CURRENT_USER, _T("Software\\ClipboardBMPSaveTool"), &m_cfg_hKey) == ERROR_SUCCESS)
		{
			if (RegGetValue(m_cfg_hKey, NULL, _T("Path"), RRF_RT_REG_BINARY, NULL, m_cfg_tszPath, &size_keyPath) == ERROR_SUCCESS &&	//��ȡʧ�ܲ���ı�ԭֵ(ԭֵ���ڹ��캯�����ʼ��)
				RegGetValue(m_cfg_hKey, NULL, _T("Keys"), RRF_RT_DWORD, NULL, m_cfg_nKey, &size_keyKeys) == ERROR_SUCCESS)  //����sizeֵ,����WCHAR��Ҫx2
			{
					RegCloseKey(m_cfg_hKey);
					return TRUE;
				
			}
			else
			{
				if (RegSetKeyValue(m_cfg_hKey, NULL, _T("Path"), REG_BINARY, m_cfg_tszPath, sizeof(m_cfg_tszPath)) == ERROR_SUCCESS &&
					RegSetKeyValue(m_cfg_hKey, NULL, _T("Keys"), REG_DWORD, m_cfg_nKey, sizeof(m_cfg_nKey)) == ERROR_SUCCESS)
				{
					RegCloseKey(m_cfg_hKey);
					return TRUE;
				}
			}

			_stprintf_s(szPathSize, 9, _T("%d"), size_keyPath);
			MessageBox(NULL, szPathSize, TEXT("GH"), MB_OK);
		}
		return FALSE;
	}

	//����������Ϣ��ע���(�����Ա������ֵ)
	BOOL SaveConfig()
	{
		if (RegCreateKey(HKEY_CURRENT_USER, _T("Software\\ClipboardBMPSaveTool"), &m_cfg_hKey) == ERROR_SUCCESS)	//��������¶�Ϊ�ɹ�
		{
			if (RegSetKeyValue(m_cfg_hKey, NULL, _T("Path"), REG_BINARY, m_cfg_tszPath, sizeof(m_cfg_tszPath)) == ERROR_SUCCESS &&	//û�м��ᴴ����
				RegSetKeyValue(m_cfg_hKey, NULL, _T("Keys"), REG_DWORD, m_cfg_nKey, sizeof(m_cfg_nKey)) == ERROR_SUCCESS)
			{
				RegCloseKey(m_cfg_hKey);
				return TRUE;
			}
		}
		return FALSE;
	}

};

