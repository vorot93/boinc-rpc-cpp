#include <experimental/optional>
#include <vector>

#include <boost/algorithm/string/trim_all.hpp>
#include <glibmm.h>
#include <libxml++/libxml++.h>

#include "exception_list.hpp"
#include "models.hpp"
#include "rpc.hpp"
#include "util.hpp"

#include "client.hpp"

namespace Boinc
{
void
verify_rpc_reply(xmlpp::Node* root_node)
{
  XMLCallbackMap b;

  bool success = false;
  b["success"] = [&success](xmlpp::Node*) {
    success = true;
    return;
  };
  b["status"] = [](xmlpp::Node* v) { throw NetworkError(v->eval_to_string(".")); };
  b["unauthorized"] = [](xmlpp::Node*) { throw AuthError(); };
  b["error"] = [](xmlpp::Node* v) {
    auto error_msg = v->eval_to_string(".");

    if ((error_msg == "unauthorized") || (error_msg == "Missing authenticator"))
    {
      throw AuthError(error_msg);
    }
    if (error_msg == "Missing URL")
    {
      throw InvalidURLError(error_msg);
    }
    if (error_msg == "Already attached to project")
    {
      throw AlreadyAttachedError(error_msg);
    }

    throw DataParseError(error_msg);
  };

  map_xml_node(root_node, b, [](Glib::ustring k) { throw DataParseError(Glib::ustring::compose("Unknown node '%1' in reply", k).raw()); });
  if (!success)
  {
    throw DataParseError("success not confirmed in reply");
  }
}

std::vector<Message>
Client::get_messages(int seqno)
{
  std::vector<Message> v;
  query_boinc_daemon(this->addr, this->port, this->password, [seqno](xmlpp::Node* root_node) { root_node->add_child("get_messages")->add_child_text(Glib::ustring::format(seqno)); },
    [&v](xmlpp::Node* root_node) {
      bool success = false;
      XMLCallbackMap b;
      b["msgs"] = [&v, &success](xmlpp::Node* n) {
        success = true;
        XMLCallbackMap b1;

        b1["msg"] = [&v](xmlpp::Node* entry_node) {
          Message entry;

          XMLCallbackMap b2;
          b2["name"] = [&entry](xmlpp::Node* node) { entry.name = node->eval_to_string("."); };
          b2["pri"] = [&entry](xmlpp::Node* node) {
            auto text = node->eval_to_string(".");
            if (!text.empty())
            {
              entry.priority = std::stoi(text);
            }
          };
          b2["seqno"] = [&entry](xmlpp::Node* node) {
            auto text = node->eval_to_string(".");
            if (!text.empty())
            {
              entry.msg_number = std::stoi(text);
            }
          };
          b2["body"] = [&entry](xmlpp::Node* node) {
            auto text = boost::algorithm::trim_all_copy_if(node->eval_to_string(".").raw(), [](char c) { return c == '\n' || c == ' '; });
            if (!text.empty())
            {
              entry.body = text;
            }
          };
          b2["time"] = [&entry](xmlpp::Node* node) {
            auto text = node->eval_to_string(".");
            if (!text.empty())
            {
              entry.dt = std::stoi(text);
            }
          };
          map_xml_node(entry_node, b2);

          v.push_back(entry);
        };
        map_xml_node(n, b1);
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("msgs node not found");
      }
    });
  return v;
}

std::vector<ProjectInfo>
Client::get_projects()
{
  std::vector<ProjectInfo> v;
  query_boinc_daemon(this->addr, this->port, this->password, [](xmlpp::Node* root_node) { root_node->add_child("get_all_projects_list"); },
    [&v](xmlpp::Node* root_node) {
      XMLCallbackMap b;
      bool success = false;
      b["projects"] = [&v, &success](xmlpp::Node* n) {
        success = true;
        XMLCallbackMap b1;

        b1["project"] = [&v](xmlpp::Node* entry_node) {
          ProjectInfo entry;

          XMLCallbackMap b2;
          b2["name"] = [&entry](xmlpp::Node* node) { entry.name = node->eval_to_string("."); };
          b2["summary"] = [&entry](xmlpp::Node* node) { entry.summary = node->eval_to_string("."); };
          b2["url"] = [&entry](xmlpp::Node* node) { entry.url = node->eval_to_string("."); };
          b2["general_area"] = [&entry](xmlpp::Node* node) { entry.general_area = node->eval_to_string("."); };
          b2["specific_area"] = [&entry](xmlpp::Node* node) { entry.specific_area = node->eval_to_string("."); };
          b2["description"] = [&entry](xmlpp::Node* node) {
            Glib::ustring body;
            for (auto subnode : node->get_children())
            {
              body += subnode->eval_to_string(".");
            }
            entry.description = body;
          };
          b2["home"] = [&entry](xmlpp::Node* node) { entry.home = node->eval_to_string("."); };
          b2["platforms"] = [&entry](xmlpp::Node* node) {
            std::vector<Glib::ustring> arr;
            for (auto subnode : node->get_children())
            {
              if (subnode->get_name() == "platform")
              {
                auto text = subnode->eval_to_string(".");
                if (!text.empty())
                {
                  arr.push_back(text);
                }
              }
            }
            entry.platforms = arr;
          };
          b2["image"] = [&entry](xmlpp::Node* node) { entry.image = node->eval_to_string("."); };

          map_xml_node(entry_node, b2);
          v.push_back(entry);
        };
        map_xml_node(n, b1);
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("projects node not found");
      }
    });

  return v;
}

AccountManagerInfo
Client::get_account_manager_info()
{
  AccountManagerInfo v;
  query_boinc_daemon(this->addr, this->port, this->password, [](xmlpp::Node* root_node) { root_node->add_child("acct_mgr_info"); },
    [&v](xmlpp::Node* root_node) {
      bool success = false;
      XMLCallbackMap b;
      b["acct_mgr_info"] = [&v, &success](xmlpp::Node* n) {
        success = true;
        XMLCallbackMap b2;
        b2["acct_mgr_url"] = [&v](xmlpp::Node* node) { v.url = node->eval_to_string("."); };
        b2["acct_mgr_name"] = [&v](xmlpp::Node* node) { v.name = node->eval_to_string("."); };
        b2["have_credentials"] = [&v](xmlpp::Node* node) { v.have_credentials = true; };
        b2["cookie_required"] = [&v](xmlpp::Node* node) { v.cookie_required = true; };
        b2["cookie_failure_url"] = [&v](xmlpp::Node* node) { v.cookie_failure_url = node->eval_to_string("."); };
        map_xml_node(n, b2);
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("acct_mgr_info node not found");
      }
    });
  return v;
}

int
Client::get_account_manager_rpc_status()
{
  int v = 0;
  query_boinc_daemon(this->addr, this->port, this->password, [](xmlpp::Node* root_node) { root_node->add_child("acct_mgr_rpc_poll"); },
    [&v](xmlpp::Node* root_node) {
      bool success = false;
      XMLCallbackMap b;
      b["acct_mgr_rpc_reply"] = [&v, &success](xmlpp::Node* n) {
        success = true;
        XMLCallbackMap b2;
        b2["error_num"] = [&v](xmlpp::Node* node) { v = node->eval_to_number("."); };
        map_xml_node(n, b2);
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("acct_mgr_rpc_reply node not found");
      }
    });
  return v;
}

void
Client::account_manager_rpc(Glib::ustring url, Glib::ustring name, Glib::ustring password)
{
  query_boinc_daemon(this->addr, this->port, this->password, [url, name, password](xmlpp::Node* root_node) {
    auto rpc_node = root_node->add_child("acct_mgr_rpc");
    rpc_node->add_child("url")->add_child_text(url);
    rpc_node->add_child("name")->add_child_text(name);
    rpc_node->add_child("password")->add_child_text(password);
  });
}

VersionInfo
Client::exchange_versions(VersionInfo info)
{
  VersionInfo v;
  query_boinc_daemon(this->addr, this->port, this->password,
    [info](xmlpp::Node* root_node) {
      auto n = root_node->add_child("exchange_versions");
      try
      {
        n->add_child("major")->add_child_text(Glib::ustring::format(info.major.value()));
      }
      catch (const std::experimental::bad_optional_access& e)
      {
      }
      try
      {
        n->add_child("minor")->add_child_text(Glib::ustring::format(info.minor.value()));
      }
      catch (const std::experimental::bad_optional_access& e)
      {
      }
      try
      {
        n->add_child("release")->add_child_text(Glib::ustring::format(info.release.value()));
      }
      catch (const std::experimental::bad_optional_access& e)
      {
      }
    },
    [&v](xmlpp::Node* root_node) {
      bool success = false;
      XMLCallbackMap b;
      b["server_version"] = [&v, &success](xmlpp::Node* node) {
        success = true;
        XMLCallbackMap b2;
        b2["major"] = [&v](xmlpp::Node* n) { v.major = n->eval_to_number("."); };
        b2["minor"] = [&v](xmlpp::Node* n) { v.minor = n->eval_to_number("."); };
        b2["release"] = [&v](xmlpp::Node* n) { v.release = n->eval_to_number("."); };
        map_xml_node(node, b2);
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("server_version node not found");
      }
    });

  return v;
}

std::vector<Result>
Client::get_results(bool active_only)
{
  std::vector<Result> v;
  query_boinc_daemon(this->addr, this->port, this->password,
    [active_only](xmlpp::Node* root_node) { root_node->add_child("get_results")->add_child("active_only")->add_child_text(active_only ? "1" : "0"); },
    [&v](xmlpp::Node* root_node) {
      bool success = false;
      XMLCallbackMap b;
      b["results"] = [&v, &success](xmlpp::Node* results_node) {
        XMLCallbackMap b2;
        b2["result"] = [&v](xmlpp::Node* entry_node) {
          Result entry;

          XMLCallbackMap b3;
          b3["name"] = [&entry](xmlpp::Node* subnode) { entry.name = subnode->eval_to_string("."); };
          b3["wu_name"] = [&entry](xmlpp::Node* subnode) { entry.wu_name = subnode->eval_to_string("."); };
          b3["platform"] = [&entry](xmlpp::Node* subnode) { entry.platform = subnode->eval_to_string("."); };
          b3["version_num"] = [&entry](xmlpp::Node* subnode) { entry.version_num = subnode->eval_to_number("."); };
          b3["plan_class"] = [&entry](xmlpp::Node* subnode) { entry.plan_class = subnode->eval_to_string("."); };
          b3["project_url"] = [&entry](xmlpp::Node* subnode) { entry.project_url = subnode->eval_to_string("."); };
          b3["final_cpu_time"] = [&entry](xmlpp::Node* subnode) { entry.final_cpu_time = subnode->eval_to_number("."); };
          b3["final_elapsed_time"] = [&entry](xmlpp::Node* subnode) { entry.final_elapsed_time = subnode->eval_to_number("."); };
          b3["exit_status"] = [&entry](xmlpp::Node* subnode) { entry.exit_status = subnode->eval_to_number("."); };
          b3["state"] = [&entry](xmlpp::Node* subnode) { entry.state = subnode->eval_to_number("."); };
          b3["report_deadline"] = [&entry](xmlpp::Node* subnode) { entry.report_deadline = subnode->eval_to_number("."); };
          b3["received_time"] = [&entry](xmlpp::Node* subnode) { entry.received_time = subnode->eval_to_number("."); };
          b3["estimated_cpu_time_remaining"] = [&entry](xmlpp::Node* subnode) { entry.estimated_cpu_time_remaining = subnode->eval_to_number("."); };
          b3["completed_time"] = [&entry](xmlpp::Node* subnode) { entry.completed_time = subnode->eval_to_number("."); };
          map_xml_node(entry_node, b3);

          v.push_back(entry);
        };
        map_xml_node(results_node, b2);
        success = true;
      };
      map_xml_node(root_node, b);
      if (!success)
      {
        throw DataParseError("results node not found");
      }
    });

  return v;
}

void
Client::set_mode(Component component, RunMode mode, double duration)
{
  std::string comp_desc;
  switch (component)
  {
  case Component::CPU:
    comp_desc = "run";
    break;

  case Component::GPU:
    comp_desc = "gpu";
    break;

  case Component::NETWORK:
    comp_desc = "network";
    break;
  }

  std::string mode_desc;
  switch (mode)
  {
  case RunMode::ALWAYS:
    mode_desc = "always";
    break;

  case RunMode::AUTO:
    mode_desc = "auto";
    break;

  case RunMode::NEVER:
    mode_desc = "never";
    break;

  case RunMode::RESTORE:
    mode_desc = "restore";
    break;
  }

  query_boinc_daemon(this->addr, this->port, this->password,
    [comp_desc, mode_desc, duration](
                       xmlpp::Node* root_node) { root_node->add_child(Glib::ustring::compose("set_%1_mode", comp_desc))->add_child("duration")->add_child_text(Glib::ustring::format(duration)); },
    verify_rpc_reply);
}

HostInfo
Client::get_host_info()
{
  HostInfo v;

  query_boinc_daemon(this->addr, this->port, this->password, [](xmlpp::Node* root_node) { root_node->add_child("get_host_info"); },
    [&v](xmlpp::Node* root_node) {
      bool success = false;

      XMLCallbackMap b;
      b["host_info"] = [&v, &success](xmlpp::Node* n) {
        success = true;
        XMLCallbackMap b2;
        b2["p_fpops"] = [&v](xmlpp::Node* node) { v.p_fpops = node->eval_to_number("."); };
        b2["p_iops"] = [&v](xmlpp::Node* node) { v.p_iops = node->eval_to_number("."); };
        b2["p_membw"] = [&v](xmlpp::Node* node) { v.p_membw = node->eval_to_number("."); };
        b2["p_calculated"] = [&v](xmlpp::Node* node) { v.p_calculated = node->eval_to_number("."); };
        b2["p_vm_extensions_disabled"] = [&v](xmlpp::Node* node) { v.p_vm_extensions_disabled = node->eval_to_boolean("."); };
        b2["host_cpid"] = [&v](xmlpp::Node* node) { v.host_cpid = node->eval_to_string("."); };
        b2["product_name"] = [&v](xmlpp::Node* node) { v.product_name = node->eval_to_string("."); };
        b2["mac_address"] = [&v](xmlpp::Node* node) { v.mac_address = node->eval_to_string("."); };
        b2["domain_name"] = [&v](xmlpp::Node* node) { v.domain_name = node->eval_to_string("."); };

        b2["ip_addr"] = [&v](xmlpp::Node* node) { v.ip_addr = node->eval_to_string("."); };
        b2["p_vendor"] = [&v](xmlpp::Node* node) { v.p_vendor = node->eval_to_string("."); };
        b2["p_model"] = [&v](xmlpp::Node* node) { v.p_model = node->eval_to_string("."); };
        b2["os_name"] = [&v](xmlpp::Node* node) { v.os_name = node->eval_to_string("."); };
        b2["os_version"] = [&v](xmlpp::Node* node) { v.os_version = node->eval_to_string("."); };
        b2["virtualbox_version"] = [&v](xmlpp::Node* node) { v.virtualbox_version = node->eval_to_string("."); };
        b2["p_features"] = [&v](xmlpp::Node* node) { v.p_features = node->eval_to_string("."); };

        b2["timezone"] = [&v](xmlpp::Node* node) { v.tz_shift = node->eval_to_number("."); };
        b2["p_ncpus"] = [&v](xmlpp::Node* node) { v.p_ncpus = node->eval_to_number("."); };

        b2["m_nbytes"] = [&v](xmlpp::Node* node) { v.m_nbytes = node->eval_to_number("."); };
        b2["m_cache"] = [&v](xmlpp::Node* node) { v.m_cache = node->eval_to_number("."); };
        b2["m_swap"] = [&v](xmlpp::Node* node) { v.m_swap = node->eval_to_number("."); };
        b2["d_total"] = [&v](xmlpp::Node* node) { v.d_total = node->eval_to_number("."); };
        b2["d_free"] = [&v](xmlpp::Node* node) { v.d_free = node->eval_to_number("."); };

        map_xml_node(n, b2);
      };
      map_xml_node(root_node, b);

      if (!success)
      {
        throw DataParseError("host_info node not found");
      }
    });

  return v;
}

void
Client::set_language(Glib::ustring language)
{
  query_boinc_daemon(
    this->addr, this->port, this->password, [language](xmlpp::Node* root_node) { root_node->add_child("set_language")->add_child("language")->add_child_text(language); }, verify_rpc_reply);
}
}
