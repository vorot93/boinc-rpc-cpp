#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include <vector>
#include <string>

#include <glibmm.h>

#include "models.hpp"

namespace Boinc
{
struct Client
{
  std::string addr;
  int port;
  std::string password;

  std::vector<Message> get_messages(int = 0);
  std::vector<ProjectInfo> get_projects();
  AccountManagerInfo get_account_manager_info();
  int get_account_manager_rpc_status();
  void account_manager_rpc(Glib::ustring, Glib::ustring, Glib::ustring);
  VersionInfo exchange_versions(VersionInfo);
  std::vector<Result> get_results(bool = false);
  void set_mode(Component, RunMode, double = 0);
  HostInfo get_host_info();
  void set_language(Glib::ustring);
};
}
#endif
