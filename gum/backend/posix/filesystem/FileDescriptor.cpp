#include <gum/backend/posix/filesystem/FileDescriptor.h>

#include <gum/sys/SystemException.h>
#include <gum/token/FunctionToken.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace gum {
namespace posix {

namespace {

int map_mode(FileMode mode) {
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
}

FileDescriptor::FileDescriptor(const String& path, const FileOpenFlags& flags)
    : _fd() {
    int internal_flags = map_mode(flags.get_mode());
    internal_flags |= O_CLOEXEC;
    if (flags.get_create())
        internal_flags |= O_CREAT;
    if (flags.get_truncate())
        internal_flags |= O_TRUNC;

    _fd = open(path.c_str(), internal_flags);
    GUM_CHECK(_fd >= 0, SystemException(String() << "open('" << path << "', " << flags << ") failed"));

    _closeToken = make_token<FunctionToken>([fd = _fd]() { close(fd); });
}
}
}
