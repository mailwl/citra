// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <cstring>
#include <memory>
#include "common/common_types.h"
#include "common/logging/log.h"
#include "core/file_sys/ivfc_archive.h"
#include "ctr.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// FileSys namespace

namespace FileSys {

std::string IVFCArchive::GetName() const {
    return "IVFC";
}

ResultVal<std::unique_ptr<FileBackend>> IVFCArchive::OpenFile(const Path& path,
                                                              const Mode& mode) const {
    return MakeResult<std::unique_ptr<FileBackend>>(
        std::make_unique<IVFCFile>(romfs_file, data_offset, data_size));
}

ResultCode IVFCArchive::DeleteFile(const Path& path) const {
    LOG_CRITICAL(Service_FS, "Attempted to delete a file from an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(Subv): Verify error code
    return ResultCode(ErrorDescription::NoData, ErrorModule::FS, ErrorSummary::Canceled,
                      ErrorLevel::Status);
}

ResultCode IVFCArchive::RenameFile(const Path& src_path, const Path& dest_path) const {
    LOG_CRITICAL(Service_FS, "Attempted to rename a file within an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(wwylele): Use correct error code
    return ResultCode(-1);
}

ResultCode IVFCArchive::DeleteDirectory(const Path& path) const {
    LOG_CRITICAL(Service_FS, "Attempted to delete a directory from an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(wwylele): Use correct error code
    return ResultCode(-1);
}

ResultCode IVFCArchive::DeleteDirectoryRecursively(const Path& path) const {
    LOG_CRITICAL(Service_FS, "Attempted to delete a directory from an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(wwylele): Use correct error code
    return ResultCode(-1);
}

ResultCode IVFCArchive::CreateFile(const Path& path, u64 size) const {
    LOG_CRITICAL(Service_FS, "Attempted to create a file in an IVFC archive (%s).",
                 GetName().c_str());
    // TODO: Verify error code
    return ResultCode(ErrorDescription::NotAuthorized, ErrorModule::FS, ErrorSummary::NotSupported,
                      ErrorLevel::Permanent);
}

ResultCode IVFCArchive::CreateDirectory(const Path& path) const {
    LOG_CRITICAL(Service_FS, "Attempted to create a directory in an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(wwylele): Use correct error code
    return ResultCode(-1);
}

ResultCode IVFCArchive::RenameDirectory(const Path& src_path, const Path& dest_path) const {
    LOG_CRITICAL(Service_FS, "Attempted to rename a file within an IVFC archive (%s).",
                 GetName().c_str());
    // TODO(wwylele): Use correct error code
    return ResultCode(-1);
}

ResultVal<std::unique_ptr<DirectoryBackend>> IVFCArchive::OpenDirectory(const Path& path) const {
    return MakeResult<std::unique_ptr<DirectoryBackend>>(std::make_unique<IVFCDirectory>());
}

u64 IVFCArchive::GetFreeBytes() const {
    LOG_WARNING(Service_FS, "Attempted to get the free space in an IVFC archive");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ResultVal<size_t> IVFCFile::Read(const u64 offset, const size_t length, u8* buffer) const {
    LOG_TRACE(Service_FS, "called offset=0x%X, length=0x%X", offset, length);

    if (romfs_file == nullptr) {
        LOG_ERROR(Service_FS, "called with no RomFS, offset=0x%X, length=0x%X", offset, length);
        return ResultVal<size_t>();
    }

    u32 pad = offset & 0xF;
    u64 read_offset = offset & ~0xF;
    size_t read_length = (size_t)std::min((u64)(length+pad), data_size - read_offset);
    romfs_file->Seek(data_offset + read_offset, SEEK_SET);

    std::vector<u8> tmp_vector(read_length);

    size_t readed = romfs_file->ReadBytes(tmp_vector.data(), read_length);

    if (romfs_file->Encrypted()) {
        u8 key[16]{};
        u8 counter[16]{};
        ctr_aes_context aes{};
        romfs_file->GetCounter(counter);
        ctr_init_counter(&aes, key, counter);
        ctr_add_counter(&aes, (read_offset+0x1000) / 0x10);
        ctr_crypt_counter(&aes, tmp_vector.data(), tmp_vector.data(), readed);
    }

    std::memcpy(buffer, tmp_vector.data() + pad, readed - pad);
    return MakeResult<size_t>(readed - pad);
}


ResultVal<size_t> IVFCFile::Write(const u64 offset, const size_t length, const bool flush,
                                  const u8* buffer) const {
    LOG_ERROR(Service_FS, "Attempted to write to IVFC file");
    // TODO(Subv): Find error code
    return MakeResult<size_t>(0);
}

u64 IVFCFile::GetSize() const {
    return data_size;
}

bool IVFCFile::SetSize(const u64 size) const {
    LOG_ERROR(Service_FS, "Attempted to set the size of an IVFC file");
    return false;
}

} // namespace FileSys
