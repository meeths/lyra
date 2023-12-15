#include <Filesystem/File.h>

namespace lyra
{
File::~File()
{
	if(File::IsOpen())
	{
		File::Close();
	}
}

void File::Open(FileConstants::OpenMode openMode)
{
	int stdBitmask = 0;
	if(openMode & FileConstants::OpenMode::Read) stdBitmask |= std::ios::in;
	if(openMode & FileConstants::OpenMode::Write) stdBitmask |= std::ios::out;
	if(openMode & FileConstants::OpenMode::Binary) stdBitmask |= std::ios::binary;
	if(openMode & FileConstants::OpenMode::AtEnd) stdBitmask |= std::ios::ate;
	if(openMode & FileConstants::OpenMode::Truncate) stdBitmask |= std::ios::trunc;

	mStream.open(mFileName, stdBitmask);
}

bool File::IsOpen()
{
	return mStream.is_open();
}

void File::Close()
{
	mStream.close();
}

unsigned long File::Read(void* buffer, unsigned long size, unsigned long elementSize)
{
	if(!IsOpen())
		throw::std::runtime_error("Reading closed file");
	mStream.read(static_cast<char*>(buffer), size * elementSize);
	if(mStream)
		return size * elementSize;
	// Could not read the requested number of bytes
	return static_cast<unsigned long>(mStream.gcount());
}

unsigned long File::Write(const void* buffer, unsigned long size, unsigned long elementSize)
{
	if(!IsOpen())
		throw::std::runtime_error("Writing closed file");

	mStream.write(static_cast<const char*>(buffer), size * elementSize);
	return size * elementSize;
}

	unsigned long File::GetPosition()
{
	if(!IsOpen())
		throw::std::runtime_error("Reading closed file");
	return static_cast<unsigned long>(mStream.tellg());
}

void File::SetPosition(unsigned long position, FileConstants::PositionOrigin origin)
{
	std::ios::seekdir seekDir;
	switch(origin) 
	{
		case FileConstants::PositionOrigin::End: 
			seekDir = std::ios::end; break;
		case FileConstants::PositionOrigin::Current: 
			seekDir = std::ios::cur; break;
		case FileConstants::PositionOrigin::Start:
		default:  
			seekDir = std::ios::beg; break;
	}

	mStream.seekg(position, seekDir);
}
}
