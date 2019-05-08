#pragma once
class HeightMapImage
{
private:
	BYTE							*m_pHeightMapPixels;

	int								m_nWidth;
	int								m_nLength;
	XMFLOAT3						m_xmf3Scale;

public:
	HeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~HeightMapImage();

	float GetHeight(float x, float z, bool bReverseQuad = false);
	XMFLOAT3 GetPointToHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	unsigned char *GetHeightMapPixels() const { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() const { return(m_nWidth); }
	int GetHeightMapLength() const { return(m_nLength); }
};

