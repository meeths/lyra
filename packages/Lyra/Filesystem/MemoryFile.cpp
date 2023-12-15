
#include <Filesystem/MemoryFile.h>
#include <Math/Functions.h>
// Created on 2019-01-10 by fmacias

namespace lyra
{
	void MemoryFile::Open(FileConstants::OpenMode openMode)
	{
		mIsOpen = true;
		mOpenMode = openMode;
	}

	bool MemoryFile::IsOpen()
	{
		return mIsOpen;
	}

	void MemoryFile::Close()
	{
		mIsOpen = false;
	}

	unsigned long MemoryFile::Read(void * buffer, unsigned long size, unsigned long elementSize)
	{
		if(!(mOpenMode & (int)FileConstants::OpenMode::Read))
			return 0;

		auto readSize = Math::Min(size * elementSize, (unsigned long)mBuffer.size() - mCursor);

		if(readSize <= 0)
			return 0;

		std::memcpy(buffer, mBuffer.data(), readSize);
		mCursor += readSize;
		return readSize;
	}

	unsigned long MemoryFile::Write(const void * buffer, unsigned long size, unsigned long elementSize)
	{
		if((static_cast<int>(mOpenMode) & static_cast<int>(FileConstants::OpenMode::Write)) == 0 )
			return 0;

		if(mOpenMode & FileConstants::OpenMode::Append)
			mCursor = (unsigned long) mBuffer.size();

		auto writeSize = size * elementSize;
		if(writeSize + mCursor > mBuffer.size())
			mBuffer.resize(writeSize + mCursor);
		std::memcpy(mBuffer.data(), buffer, writeSize);
		mCursor = mCursor + writeSize;
		return writeSize;
	}

	void MemoryFile::SetPosition(unsigned long position, FileConstants::PositionOrigin origin)
	{
		switch(origin)
		{
		case FileConstants::PositionOrigin::Start:
			mCursor = position;
			break;
		case FileConstants::PositionOrigin::End:
			mCursor = (unsigned long) mBuffer.size() - position;
			break;
		case FileConstants::PositionOrigin::Current:
			mCursor = mCursor + position;
			break;
		}
	}
}

