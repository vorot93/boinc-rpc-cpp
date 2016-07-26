#ifndef _MODELS_HPP_
#define _MODELS_HPP_

#include <vector>
#include <experimental/optional>

#include <glibmm.h>

namespace Boinc
{
enum class Component
{
  CPU,
  GPU,
  NETWORK
};

enum class RunMode
{
  ALWAYS,
  AUTO,
  NEVER,
  RESTORE
};

enum class CpuSched
{
  UNINITIALIZED,
  PREEMPTED,
  SCHEDULED
};

enum class ResultState
{
  NEW,
  FILES_DOWNLOADING,
  FILES_DOWNLOADED,
  COMPUTE_ERROR,
  FILES_UPLOADING,
  FILES_UPLOADED,
  ABORTED,
  UPLOAD_FAILED
};

enum class Process
{
  UNINITIALIZED = 0,
  EXECUTING = 1,
  SUSPENDED = 9,
  ABORT_PENDING = 5,
  QUIT_PENDING = 8,
  COPY_PENDING = 10
};

struct VersionInfo
{
  std::experimental::optional<int> major;
  std::experimental::optional<int> minor;
  std::experimental::optional<int> release;
};

struct HostInfo
{
  std::experimental::optional<int> tz_shift;
  std::experimental::optional<Glib::ustring> domain_name;
  std::experimental::optional<Glib::ustring> serialnum;
  std::experimental::optional<Glib::ustring> ip_addr;
  std::experimental::optional<Glib::ustring> host_cpid;

  std::experimental::optional<int> p_ncpus;
  std::experimental::optional<Glib::ustring> p_vendor;
  std::experimental::optional<Glib::ustring> p_model;
  std::experimental::optional<Glib::ustring> p_features;
  std::experimental::optional<double> p_fpops;
  std::experimental::optional<double> p_iops;
  std::experimental::optional<double> p_membw;
  std::experimental::optional<double> p_calculated;
  std::experimental::optional<bool> p_vm_extensions_disabled;

  std::experimental::optional<double> m_nbytes;
  std::experimental::optional<double> m_cache;
  std::experimental::optional<double> m_swap;

  std::experimental::optional<double> d_total;
  std::experimental::optional<double> d_free;

  std::experimental::optional<Glib::ustring> os_name;
  std::experimental::optional<Glib::ustring> os_version;
  std::experimental::optional<Glib::ustring> product_name;

  std::experimental::optional<Glib::ustring> mac_address;

  std::experimental::optional<Glib::ustring> virtualbox_version;
};

struct ProjectInfo
{
  std::experimental::optional<Glib::ustring> name;
  std::experimental::optional<Glib::ustring> summary;
  std::experimental::optional<Glib::ustring> url;
  std::experimental::optional<Glib::ustring> general_area;
  std::experimental::optional<Glib::ustring> specific_area;
  std::experimental::optional<Glib::ustring> description;
  std::experimental::optional<Glib::ustring> home;
  std::experimental::optional<std::vector<Glib::ustring>> platforms;
  std::experimental::optional<Glib::ustring> image;
};

struct AccountManagerInfo
{
  std::experimental::optional<Glib::ustring> url;
  std::experimental::optional<Glib::ustring> name;
  std::experimental::optional<bool> have_credentials;
  std::experimental::optional<bool> cookie_required;
  std::experimental::optional<Glib::ustring> cookie_failure_url;
};

struct Message
{
  std::experimental::optional<Glib::ustring> name;
  std::experimental::optional<int> priority;
  std::experimental::optional<int> msg_number;
  std::experimental::optional<Glib::ustring> body;
  std::experimental::optional<double> dt;
};

struct Result
{
  std::experimental::optional<Glib::ustring> name;
  std::experimental::optional<Glib::ustring> wu_name;
  std::experimental::optional<Glib::ustring> platform;
  std::experimental::optional<int> version_num;
  std::experimental::optional<Glib::ustring> plan_class;
  std::experimental::optional<Glib::ustring> project_url;
  std::experimental::optional<double> final_cpu_time;
  std::experimental::optional<double> final_elapsed_time;
  std::experimental::optional<int> exit_status;
  std::experimental::optional<int> state;
  std::experimental::optional<double> report_deadline;
  std::experimental::optional<double> received_time;
  std::experimental::optional<double> estimated_cpu_time_remaining;
  std::experimental::optional<double> completed_time;
};
}

#endif
