// Include Files
//==============

#include "../cTextureBuilder.h"

#include <algorithm>
#include <codecvt>
#include <Engine/Graphics/TextureFormats.h>
#include <Engine/Math/Functions.h>
#include <External/DirectXTex/Includes.h>
#include <fstream>
#include <locale>
#include <string>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <utility>

// Helper Function Declarations
//=============================

namespace
{
	eae6320::cResult BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture );
	constexpr eae6320::Graphics::TextureFormats::Compression::eType GetCompressionType( const DXGI_FORMAT i_dxgiFormat );
	eae6320::cResult LoadSourceImage( const char *const i_path, DirectX::ScratchImage &o_image );
	eae6320::cResult WriteTextureToFile( const char* const i_path_target, const DirectX::ScratchImage &i_texture );
}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cTextureBuilder::Build( const std::vector<std::string>& )
{
	auto result = eae6320::Results::Success;

	DirectX::ScratchImage sourceImage;
	DirectX::ScratchImage builtTexture;
	auto shouldComBeUninitialized = false;

	// Initialize COM
	{
		void* const thisMustBeNull = nullptr;
		if ( SUCCEEDED( CoInitialize( thisMustBeNull ) ) )
		{
			shouldComBeUninitialized = true;
		}
		else
		{
			result = Results::Failure;
			Assets::OutputErrorMessageWithFileInfo( m_path_source,
				"DirectXTex couldn't be used because COM couldn't be initiazed" );
			goto OnExit;
		}
	}
	// Load the source image
	if ( !( result = LoadSourceImage( m_path_source, sourceImage ) ) )
	{
		goto OnExit;
	}
	// Build the texture
	if ( !( result = BuildTexture( m_path_source, sourceImage, builtTexture ) ) )
	{
		goto OnExit;
	}
	// Write the texture to a file
	if ( !( result = WriteTextureToFile( m_path_target, builtTexture ) ) )
	{
		goto OnExit;
	}

OnExit:
	
	if ( shouldComBeUninitialized )
	{
		CoUninitialize();
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture )
	{
		// DirectX can only do image processing on uncompressed images
		DirectX::ScratchImage uncompressedImage;
		if ( DirectX::IsCompressed( io_sourceImageThatMayNotBeValidAfterThisCall.GetMetadata().format ) )
		{
			// The uncompressed format is chosen naively and assumes "standard" textures
			// (it will lose precision on any source images that use more than 8 bits per channel
			// and lose information on any that aren't normalized [0,1])
			constexpr auto formatToDecompressTo = DXGI_FORMAT_R8G8B8A8_UNORM;
			const auto result = DirectX::Decompress( io_sourceImageThatMayNotBeValidAfterThisCall.GetImages(), io_sourceImageThatMayNotBeValidAfterThisCall.GetImageCount(),
				io_sourceImageThatMayNotBeValidAfterThisCall.GetMetadata(), formatToDecompressTo, uncompressedImage );
			if ( FAILED( result ) )
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex failed to uncompress source image" );
				return eae6320::Results::Failure;
			}
		}
		else
		{
			uncompressedImage = std::move( io_sourceImageThatMayNotBeValidAfterThisCall );
		}
		// Standard DDS images are upside-down from what OpenGL expects
		DirectX::ScratchImage flippedImage;
#if defined ( EAE6320_PLATFORM_GL )
		{
			constexpr auto flipVertically = static_cast<DWORD>( DirectX::TEX_FR_FLIP_VERTICAL );
			const auto result = DirectX::FlipRotate( uncompressedImage.GetImages(), uncompressedImage.GetImageCount(),
				uncompressedImage.GetMetadata(), flipVertically, flippedImage );
			if ( FAILED( result ) )
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex failed to flip the source image vertically" );
				return eae6320::Results::Failure;
			}
		}
#else
		flippedImage = std::move( uncompressedImage );
