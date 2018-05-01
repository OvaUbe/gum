#include <gum/backend/posix/filesystem/FileDescriptor.h>

#include <gum/sys/SystemException.h>
#include <gum/token/FunctionToken.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace gum {
namespace posix {

namespace {

int map_open_mode(FileMode mode) {
    switch (mode) {
    case FileMode::Read:
        return O_RDONLY;
    case FileMode::Write:
        return O_WRONLY;
    case FileMode::ReadWrite:
        return O_RDWR;
    default:
        GUM_THROW(NotImplementedException());
    }
}

int map_seek_mode(SeekMode mode) {
    switch (mode) {
    case SeekMode::Begin:
        return SEEK_SET;
    case SeekMode::Current:
        return SEEK_CUR;
    case SeekMode::End:
        return SEEK_END;
    default:
        GUM_THROW(NotImplementedException());
    }
}
}

FileDescriptor::FileDescriptor(const String& path, const FileOpenFlags& flags)
    : _fd() {
    int internal_flags = map_open_mode(flags.get_mode());
    internal_flags |= O_CLOEXEC;
    if (flags.get_create())
        internal_flags |= O_CREAT;
    if (flags.get_truncate())
        internal_flags |= O_TRUNC;

    _fd = open(path.c_str(), internal_flags);
    GUM_CHECK(_fd >= 0, SystemException(String() << "open('" << path << "', " << flags << ") failed"));

    _closeToken = make_token<FunctionToken>([fd = _fd]() { close(fd); });
}

void FileDescriptor::seek(s64 offset, SeekMode mode) {
    const off_t result = lseek(_fd, offset, map_seek_mode(mode));
    GUM_CHECK(result >= 0, SystemException(String() << "lseek(" << offset << ", " << mode << ") failed"));
}
}
}
