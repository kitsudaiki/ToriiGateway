#ifndef RESPONSE_BUILDS_H
#define RESPONSE_BUILDS_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <string>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiHanamiCommon/enums.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

/**
 * @brief success_ResponseBuild
 * @param httpResp
 * @param message
 */
bool
success_ResponseBuild(http::response<http::dynamic_body> &httpResp,
                      const std::string &message)
{
    httpResp.result(http::status::ok);
    httpResp.set(http::field::content_type, "text/json");
    beast::ostream(httpResp.body()) << message;
    return true;
}

/**
 * @brief invalid_ResponseBuild
 * @param httpResp
 * @param message
 */
bool
invalid_ResponseBuild(http::response<http::dynamic_body> &httpResp,
                      const std::string &message)
{
    httpResp.result(http::status::bad_request);
    httpResp.set(http::field::content_type, "text/plain");
    beast::ostream(httpResp.body()) << message;
    Kitsunemimi::ErrorContainer error;
    error.errorMessage = message;
    LOG_ERROR(error);
    return false;
}

/**
 * @brief internalError_ResponseBuild
 * @param httpResp
 * @param message
 */
bool
internalError_ResponseBuild(http::response<http::dynamic_body> &httpResp,
                            const std::string &message)
{
    httpResp.result(http::status::internal_server_error);
    httpResp.set(http::field::content_type, "text/plain");
    Kitsunemimi::ErrorContainer error;
    error.errorMessage = message;
    LOG_ERROR(error);
    return false;
}

/**
 * @brief genericError_ResponseBuild
 * @param httpResp
 * @param type
 * @param message
 */
bool
genericError_ResponseBuild(http::response<http::dynamic_body> &httpResp,
                           const Kitsunemimi::Hanami::HttpResponseTypes type,
                           const std::string &message)
{
    httpResp.result(static_cast<http::status>(type));
    httpResp.set(http::field::content_type, "text/plain");
    beast::ostream(httpResp.body()) << message;
    Kitsunemimi::ErrorContainer error;
    error.errorMessage = message;
    LOG_ERROR(error);
    return false;
}

#endif // RESPONSE_BUILDS_H