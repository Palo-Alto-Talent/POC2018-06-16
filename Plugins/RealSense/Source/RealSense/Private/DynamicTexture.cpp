#include "PCH.h"

FDynamicTexture::FDynamicTexture(FString Name) : TexName(Name)
{
	REALSENSE_TRACE(TEXT("+FDynamicTexture_%X"), this);
}

FDynamicTexture::~FDynamicTexture()
{
	REALSENSE_TRACE(TEXT("~FDynamicTexture_%X"), this);

	Release();
}

void FDynamicTexture::Release()
{
	if (TextureObject)
	{
		REALSENSE_TRACE(TEXT("FDynamicTexture_%X::Release %s (TudCount=%d)"), this, *TexName, TudPool.size());
		TextureObject->RemoveFromRoot();
		TextureObject = nullptr;
	}

	for (FTextureUpdateData* Tud : TudPool)
	{
		if (Tud->Data) FMemory::Free(Tud->Data);
		delete Tud;
	}
	TudPool.clear();

	CurWidth = CurHeight = 0;
	CurFormat = PF_Unknown;
}

void FDynamicTexture::Resize(int Width, int Height, EPixelFormat Format)
{
	SCOPED_PROFILER;

	REALSENSE_TRACE(TEXT("FDynamicTexture_%X::Resize %s Width=%d Height=%d Format=%d"), 
		this, *TexName, Width, Height, (int)Format);

	Release();

	CurWidth = Width;
	CurHeight = Height;
	CurFormat = Format;

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		CreateTextureCmd,
		FDynamicTexture*, Context, this,
	{
		Context->RenderCmd_CreateTexture();
	});
}

void FDynamicTexture::RenderCmd_CreateTexture()
{
	SCOPED_PROFILER;

	auto Tex = UTexture2D::CreateTransient(CurWidth, CurHeight, CurFormat);
	if (!Tex)
	{
		REALSENSE_ERR(TEXT("UTexture2D::CreateTransient failed"));
		return;
	}

	#if WITH_EDITORONLY_DATA
		Tex->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	#endif

	Tex->SRGB = 1;
	Tex->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	Tex->Filter = TextureFilter::TF_Nearest;

	Tex->AddToRoot();
	Tex->UpdateResource();

	TextureObject = Tex;
}

bool FDynamicTexture::UpdateFrame(rs2::video_frame Frame)
{
	SCOPED_PROFILER;

	const void* Data = (const void*)Frame.get_data();
	const int Pitch = Frame.get_stride_in_bytes();
	const int Width = Frame.get_width();
	const int Height = Frame.get_height();

	if (!Data || Width != CurWidth || Height != CurHeight)
	{
		REALSENSE_ERR(TEXT("Invalid video_frame %s"), *TexName);
		return false;
	}

	if (TextureObject && TextureObject->Resource)
	{
		FTextureUpdateData* Tud;
		{
			FScopeLock Lock(&TudMx);
			if (!TudPool.empty()) { Tud = TudPool.back(); TudPool.pop_back(); }
			else { Tud = new FTextureUpdateData(); FMemory::Memset(Tud, 0, sizeof(FTextureUpdateData)); }
		}

		Tud->Context = this;
		Tud->RsFrame = Frame.get();
		Tud->Pitch = Pitch;
		Tud->Width = Width;
		Tud->Height = Height;

		//Tud->Data = Data;
		//rs2_error* e = nullptr;
		//rs2_frame_add_ref(Frame.get(), &e);

		if (!Tud->Data)
		{
			Tud->DataSize = Pitch * Height;
			Tud->Data = FMemory::Malloc(Tud->DataSize, PLATFORM_CACHE_LINE_SIZE);
		}
		FMemory::Memcpy(Tud->Data, Data, Tud->DataSize); // TODO: don't copy if possible

		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			UpdateTextureCmd,
			FTextureUpdateData*, Tud, Tud,
		{
			Tud->Context->RenderCmd_UpdateTexture(Tud);
		});
	}

	return true;
}

void FDynamicTexture::RenderCmd_UpdateTexture(FTextureUpdateData* Tud)
{
	SCOPED_PROFILER;

	auto Tex = Tud->Context->TextureObject;
	if (Tex && Tex->Resource)
	{
		RHIUpdateTexture2D(
			((FTexture2DResource*)Tex->Resource)->GetTexture2DRHI(), 
			0, 
			FUpdateTextureRegion2D(0, 0, 0, 0, Tud->Width, Tud->Height), 
			Tud->Pitch, 
			(const uint8*)Tud->Data
		);

		//rs2_release_frame(Tud->RsFrame);

		{
			FScopeLock Lock(&TudMx);
			TudPool.push_back(Tud);
		}
	}
}
