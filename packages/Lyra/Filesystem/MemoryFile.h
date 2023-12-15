#pragma once
#include <Filesystem/IFile.h>
#include <Core/CoreDefines.h>
#include <Core/Types.h>
// Created on 2019-01-10 by fmacias

namespace lyra
{

class MemoryFile : public IFile
{
public:
	explicit MemoryFile(Vector<char>& _buffer) : mBuffer(_buffer), mCursor(0), mIsOpen(false), mOpenMode(FileConstants::Read)
	{
	};
	DeclareNonCopyable(MemoryFile);
	DeclareDefaultMoveable(MemoryFile);


	void Open(FileConstants::OpenMode openMode) override;
	bool IsOpen() override;
	void Close() override;
	unsigned long Read(void* buffer, unsigned long size, unsigned long elementSize) override;
	unsigned long Write(const void* buffer, unsigned long size, unsigned long elementSize) override;
	unsigned long GetPosition() override { return mCursor; };
	void SetPosition(unsigned long position, FileConstants::PositionOrigin origin) override;

private:
	Vector<char>& mBuffer;
	unsigned long mCursor;
	bool mIsOpen;
	FileConstants::OpenMode mOpenMode;
};

}

