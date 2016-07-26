#ifndef _EXCEPTION_LIST_HPP_
#define _EXCEPTION_LIST_HPP_

#include "exception_util.hpp"

namespace Boinc
{
DEFINE_EXCEPTION(ConnectError, "failed to connect");
DEFINE_EXCEPTION(DataParseError, "error parsing data");
DEFINE_EXCEPTION(InvalidPasswordError, "invalid password");
DEFINE_EXCEPTION(DaemonError, "daemon returned error");
DEFINE_EXCEPTION(NullError, "unexpected null detected");
DEFINE_EXCEPTION(NetworkError, "network error occurred");
DEFINE_EXCEPTION(AuthError, "auth error occurred");
DEFINE_EXCEPTION(InvalidURLError, "invalid URL");
DEFINE_EXCEPTION(AlreadyAttachedError, "already attached");
}
#endif
