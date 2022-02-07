#pragma once
#include "CAppConfig.h"
#define SUCCESS             0
#define CREATEDIR_FAILED    1
#define GETDIR_FAILED       2
#define OPENCB_FAILED       3
#define NOBMPDATA           4
#define GETNAME_FAILED      5
#define OPENFILE_FAILED     6
#define GETDATA_FAILED      7
#define WRITEFILE_FAILED    8

#define SZRESULTLENGTH      50


//ʵ�ִ��ڴ�����ȡBMP���ļ�����

class CBMPExtractor : public CAppConfig
{
private:
    TCHAR m_tszPath[PATH_LENGTH];
    TCHAR m_tszFileName[FILENAME_LENGTH];
    TCHAR m_tszFinalPath[MAX_PATH];

    LPBITMAPINFOHEADER m_lpDIB;
    HANDLE m_hFile;

    SIZE_T m_sizeofData;

    DWORD WriteBMPFile()
    {
        /*�򵥼�¼��bmp�ļ����:
        1.BITMAPFILEHEADER
        2.BITMAPINFOHEADER
        (��Щ��Ҫ��ɫ�壩
        3.DATA
        �ڴ�����CF_BITMAP����CF_DIB����,ԭ��δ֪
        */

        SIZE_T writtenbCnt[3];
        BOOL flagChk[3];
        BOOL has12byteOffset = 0;
        const SIZE_T sizeofImage = m_lpDIB->biSizeImage;
        BITMAPFILEHEADER fileH;
        LPBYTE  lpData = (LPBYTE)m_lpDIB;

        lpData += sizeof(BITMAPINFOHEADER);     //ָ��ָ��λͼ������,����ΪLPBYTE���ں����ƶ�ָ��
        m_lpDIB->biCompression = 0;         //�㲻��Ϊʲôԭʼ��ѹ������������3,һֱ���������ֵ�����ļ��򲻿����ĳ�0�ͺ���
        fileH.bfType = 0x4d42;
        fileH.bfReserved1 = 0;
        fileH.bfReserved2 = 0;
        fileH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);      //ƫ��
        fileH.bfSize = sizeofImage + fileH.bfOffBits;
        if (m_sizeofData - m_lpDIB->biSize - sizeofImage == 12)     //12�ֽ�ƫ�Ƽ��жϣ�PrtScr������,��BMP��Ϣ�����12�ֽڵ����ݣ������ȥ�����ᵼ��ͼƬ��λ��
            has12byteOffset = 1;

        flagChk[0] = WriteFile(m_hFile, &fileH,
            sizeof(BITMAPFILEHEADER),
            &writtenbCnt[0],
            NULL);
        //   SetFilePointer(m_hFile, 0, NULL, FILE_END);        //������д���ļ����ļ�ָ���Զ����ƣ������ֶ�����

        flagChk[1] = WriteFile(m_hFile, m_lpDIB,
            sizeof(BITMAPINFOHEADER),
            &writtenbCnt[1],
            NULL);
        // SetFilePointer(m_hFile, 0, NULL, FILE_END);

        if (has12byteOffset)
        {
            lpData += 12;
        }

        flagChk[2] = WriteFile(m_hFile, lpData,
            sizeofImage,
            &writtenbCnt[2],
            NULL);

        if (flagChk[0] && flagChk[1] && flagChk[2])
            return SUCCESS;
        else
            return WRITEFILE_FAILED;
    }


    DWORD GetFileName()          //����ļ���ͬʱ���·���������ļ���
    {
        if (GetSavePathCfg(m_tszPath))          //��ȡ·��
        {
            TCHAR substr_Dir[PATH_LENGTH] = { 0 };
            //�༶Ŀ¼����
            if (m_tszPath[_tcslen(m_tszPath) - 1] != '\\')
            {
                _tcscat_s(m_tszPath, PATH_LENGTH, _T("\\"));
            }

            for (register int i = 0; i < _tcslen(m_tszPath); ++i)
            {
                if (m_tszPath[i] == '\\')
                {
                    _tcsncpy_s(substr_Dir, PATH_LENGTH, m_tszPath, i + 1);
                    if (
                        !(PathIsDirectory(m_tszPath) || 
                        CreateDirectory(m_tszPath, NULL))
                        )                                       //���Ŀ¼�Ƿ���� �ر�ע�⣺���Ŀ¼�Ѿ����ڣ�CreateDirectory����ֵΪ0
                    {
                        return CREATEDIR_FAILED; 
                    }
                }
            }
          
            SYSTEMTIME time;
            GetLocalTime(&time);
            _stprintf_s(m_tszFileName, FILENAME_LENGTH, _T("%d-%d-%d_%d-%d-%02d.bmp"),
                time.wYear,
                time.wMonth,
                time.wDay,
                time.wHour,
                time.wMinute,
                time.wSecond);
            _tcscpy_s(m_tszFinalPath, MAX_PATH, m_tszPath);        //����Ŀ¼����
            _tcscat_s(m_tszFinalPath, MAX_PATH, m_tszFileName);    //�����ļ���
            return SUCCESS;
        }
        return GETDIR_FAILED;
    }



public:
    CBMPExtractor()
    {
        m_hFile = nullptr;
        m_lpDIB = nullptr;
        m_sizeofData = 0;
        memset(m_tszPath, 0, sizeof(m_tszPath));
        memset(m_tszFileName, 0, sizeof(m_tszFileName));
        memset(m_tszFinalPath, 0, sizeof(m_tszFinalPath));
    }

    ~CBMPExtractor()
    {
        CloseHandle(m_hFile);   
        CloseClipboard();
    }


    DWORD CreateBMPFile()
    {
        DWORD state = -1;
        if (!OpenClipboard(NULL))
            return OPENCB_FAILED;
        if (IsClipboardFormatAvailable(CF_DIB))        //�򿪼��а�����ᵼ�����������޷�д��
        {
            state = GetFileName();
            if (state == SUCCESS)
            {
                HGLOBAL hBMPData = GetClipboardData(CF_DIB);            //��ü��а�����
                m_lpDIB = (LPBITMAPINFOHEADER)GlobalLock(hBMPData);     //�������а��ڴ� 
                m_hFile = CreateFile(m_tszFinalPath, GENERIC_WRITE,        //�����ļ�
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
                if (m_hFile)               //�ļ�������ڴ�����״̬
                {
                    if (m_lpDIB)
                    {
                        m_sizeofData = GlobalSize(hBMPData);
                        state = WriteBMPFile();
                        CloseHandle(m_hFile);
                        GlobalUnlock(m_lpDIB);
                    }
                    else
                        state = GETDATA_FAILED;
                }
                else
                    state = OPENFILE_FAILED;
            }
        }
        else
            state = NOBMPDATA;
        CloseClipboard();
        return state;
    }

};


