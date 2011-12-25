// JpegReaderDoc.h : CJpegReaderDoc �N���X�̃C���^�[�t�F�C�X
//

//#include "Dib.h"	// ClassView �ɂ���Ēǉ�����܂����B
#pragma once

class CJpegReaderDoc : public CDocument
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CJpegReaderDoc();
	DECLARE_DYNCREATE(CJpegReaderDoc)

// ����
public:

// ����
public:

// �I�[�o�[���C�h
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ����
public:
	virtual ~CJpegReaderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CDib* GetDib() {return &m_Dib;}			//DIB�ɕϊ�����JPEG�f�R�[�h�ς݃f�[�^�̎擾
	CSize GetPictureSize(void);				// View�ɉ摜�T�C�Y��n��

protected:
	int		m_iHSize;	// JPEG�摜�f�[�^�̕�
	int		m_iVSize;	// JPEG�摜�f�[�^�̍���
	u_char* m_pRedP;	// �Ԃ̗v�f�̃r�b�g�f�[�^
	u_char* m_pGreenP;	// �΂̗v�f�̃r�b�g�f�[�^
	u_char* m_pBlueP;	// �̗v�f�̃r�b�g�f�[�^
	bool	m_bLoadFlg;	// JPEG�t�@�C�����[�h�ς݂��ۂ��̃t���O
	CDib	m_Dib;		// DIB�I�u�W�F�N�g

	bool LoadJpegFile(LPCTSTR pszFName);	// JPEG�t�@�C����Ǎ���
	bool GetJpegData(u_char*& pRed, u_char*& pGreen, u_char*& pBlue, int & iHSize, int & iVSize);	// JPEG�f�[�^�̎��o��
	void CreateDIB(void);					// �����ς݂�JPEG�f�[�^����DIB���쐬

};


