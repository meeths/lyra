
#include <Threading/SRWLock.h>
#include <windows.h>
// Created on 2023-10-17 by sisco

namespace lyra
{
    SRWLock::SRWLock()
    {
        InitializeSRWLock(reinterpret_cast<PSRWLOCK>(&m_lock));
    }

    SRWLock::~SRWLock()
    {
    }

    void SRWLock::LockRead()
    {
        AcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
    }

    void SRWLock::UnlockRead()
    {
        ReleaseSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
    }

    void SRWLock::LockWrite()
    {
        AcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
    }

    void SRWLock::UnlockWrite()
    {
        ReleaseSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
    }
}

