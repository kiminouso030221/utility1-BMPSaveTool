#include "pch.h"
#include "CBmpExtractor.h"


CBMPExtractor g_myObj;

DWORD WINAPI Implementation(TCHAR* _Out_ result)
{
	switch (g_myObj.CreateBMPFile())
	{
	case SUCCESS:
	case NOBMPDATA:
		_stprintf_s(result, SZRESULTLENGTH, _T("NOERROR"));
		break;
	case CREATEDIR_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("����Ŀ¼ʧ��"));
		break;
	case GETDIR_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("��ȡ����Ŀ¼ʧ��"));
		break;
	case OPENCB_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("�򿪼��а�ʧ��"));
		break;
	case GETNAME_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("��ȡ�ļ���ʧ��"));
		break;
	case OPENFILE_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("�����ļ�ʧ��"));
		break;
	case GETDATA_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("��ȡ���а�λͼ����ʧ��"));
		break;
	case WRITEFILE_FAILED:
		_stprintf_s(result, SZRESULTLENGTH, _T("д���ļ�ʧ��"));
		break;
	default:
		_stprintf_s(result, SZRESULTLENGTH, _T("�������δ����Ĵ���, ������ͨ���˳����\"����\"���ڻ�����ߵ���ϵ��ʽ, ��������"));
	}

	return 0;
}

