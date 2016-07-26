#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <functional>
#include <map>
#include <memory>

#include <glibmm.h>
#include <libxml++/libxml++.h>

namespace Boinc
{
typedef std::function<void(xmlpp::Node*)> XMLCallback;
typedef std::map<Glib::ustring, XMLCallback> XMLCallbackMap;

void map_xml_node(xmlpp::Node*, XMLCallbackMap, std::function<void(Glib::ustring)> = nullptr);
void xml_clear_children(xmlpp::Node*);
Glib::ustring xml_node_to_string(xmlpp::Node* = nullptr);
std::shared_ptr<xmlpp::Document> load_xml(Glib::ustring);
}
#endif
