#ifndef _RPC_HPP_
#define _RPC_HPP_

#include <string>

#include <glibmm.h>

#include "util.hpp"

namespace Boinc
{
std::string compute_nonce_hash(std::string, std::string);
void query_boinc_daemon(Glib::ustring, int, Glib::ustring, XMLCallback, XMLCallback = nullptr);
}
#endif
