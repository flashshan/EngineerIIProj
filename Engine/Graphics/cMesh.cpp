#include "cMesh.h"
			  
#include <Engine//Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <new>


//#include <Engine/Time/Time.h>
//#include <fstream> 

eae6320::Assets::cManager<eae6320::Graphics::cMesh> eae6320::Graphics::cMesh::s_manager;



// not being used now
eae6320::cResult eae6320::Graphics::cMesh::MakeMesh(cMesh*& o_mesh, const char* i_meshFilePath)
{
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::Graphics::cMesh::Load(const char* const i_meshFilePath, cMesh*& o_mesh)
{
	eae6320::cResult result = eae6320::Results::Success;
	eae6320::Graphics::cMesh* newMesh = new (std::nothrow) eae6320::Graphics::cMesh();
	
	Platform::sDataFromFile binDataFromFile;
	std::string errorMessage;

	// allocate
	if (!newMesh)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for the sprite");
		Logging::OutputError("Failed to allocate memory for the sprite");
		goto OnExit;
	}

	if (!(result = Platform::LoadBinaryFile(i_meshFilePath, binDataFromFile, &errorMessage)))
	{
		EAE6320_ASSERTF(false, "Couldn't load binary file");
		goto OnExit;
	}

	uintptr_t currentOffset = reinterpret_cast<uintptr_t>(binDataFromFile.data);
	unsigned int vertexCount = static_cast<unsigned int>(*reinterpret_cast<uint16_t*>(currentOffset));
	
	currentOffset += sizeof(uint16_t);
	VertexFormats::sGeometry *vertexData = reinterpret_cast<VertexFormats::sGeometry *>(currentOffset);
	
	currentOffset += sizeof(VertexFormats::sGeometry) * vertexCount;
	unsigned int indexCount = static_cast<unsigned int>(*reinterpret_cast<uint16_t*>(currentOffset));
	
	currentOffset += sizeof(uint16_t);
	uint16_t *indexData = reinterpret_cast<uint16_t *>(currentOffset);

	if (!(result = newMesh->Initialize(vertexData, vertexCount, indexData, indexCount)))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		goto OnExit;
	}
	binDataFromFile.Free();

OnExit:
	if (result)
	{
		EAE6320_ASSERT(newMesh);
		o_mesh = newMesh;
	}
	else
	{
		if (newMesh)
		{
			newMesh->DecrementReferenceCount();
			newMesh = nullptr;
		}
		o_mesh = nullptr;
	}
	return result;
}


eae6320::Graphics::cMesh::~cMesh()     // do clean up
{
	this->CleanUp();
};