#endif
		// Textures used by the GPU have size restrictions that standard images don't
		DirectX::ScratchImage resizedImage;
		{
			const auto flippedMetadata = flippedImage.GetMetadata();
			auto targetWidth = flippedMetadata.width;
			auto targetHeight = flippedMetadata.height;
			{
				// Direct3D will only load BC compressed textures whose dimensions are multiples of 4
				// ("BC" stands for "block compression", and each block is 4x4)
				{
					// Round up to the nearest multiple of 4
					constexpr size_t blockSize = 4;
					targetWidth = eae6320::Math::RoundUpToMultiple_powerOf2( targetWidth, blockSize );
					targetHeight = eae6320::Math::RoundUpToMultiple_powerOf2( targetHeight, blockSize );
				}
				// Direct3D can't support textures over a certain size
				{
					if ( !resizedImage.GetMetadata().IsVolumemap() )
					{
						if ( !resizedImage.GetMetadata().IsCubemap() )
						{
							targetWidth = std::min<size_t>( targetWidth, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION );
							targetHeight = std::min<size_t>( targetHeight, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION );
						}
						else
						{
							targetWidth = std::min<size_t>( targetWidth, D3D11_REQ_TEXTURECUBE_DIMENSION );
							targetHeight = std::min<size_t>( targetHeight, D3D11_REQ_TEXTURECUBE_DIMENSION );
						}
					}
					else
					{
						targetWidth = std::min<size_t>( targetWidth, D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
						targetHeight = std::min<size_t>( targetHeight, D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION );
					}
				}
			}
			if ( ( targetWidth != flippedMetadata.width ) || ( targetHeight != flippedMetadata.height ) )
			{
				constexpr auto useDefaultFiltering = static_cast<DWORD>( DirectX::TEX_FILTER_DEFAULT );
				const auto result = DirectX::Resize( flippedImage.GetImages(), flippedImage.GetImageCount(),
					flippedMetadata, targetWidth, targetHeight, useDefaultFiltering, resizedImage );
			}
			else
			{
				resizedImage = std::move( flippedImage );
			}
		}
		// Generate MIP maps
		// (Note that this will overwrite any existing MIP maps)
		DirectX::ScratchImage imageWithMipMaps;
		{
			constexpr DWORD useDefaultFiltering = DirectX::TEX_FILTER_DEFAULT;
			constexpr size_t generateAllPossibleLevels = 0;
			HRESULT result;
			if ( !resizedImage.GetMetadata().IsVolumemap() )
			{
				result = DirectX::GenerateMipMaps( resizedImage.GetImages(), resizedImage.GetImageCount(),
					resizedImage.GetMetadata(), useDefaultFiltering, generateAllPossibleLevels, imageWithMipMaps );
			}
			else
			{
				result = DirectX::GenerateMipMaps3D( resizedImage.GetImages(), resizedImage.GetImageCount(),
					resizedImage.GetMetadata(), useDefaultFiltering, generateAllPossibleLevels, imageWithMipMaps );
			}
			if ( FAILED( result ) )
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex failed to generate MIP maps" );
				return eae6320::Results::Failure;
			}
		}
		// Compress the texture
		{
			// Our texture builder only supports two kinds of formats:
			//	* BC1 (compressed with no alpha, used to be known as "DXT1")
			//	* BC3 (compressed with alpha, used to be known as "DXT5")
			const auto formatToCompressTo = resizedImage.IsAlphaAllOpaque() ? DXGI_FORMAT_BC1_UNORM : DXGI_FORMAT_BC3_UNORM;
			constexpr DWORD useDefaultCompressionOptions = DirectX::TEX_COMPRESS_DEFAULT;
			const float useDefaultThreshold = DirectX::TEX_THRESHOLD_DEFAULT;
			const HRESULT result = DirectX::Compress( imageWithMipMaps.GetImages(), imageWithMipMaps.GetImageCount(),
				imageWithMipMaps.GetMetadata(), formatToCompressTo, useDefaultCompressionOptions, useDefaultThreshold, o_texture );
			if ( FAILED( result ) )
			{
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex failed to compress the texture" );
				return eae6320::Results::Failure;
			}
		}

		return eae6320::Results::Success;
	}

	constexpr eae6320::Graphics::TextureFormats::Compression::eType GetCompressionType( const DXGI_FORMAT i_dxgiFormat )
	{
		switch ( i_dxgiFormat )
		{
			case DXGI_FORMAT_BC1_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC1;
			case DXGI_FORMAT_BC2_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC2;
			case DXGI_FORMAT_BC3_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC3;
			case DXGI_FORMAT_BC4_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC4;
			case DXGI_FORMAT_BC5_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC5;
			case DXGI_FORMAT_BC7_UNORM: return eae6320::Graphics::TextureFormats::Compression::BC7;
		}
		
		return eae6320::Graphics::TextureFormats::Compression::Unknown;
	}

	eae6320::cResult LoadSourceImage( const char *const i_path, DirectX::ScratchImage &o_image )
	{
		// DirectXTex uses wide strings
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> stringConverter;
		const std::wstring path( stringConverter.from_bytes( i_path ) );

		// Open the image based on its file extension
		// (An image's format can also often be deduced by looking at its actual bits
		// because the first of a file will be some kind of recognizable header,
		// but our TextureBuilder keeps things simple)
		HRESULT result = E_UNEXPECTED;
		{
			const std::wstring extension = path.substr( path.find_last_of( L'.' ) + 1 );
			DirectX::TexMetadata* const dontReturnMetadata = nullptr;
			if ( extension == L"dds" )
			{
				constexpr DWORD useDefaultBehavior = DirectX::DDS_FLAGS_NONE
					// Just in case you happen to use any old-style DDS files with luminance
					// this will expand the single luminance channel to all RGB channels
					// (which keeps it greyscale rather than using a red-only channel format)
					| DirectX::DDS_FLAGS_EXPAND_LUMINANCE
					;
				if ( FAILED( result = DirectX::LoadFromDDSFile( path.c_str(), useDefaultBehavior, dontReturnMetadata, o_image ) ) )
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex couldn't load the DDS file" );
				}
			}
			else if ( extension == L"tga" )
			{
				if ( FAILED( result = DirectX::LoadFromTGAFile( path.c_str(), dontReturnMetadata, o_image ) ) )
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "DirectXTex couldn't load the TGA file" );
				}
			}
			else
			{
				// Try to Windows Imaging Component and hope it supports the image type
				constexpr DWORD useDefaultBehavior = DirectX::WIC_FLAGS_NONE
					// If an image has an embedded sRGB profile ignore it
					// since our renderer isn't gamma-correct
					// (we want all textures in the shaders to have the same values they do as source images)
					| DirectX::WIC_FLAGS_IGNORE_SRGB
					;
				if ( FAILED( result = DirectX::LoadFromWICFile( path.c_str(), useDefaultBehavior, dontReturnMetadata, o_image ) ) )
				{
					eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "WIC couldn't load the source image" );
				}
			}
		}

		return SUCCEEDED( result ) ? eae6320::Results::Success : eae6320::Results::Failure;
	}

	eae6320::cResult WriteTextureToFile( const char* const i_path_target, const DirectX::ScratchImage &i_texture )
	{
		auto result = eae6320::Results::Success;

		// Open the file
		std::ofstream fout( i_path_target, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary );
		if ( !fout.is_open() )
		{
			result = eae6320::Results::Failure;
			eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target, "Target texture file couldn't be opened for writing" );
			goto OnExit;
		}

		// Write the texture information
		eae6320::Graphics::TextureFormats::sTextureInfo textureInfo;
		{
			auto &metadata = i_texture.GetMetadata();
			if ( metadata.width < ( 1u << ( sizeof( textureInfo.width ) * 8 ) ) )
			{
				textureInfo.width = static_cast<uint16_t>( metadata.width );
			}
			else
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"The width (%u) is too big for a sTextureInfo", metadata.width );
				goto OnExit;
			}
			if ( metadata.height < ( 1u << ( sizeof( textureInfo.height ) * 8 ) ) )
			{
				textureInfo.height = static_cast<uint16_t>( metadata.height );
			}
			else
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"The height (%u) is too big for a sTextureInfo", metadata.height );
				goto OnExit;
			}
			if ( metadata.mipLevels < ( 1u << ( sizeof( textureInfo.mipMapCount ) * 8 ) ) )
			{
				textureInfo.mipMapCount = static_cast<uint8_t>( metadata.mipLevels );
			}
			else
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"There are too many MIP levels (%u) for a sTextureInfo", metadata.mipLevels );
				goto OnExit;
			}
			textureInfo.compressionType = GetCompressionType( metadata.format );
			if ( textureInfo.compressionType == eae6320::Graphics::TextureFormats::Compression::Unknown )
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"The DXGI_Format (%i) isn't valid for a sTextureInfo", metadata.format );
				goto OnExit;
			}
		}
		{
			const auto byteCountToWrite = sizeof( textureInfo );
			fout.write( reinterpret_cast<const char*>( &textureInfo ), byteCountToWrite );
			if ( !fout.good() )
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"Failed to write %u bytes for the texture information", byteCountToWrite );
				goto OnExit;
			}
		}
		// Write the data for each MIP map
		{
			
			auto currentWidth = static_cast<uint_fast16_t>( textureInfo.width );
			auto currentHeight = static_cast<uint_fast16_t>( textureInfo.height );
			const auto blockSize = eae6320::Graphics::TextureFormats::Compression::GetSizeOfBlock( textureInfo.compressionType );
			const auto mipMapCount = static_cast<uint_fast8_t>( textureInfo.mipMapCount );
			if ( mipMapCount == i_texture.GetImageCount() )
			{
				const auto* const mipMaps = i_texture.GetImages();
				for ( uint_fast8_t i = 0; i < mipMapCount; ++i )
				{
					const auto& currentMipMap = mipMaps[i];
					// Calculate how much memory this MIP level uses
					const auto blockCount_singleRow = ( currentWidth + 3 ) / 4;
					const auto byteCount_singleRow = blockCount_singleRow * blockSize;
					if ( byteCount_singleRow != currentMipMap.rowPitch )
					{
						result = eae6320::Results::Failure;
						eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
							"Unexpected mismatch between calculated byte count for a single row of MIP map #%u (%u) and DirectXTex row pitch (%u)",
							i, byteCount_singleRow, currentMipMap.rowPitch );
						goto OnExit;
					}
					const auto rowCount = ( currentHeight + 3 ) / 4;
					const auto byteCount_currentMipLevel = byteCount_singleRow * rowCount;
					if ( byteCount_currentMipLevel != currentMipMap.slicePitch )
					{
						result = eae6320::Results::Failure;
						eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
							"Unexpected mismatch between calculated byte count for MIP map #%u (%u) and DirectXTex slice pitch (%u)",
							i, byteCount_currentMipLevel, currentMipMap.slicePitch );
						goto OnExit;
					}
					// Write this MIP map
					{
						fout.write( reinterpret_cast<const char*>( currentMipMap.pixels ), byteCount_currentMipLevel );
						if ( !fout.good() )
						{
							result = eae6320::Results::Failure;
							eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
								"Failed to write %u bytes for MIP map #%u", byteCount_currentMipLevel, i );
							goto OnExit;
						}
					}
					// Update dimensions for the next iteration
					{
						currentWidth = std::max( currentWidth / 2, 1u );
						currentHeight = std::max( currentHeight / 2, 1u );
					}
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"Unexpected mismatch between MIP map count (%u) and DirectXTex image count (%u)",
					mipMapCount, i_texture.GetImageCount() );
				goto OnExit;
			}
		}

	OnExit:

		if ( fout.is_open() )
		{
			fout.close();
			if ( fout.is_open() )
			{
				if ( result )
				{
					result = eae6320::Results::Failure;
				}
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path_target,
					"Failed to close the target texture file after writing" );
			}
		}

		return result;
	}
}
