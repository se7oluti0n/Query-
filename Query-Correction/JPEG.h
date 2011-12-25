/******************************************************
JPEG.h Ver.1.1.0 JPEG�֘A�萔
	Copyright 2004 AsWe.Co.Ltd. All right reseved.
The comments are written in Japanese (Shift-JIS).
******************************************************/

#pragma once

typedef unsigned char	u_char;
typedef unsigned int	u_int;

// �v�Z�A���ʗp�萔
const char		kJFIF[] = "JFIF";				// JFIF���ʕ�����
const char		kJFXX[] = "JFXX";				// JFXX���ʕ�����
const double	kSqrt2 = 1.41421356;			// 2�̕�����
const double	kDisSqrt2 = 1.0 / kSqrt2;		// 2�̕������̋t��
const double	kPaiDiv16 = 3.14159265 / 16;	// �~����/16

/************************/
/* �W�O�U�O�V�[�P���X�p */
/************************/

static const int	kZigzagT[64] = {
	 0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

// ��������
enum
eProcessResult {
	emNoError			= 0,		// �G���[����
	emRuntimeError		= -1,		// ���s�G���[
	emDataError			= -2		// �f�[�^�G���[
};

// �}�[�J�R�[�h
enum
eMarker {
// �t���[���J�n
	// �񍷕��n�t�}������
	emSOF0		= 0xc0,		// ��{DCT����
	emSOF1		= 0xc1,		// �g���V�[�P���V����DCT����
	emSOF2		= 0xc2,		// �v���O���b�V�uDCT����
	emSOF3		= 0xc3,		// ��ԉt����
	// �����n�t�}������
	emSOF5		= 0xc5,		// �����V�[�P���V����DCT����
	emSOF6		= 0xc6,		// �����v���O���b�V�uDCT����
	emSOF7		= 0xc7,		// ������ԕ���
	// �񍷕��Z�p����
	emJPG		= 0xc8,		// JPEG�g���̂��߂̃��U�[�u����
	emSOF9		= 0xc9,		// �g���V�[�P���V����DCT����
	emSOF10		= 0xca,		// �v���O���b�V�uDCT����
	emSOF11		= 0xcb,		// ��ԁi�V�[�P���V�����j�t����
	// �����Z�p����
	emSOF13		= 0xcd,		// �����V�[�P���V����DCT����
	emSOF14		= 0xce,		// �����v���O���b�V�uDCT����
	emSOF15		= 0xcf,		// ������ԕ���

// �n�t�}���e�[�u���w��
	emDHT		= 0xc4,		// �n�t�}���e�[�u����`

// �Z�p���������t���w��
	emDAC		= 0xcc,		// �Z�p�����������t����`

// ���X�^�[�g�C���^�o���I��	���W����8��0��҂��X�^�[�g
	emRST0		= 0xd0,		emRST1		= 0xd1,
	emRST2		= 0xd2,		emRST3		= 0xd3,
	emRST4		= 0xd4,		emRST5		= 0xd5,
	emRST6		= 0xd6,		emRST7		= 0xd7,

// ���̑�
	emSOI		= 0xd8,		// �摜�J�n
	emEOI		= 0xd9,		// �摜�I��
	emSOS		= 0xda,		// �X�L�����J�n
	emDQT		= 0xdb,		// �ʎq���e�[�u����`
	emDNL		= 0xdc,		// ���C������`
	emDRI		= 0xdd,		// ���X�^�[�g�C���^�o����`
	emDHP		= 0xde,		// �n�C�A���[�L�J���E�v���O���b�V������`
	emEXP		= 0xdf,		// �Q�Ɛ����g��
	emCOM		= 0xfe,		// �R�����g

// �A�v���P�[�V�����Z�O�����g
	emAPP0		= 0xe0,		emAPP1		= 0xe1,
	emAPP2		= 0xe2,		emAPP3		= 0xe3,
	emAPP4		= 0xe4,		emAPP5		= 0xe5,
	emAPP6		= 0xe6,		emAPP7		= 0xe7,
	emAPP8		= 0xe8,		emAPP9		= 0xe9,
	emAPP10		= 0xea,		emAPP11		= 0xeb,
	emAPP12		= 0xec,		emAPP13		= 0xed,
	emAPP14		= 0xee,		emAPP15		= 0xef,

// JPEG�g���̂��߂̃��U�[�u
	emJPG0		= 0xf0,		emJPG1		= 0xf1,
	emJPG2		= 0xf2,		emJPG3		= 0xf3,
	emJPG4		= 0xf4,		emJPG5		= 0xf5,
	emJPG6		= 0xf6,		emJPG7		= 0xf7,
	emJPG8		= 0xf8,		emJPG9		= 0xf9,
	emJPG10		= 0xfa,		emJPG11		= 0xfb,
	emJPG12		= 0xfc,		emJPG13		= 0xfd,

// ���U�[�u���ꂽ�}�[�J
	emTEM		= 0x01,		// �Z�p�����Ŏg�p
	emRESst		= 0x02,
	emRESend	= 0xbf,

// �G���[�����p��K��}�[�J
	emError		= 0xff,		// �}�[�J���o���G���[�����i���o�ł��Ȃ������j
	emMarker	= 0xff		// �}�[�J
};

// JPEG�摜�v���p�e�B
struct
SJpegProperty {
	int			CanDecode;			// �����\���i���������̂݁j
	int			HSize;				// ���摜����f��
	int			VSize;				// ���摜�c��f��
	int			Dimension;			// ������
	int			SamplePrecision;	// �T���v�����x
	char		*CommentP;			// �R�����g
	int			Format;				// �t�H�[�}�b�g��� 0:�s���A1:JFIF�A2:JFXX 
	u_char		MajorRevisions;		// �o�[�W���������_���
	u_char		MinorRevisions;		// �o�[�W���������_����
	int			Units;				// ���x�P�� 0:�����A1:dots/inch�A2:dots/cm
	int			HDensity;			// �����x
	int			VDensity;			// �c���x
	int			HThumbnail;			// �T���l�C������f��
	int			VThumbnail;			// �T���l�C���c��f��
	int			ExtensionCode;		// �g���R�[�h 0x10:JPEG, 0x11:1byte/pixel, 0x13:3bytes/pixel
};

/********************/
/* �G���[�ʒm�N���X */
/********************/

// �������G���[��O�ʒm�p�N���X
class EMemoryError
{
public:
			EMemoryError(
				int		size,		// �v���T�C�Y
				char*	messageP )	// �G���[���b�Z�[�W
				{	mSize=size;	mMessageP=messageP;	};
	
	int			mSize;				// �v���T�C�Y
	char*		mMessageP;			// �G���[���b�Z�[�W
};

// �e�[�u���O�Q�ƃG���[��O�ʒm�p�N���X
class EIndexError
{
public:
			EIndexError(
				int		maxIndex,	// ���e�C���f�b�N�X
				int		errorIndex, // �G���[�C���f�b�N�X
				char*	messageP )		// �G���[���b�Z�[�W
				{	mMaxIndex=maxIndex;	mErrorIndex=errorIndex;	mMessageP = messageP;	};
	
	int			mMaxIndex;			// ���e�C���f�b�N�X
	int			mErrorIndex;		// �G���[�C���f�b�N�X
	char*		mMessageP;			// �G���[���b�Z�[�W
};

// �o�b�t�@�I���G���[��O�ʒm�p�N���X
class	EBufferError
{
public:
			EBufferError(
				char*	messageP )		// �G���[���b�Z�[�W
				{	mMessageP = messageP;	};
	char*		mMessageP;			// �G���[���b�Z�[�W
};

// JPEG�f�[�^�t�H�[�}�b�g�G���[��O�ʒm�p�N���X
class EDataFormatError
{
public:
			EDataFormatError(
				void*	addressP,	// �G���[�����A�h���X
				char*	messageP )	// �G���[���b�Z�[�W
				{	mAddressP=addressP;	mMessageP=messageP;	};

	void*		mAddressP;			// �G���[�����A�h���X
	char*		mMessageP;			// �G���[���b�Z�[�W
};

// ��Ή��f�[�^�G���[�ʒm�p�N���X
class ENotSupported
{
public:
			ENotSupported(
				eMarker	aMark,		// �t���[���R�[�h
				void*	addressP )	// �G���[�����A�h���X
				{ mMark = aMark; mAddressP=addressP; };

	eMarker		mMark;					// �t���[���R�[�h
	void*		mAddressP;				// �G���[�����A�h���X
};
