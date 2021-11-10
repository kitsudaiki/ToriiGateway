#ifndef FILE_SEND_H
#define FILE_SEND_H

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

#include <string>
#include <filesystem>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>

const std::string getResponseType(const std::string &ext);
bool sendFileFromLocalLocation(http::response<http::dynamic_body> &response,
                               const std::string &dir,
                               const std::string &relativePath);
bool processClientRequest(http::response<http::dynamic_body> &response,
                          const std::string &path);

#endif // FILE_SEND_H
