#pragma once

struct FTextureUpdateData
{
	class FDynamicTexture* Context;
	rs2_frame* RsFrame;
	void* Data;
	uint32 DataSize;
	uint32 Pitch;
	uint32 Width;
	uint32 Height;
};

class FDynamicTexture
{
private:

	FString TexName;
	UTexture2D* TextureObject = nullptr;

	FCriticalSection TudMx;
	std::vector<FTextureUpdateData*> TudPool;

	int CurWidth = 0;
	int CurHeight = 0;
	EPixelFormat CurFormat = PF_Unknown;

private:

	void RenderCmd_CreateTexture();
	void RenderCmd_UpdateTexture(FTextureUpdateData* Tud);

public:

	FDynamicTexture(FString Name);
	virtual ~FDynamicTexture();

	void Release();
	void Resize(int Width, int Height, EPixelFormat Format);
	bool UpdateFrame(rs2::video_frame Frame);

	inline UTexture2D* GetTextureObject() { return TextureObject; }
};
