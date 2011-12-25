/******************************************************
CInBitStream Ver.1.0.0 �r�b�g�X�g���[���E�N���X
	Copyright 2004 AsWe.Co.,Ltd. All rights reserved.
# The comments are written in Japanese (Shift-JIS).
******************************************************/

// �r�b�g���o���̂��߂̃}�X�N
static const int	kBitTestMaskT[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

class CInBitStream
{
public:
	// �R���X�g���N�^
			CInBitStream( 
				char* 	aBufP,				// �o�b�t�@
				int		size );				// �o�b�t�@�̃T�C�Y

	// �r�b�g�P�ʂœǂݏo��
	int		GetBits( 						// �Ԓl�F�ǂݏo���f�[�^
				int 	numOfBits );		// �ǂݏo���r�b�g��

	// 1�o�C�g�ǂݏo��
	u_char	GetByte( void );				// �Ԓl�F�ǂݏo��������

	// 2�o�C�g�ǂݏo��
	u_int	GetWord( void );				// �Ԓl�F�ǂݏo��������

	// n�o�C�g�ǂݏo��
	void	CopyByte(
				char*	disP,				// �i�[��A�h���X
				int		n );				// �ǂݏo���o�C�g��

	// n�o�C�g�i�߂�
	void	SkipByte(
				int		n );				// �ǂݐi�߂�o�C�g��

	// ���̓ǂݏo���A�h���X
	char*	GetNextAddress( void );			// �Ԓl�F���̓ǂݏo���A�h���X

protected:
	u_char*	mBufP;							// �ǂݏo���A�h���X
	u_char*	mEndOfBufP;						// �o�b�t�@�̏I���A�h���X
	int		mBitPos;						// �ǂݏo���r�b�g�ʒu�i��ʃr�b�g��7�A���ʃr�b�g��0�j
	int		mNextFlag;						// ���̃o�C�g��ǂ�ł������ǂ���
	int		mReadFlag;						// 1:�ǂݏo����, 0:�ǂݏo���s��

	// �ǂݏo���A�h���X�̃C���N�������g�ƃA�N�Z�X�ᔽ�̃`�F�b�N
	void	IncBuf( void );
};
