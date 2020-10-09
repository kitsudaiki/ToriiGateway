/**
 *  @file    gatewayServer.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gateway.h"

#include <httpServer.h>

#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

#include <libKitsunemimiPersistence/logger/logger.h>
#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>


/**
 * @brief GatewayServer::GatewayServer
 */
Gateway::Gateway()
{

}

/**
 * @brief Gateway::~Gateway
 */
Gateway::~Gateway()
{
}

bool Gateway::initHttpServer()
{
    std::string host = "http://127.0.0.1:8000";
    web::uri_builder uri(host);
    uri.append(U("MyServer/Action"));
    std::string path = uri.to_uri().to_string();

    std::cout<<"listen on path: "<<path<<std::endl;
    HttpServer* server = new HttpServer(path);
    server->open().wait();

    return true;
}

