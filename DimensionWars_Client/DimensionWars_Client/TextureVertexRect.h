#pragma once
class TextureVertexRect
{
private:
	XMFLOAT3 m_xmf3Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2 m_xmf2Size = { 100.0f, 100.0f };
public:
	TextureVertexRect();
	TextureVertexRect(XMFLOAT3& xmf3Position, XMFLOAT2& xmf2Size);
	virtual ~TextureVertexRect();
};

