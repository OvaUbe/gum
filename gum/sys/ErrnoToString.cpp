/*
 * Copyright (c) Vladimir Golubev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <gum/sys/ErrnoToString.h>

#include <gum/string/ToString.h>

#include <string.h>

namespace gum {

namespace {

#define GUM_DETAIL_ERRNO_STR(val)                                                                                                                              \
    case val:                                                                                                                                                  \
        return #val
String errno_code_to_string(int err) {
    switch (err) {
        GUM_DETAIL_ERRNO_STR(EPERM);
        GUM_DETAIL_ERRNO_STR(ENOENT);
        GUM_DETAIL_ERRNO_STR(ESRCH);
        GUM_DETAIL_ERRNO_STR(EINTR);
        GUM_DETAIL_ERRNO_STR(EIO);
        GUM_DETAIL_ERRNO_STR(ENXIO);
        GUM_DETAIL_ERRNO_STR(E2BIG);
        GUM_DETAIL_ERRNO_STR(ENOEXEC);
        GUM_DETAIL_ERRNO_STR(EBADF);
        GUM_DETAIL_ERRNO_STR(ECHILD);
        GUM_DETAIL_ERRNO_STR(EAGAIN);
        GUM_DETAIL_ERRNO_STR(ENOMEM);
        GUM_DETAIL_ERRNO_STR(EACCES);
        GUM_DETAIL_ERRNO_STR(EFAULT);
        GUM_DETAIL_ERRNO_STR(ENOTBLK);
        GUM_DETAIL_ERRNO_STR(EBUSY);
        GUM_DETAIL_ERRNO_STR(EEXIST);
        GUM_DETAIL_ERRNO_STR(EXDEV);
        GUM_DETAIL_ERRNO_STR(ENODEV);
        GUM_DETAIL_ERRNO_STR(ENOTDIR);
        GUM_DETAIL_ERRNO_STR(EISDIR);
        GUM_DETAIL_ERRNO_STR(EINVAL);
        GUM_DETAIL_ERRNO_STR(ENFILE);
        GUM_DETAIL_ERRNO_STR(EMFILE);
        GUM_DETAIL_ERRNO_STR(ENOTTY);
        GUM_DETAIL_ERRNO_STR(ETXTBSY);
        GUM_DETAIL_ERRNO_STR(EFBIG);
        GUM_DETAIL_ERRNO_STR(ENOSPC);
        GUM_DETAIL_ERRNO_STR(ESPIPE);
        GUM_DETAIL_ERRNO_STR(EROFS);
        GUM_DETAIL_ERRNO_STR(EMLINK);
        GUM_DETAIL_ERRNO_STR(EPIPE);
        GUM_DETAIL_ERRNO_STR(EDOM);
        GUM_DETAIL_ERRNO_STR(ERANGE);

        GUM_DETAIL_ERRNO_STR(EDEADLK);
        GUM_DETAIL_ERRNO_STR(ENAMETOOLONG);
        GUM_DETAIL_ERRNO_STR(ENOLCK);
        GUM_DETAIL_ERRNO_STR(ENOSYS);
        GUM_DETAIL_ERRNO_STR(ENOTEMPTY);
        GUM_DETAIL_ERRNO_STR(ELOOP);
        GUM_DETAIL_ERRNO_STR(ENOMSG);
        GUM_DETAIL_ERRNO_STR(EIDRM);
        GUM_DETAIL_ERRNO_STR(ECHRNG);
        GUM_DETAIL_ERRNO_STR(EL2NSYNC);
        GUM_DETAIL_ERRNO_STR(EL3HLT);
        GUM_DETAIL_ERRNO_STR(EL3RST);
        GUM_DETAIL_ERRNO_STR(ELNRNG);
        GUM_DETAIL_ERRNO_STR(EUNATCH);
        GUM_DETAIL_ERRNO_STR(ENOCSI);
        GUM_DETAIL_ERRNO_STR(EL2HLT);
        GUM_DETAIL_ERRNO_STR(EBADE);
        GUM_DETAIL_ERRNO_STR(EBADR);
        GUM_DETAIL_ERRNO_STR(EXFULL);
        GUM_DETAIL_ERRNO_STR(ENOANO);
        GUM_DETAIL_ERRNO_STR(EBADRQC);
        GUM_DETAIL_ERRNO_STR(EBADSLT);

        GUM_DETAIL_ERRNO_STR(EBFONT);
        GUM_DETAIL_ERRNO_STR(ENOSTR);
        GUM_DETAIL_ERRNO_STR(ENODATA);
        GUM_DETAIL_ERRNO_STR(ETIME);
        GUM_DETAIL_ERRNO_STR(ENOSR);
        GUM_DETAIL_ERRNO_STR(ENONET);
        GUM_DETAIL_ERRNO_STR(ENOPKG);
        GUM_DETAIL_ERRNO_STR(EREMOTE);
        GUM_DETAIL_ERRNO_STR(ENOLINK);
        GUM_DETAIL_ERRNO_STR(EADV);
        GUM_DETAIL_ERRNO_STR(ESRMNT);
        GUM_DETAIL_ERRNO_STR(ECOMM);
        GUM_DETAIL_ERRNO_STR(EPROTO);
        GUM_DETAIL_ERRNO_STR(EMULTIHOP);
        GUM_DETAIL_ERRNO_STR(EDOTDOT);
        GUM_DETAIL_ERRNO_STR(EBADMSG);
        GUM_DETAIL_ERRNO_STR(EOVERFLOW);
        GUM_DETAIL_ERRNO_STR(ENOTUNIQ);
        GUM_DETAIL_ERRNO_STR(EBADFD);
        GUM_DETAIL_ERRNO_STR(EREMCHG);
        GUM_DETAIL_ERRNO_STR(ELIBACC);
        GUM_DETAIL_ERRNO_STR(ELIBBAD);
        GUM_DETAIL_ERRNO_STR(ELIBSCN);
        GUM_DETAIL_ERRNO_STR(ELIBMAX);
        GUM_DETAIL_ERRNO_STR(ELIBEXEC);
        GUM_DETAIL_ERRNO_STR(EILSEQ);
        GUM_DETAIL_ERRNO_STR(ERESTART);
        GUM_DETAIL_ERRNO_STR(ESTRPIPE);
        GUM_DETAIL_ERRNO_STR(EUSERS);
        GUM_DETAIL_ERRNO_STR(ENOTSOCK);
        GUM_DETAIL_ERRNO_STR(EDESTADDRREQ);
        GUM_DETAIL_ERRNO_STR(EMSGSIZE);
        GUM_DETAIL_ERRNO_STR(EPROTOTYPE);
        GUM_DETAIL_ERRNO_STR(ENOPROTOOPT);
        GUM_DETAIL_ERRNO_STR(EPROTONOSUPPORT);
        GUM_DETAIL_ERRNO_STR(ESOCKTNOSUPPORT);
        GUM_DETAIL_ERRNO_STR(EOPNOTSUPP);
        GUM_DETAIL_ERRNO_STR(EPFNOSUPPORT);
        GUM_DETAIL_ERRNO_STR(EAFNOSUPPORT);
        GUM_DETAIL_ERRNO_STR(EADDRINUSE);
        GUM_DETAIL_ERRNO_STR(EADDRNOTAVAIL);
        GUM_DETAIL_ERRNO_STR(ENETDOWN);
        GUM_DETAIL_ERRNO_STR(ENETUNREACH);
        GUM_DETAIL_ERRNO_STR(ENETRESET);
        GUM_DETAIL_ERRNO_STR(ECONNABORTED);
        GUM_DETAIL_ERRNO_STR(ECONNRESET);
        GUM_DETAIL_ERRNO_STR(ENOBUFS);
        GUM_DETAIL_ERRNO_STR(EISCONN);
        GUM_DETAIL_ERRNO_STR(ENOTCONN);
        GUM_DETAIL_ERRNO_STR(ESHUTDOWN);
        GUM_DETAIL_ERRNO_STR(ETOOMANYREFS);
        GUM_DETAIL_ERRNO_STR(ETIMEDOUT);
        GUM_DETAIL_ERRNO_STR(ECONNREFUSED);
        GUM_DETAIL_ERRNO_STR(EHOSTDOWN);
        GUM_DETAIL_ERRNO_STR(EHOSTUNREACH);
        GUM_DETAIL_ERRNO_STR(EALREADY);
        GUM_DETAIL_ERRNO_STR(EINPROGRESS);
        GUM_DETAIL_ERRNO_STR(ESTALE);
        GUM_DETAIL_ERRNO_STR(EUCLEAN);
        GUM_DETAIL_ERRNO_STR(ENOTNAM);
        GUM_DETAIL_ERRNO_STR(ENAVAIL);
        GUM_DETAIL_ERRNO_STR(EISNAM);
        GUM_DETAIL_ERRNO_STR(EREMOTEIO);
        GUM_DETAIL_ERRNO_STR(EDQUOT);

        GUM_DETAIL_ERRNO_STR(ENOMEDIUM);
        GUM_DETAIL_ERRNO_STR(EMEDIUMTYPE);
        GUM_DETAIL_ERRNO_STR(ECANCELED);

#ifdef ENOKEY
        GUM_DETAIL_ERRNO_STR(ENOKEY);
#endif
#ifdef EKEYEXPIRED
        GUM_DETAIL_ERRNO_STR(EKEYEXPIRED);
#endif
#ifdef EKEYREVOKED
        GUM_DETAIL_ERRNO_STR(EKEYREVOKED);
#endif
#ifdef EKEYREJECTED
        GUM_DETAIL_ERRNO_STR(EKEYREJECTED);
#endif

        GUM_DETAIL_ERRNO_STR(EOWNERDEAD);
        GUM_DETAIL_ERRNO_STR(ENOTRECOVERABLE);

        GUM_DETAIL_ERRNO_STR(ERFKILL);

    default:
        return to_string(err);
    };
}
#undef GUM_DETAIL_ERRNO_STR
}

String errno_to_string(int err) {
#ifdef GUM_HAS_STRERROR_R
    String result;
    char buf[256];
    if (const char* msg = strerror_r(err, buf, sizeof(buf)))
        result << msg;
    else
        result << "[Unknown errno]";

    return result << " (" << errno_code_to_string(err) << ")";
#else
    return String() << errno_code_to_string(err);
#endif
}
}
