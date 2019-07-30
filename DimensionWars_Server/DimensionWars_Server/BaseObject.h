#pragma once
class BaseObject
{
public:
	BaseObject();
	~BaseObject();

private:
	XMFLOAT3 position;
	BoundingOrientedBox colbox;
};

