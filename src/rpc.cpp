#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <glibmm.h>
#include <libxml++/libxml++.h>

#include "exception_list.hpp"
#include "models.hpp"
#include "util.hpp"

#include "rpc.hpp"

namespace Boinc
{

std::string
compute_nonce_hash(std::string pass, std::string nonce)
{
  return Glib::Checksum::compute_checksum(Glib::Checksum::ChecksumType::CHECKSUM_MD5, (nonce + pass));
};

void
query_boinc_daemon(Glib::ustring host, int port, Glib::ustring password, XMLCallback request_writer, XMLCallback success_response_handler)
{
  if (!request_writer)
  {
    success_response_handler = nullptr;
  }

  boost::asio::io_service ios;
  boost::asio::ip::tcp::socket socket(ios);
  socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host.raw()), port));

  xmlpp::Document req_doc("1.0");

  auto req_root = req_doc.create_root_node("boinc_gui_rpc_request");
  req_root->add_child("auth1");

  bool auth_complete = false;
  bool request_sent = false;
  while (true)
  {
    if (req_root->get_children().empty())
    {
      return;
    }
    auto req_string = boost::replace_all_copy(req_doc.write_to_string().raw(), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", "");
    xml_clear_children(req_root);

    req_string += '\3';
#ifndef NDEBUG
    std::cout << req_string << std::endl;
#endif
    socket.write_some(boost::asio::buffer(req_string));

    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, '\3');
    auto recv_data =
      boost::algorithm::trim_right_copy_if(std::string(boost::asio::buffers_begin(buf.data()), boost::asio::buffers_begin(buf.data()) + buf.size()), [](char v) -> bool { return v == '\3'; });
#ifndef NDEBUG
    std::cout << recv_data << std::endl;
#endif

    auto rsp_doc = load_xml(boost::replace_all_copy(recv_data, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>", ""));
    auto root_node = rsp_doc->get_root_node();
    if (root_node->get_name() != "boinc_gui_rpc_reply")
    {
      throw DataParseError("invalid response XML root node");
    }

    bool auth_in_progress = false;
    bool done = false;

    XMLCallbackMap b;

    b["nonce"] = [req_root, &auth_in_progress, password](xmlpp::Node* v) {
      req_root->add_child("auth2")->add_child("nonce_hash")->add_child_text(compute_nonce_hash(password, v->eval_to_string(".")));

      auth_in_progress = true;
    };
    b["unauthorized"] = [password](xmlpp::Node*) { throw InvalidPasswordError(password.raw()); };
    b["error"] = [](xmlpp::Node* v) { throw DaemonError(Glib::ustring::compose("BOINC daemon returned error: %1", (v->eval_to_string("."))).raw()); };
    b["authorized"] = [&auth_complete, &auth_in_progress](xmlpp::Node* v) {
      auth_complete = true;
      auth_in_progress = false;
    };

    map_xml_node(root_node, b);

    if ((auth_complete && request_sent) && success_response_handler)
    {
      try
      {
        success_response_handler(root_node);
        done = true;
      }
      catch (const std::exception& e)
      {
        throw DataParseError(Glib::ustring::compose("%1 : %2", e.what(), xml_node_to_string(root_node)));
      }
    }

    if (done || (auth_complete && !success_response_handler))
    {
      return;
    }

    if ((auth_complete && !request_sent) && request_writer)
    {
      request_writer(req_root);
      request_sent = true;
    }

    if (!auth_in_progress && !request_sent)
    {
      throw DataParseError(Glib::ustring::compose("Invalid XML response: %1", recv_data).raw());
    }
  }
}
}
