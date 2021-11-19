#include "file_send.h"

#include <libKitsunemimiCommon/common_methods/string_methods.h>
#include <libKitsunemimiCommon/files/text_file.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiConfig/config_handler.h>

/**
 * @brief HttpRequestEvent::getResponseType
 * @param ext
 * @return
 */
const std::string
getResponseType(const std::string &ext)
{
    if(beast::iequals(ext, ".htm"))  { return "text/html"; }
    if(beast::iequals(ext, ".html")) { return "text/html"; }
    if(beast::iequals(ext, ".php"))  { return "text/html"; }
    if(beast::iequals(ext, ".css"))  { return "text/css"; }
    if(beast::iequals(ext, ".txt"))  { return "text/plain"; }
    if(beast::iequals(ext, ".js"))   { return "application/javascript"; }
    if(beast::iequals(ext, ".json")) { return "application/json"; }
    if(beast::iequals(ext, ".xml"))  { return "application/xml"; }
    if(beast::iequals(ext, ".swf"))  { return "application/x-shockwave-flash"; }
    if(beast::iequals(ext, ".flv"))  { return "video/x-flv"; }
    if(beast::iequals(ext, ".png"))  { return "image/png"; }
    if(beast::iequals(ext, ".jpe"))  { return "image/jpeg"; }
    if(beast::iequals(ext, ".jpeg")) { return "image/jpeg"; }
    if(beast::iequals(ext, ".jpg"))  { return "image/jpeg"; }
    if(beast::iequals(ext, ".gif"))  { return "image/gif"; }
    if(beast::iequals(ext, ".bmp"))  { return "image/bmp"; }
    if(beast::iequals(ext, ".ico"))  { return "image/vnd.microsoft.icon"; }
    if(beast::iequals(ext, ".tiff")) { return "image/tiff"; }
    if(beast::iequals(ext, ".tif"))  { return "image/tiff"; }
    if(beast::iequals(ext, ".svg"))  { return "image/svg+xml"; }
    if(beast::iequals(ext, ".svgz")) { return "image/svg+xml"; }

    return "application/text";
}

/**
 * @brief send file, which was requested
 *
 * @return true, if successful, else false
 */
bool
sendFileFromLocalLocation(http::response<http::dynamic_body> &response,
                          const std::string &dir,
                          const std::string &relativePath,
                          Kitsunemimi::ErrorContainer &error)
{
    // create file-path
    std::string path = dir + relativePath;
    if(relativePath == "/"
            || relativePath == "")
    {
        path = "/index.html";
    }

    LOG_DEBUG("load file " + path);

    // set response-type based on file-type
    std::filesystem::path pathObj(path);
    const std::string extension = pathObj.extension().string();
    response.set(http::field::content_type, getResponseType(extension));

    // read file and send content back
    std::string fileContent = "";
    if(Kitsunemimi::readFile(fileContent, path, error))
    {
        beast::ostream(response.body()) << fileContent;
        return true;
    }

    response.result(http::status::internal_server_error);
    response.set(http::field::content_type, "text/plain");
    LOG_ERROR(error);

    return false;
}

/**
 * @brief HttpRequestEvent::processClientRequest
 * @param path
 * @return
 */
bool
processClientRequest(http::response<http::dynamic_body> &response,
                     const std::string &path,
                     Kitsunemimi::ErrorContainer &error)
{
    bool success = false;
    const std::string fileLocation = GET_STRING_CONFIG("server", "dashboard_files", success);
    return sendFileFromLocalLocation(response, fileLocation, path, error);
}
