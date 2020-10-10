/**
 *  @file    gatewayServer.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  Apache License Version 2.0
 */

#include "gateway.h"

#include <http_server.h>
#include <test_blossom.h>

#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

#include <libKitsunemimiPersistence/logger/logger.h>
#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

#include <libKitsunemimiPersistence/files/text_file.h>


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

/**
 * @brief Gateway::initHttpServer
 * @param address
 * @param port
 * @return
 */
bool
Gateway::initHttpServer(const std::string &address,
                        const std::string &port)
{
    TestBlossom* newBlossom = new TestBlossom();
    Kitsunemimi::Sakura::SakuraLangInterface::getInstance()->addBlossom("test1",
                                                   "test2",
                                                   newBlossom);
    std::string errorMessage = "";
    Kitsunemimi::Sakura::SakuraLangInterface::getInstance()->addTree("test-tree",
                                                getTestTree(),
                                                errorMessage);
    Kitsunemimi::Persistence::writeFile("/tmp/test-config.conf",
                                        getTestConfig(),
                                        errorMessage,
                                        true);

    std::string host = "http://" + address + ":" + port;
    web::uri_builder uri(host);
    uri.append(U("MyServer/Action"));
    std::string path = uri.to_uri().to_string();

    std::cout<<"listen on path: "<<path<<std::endl;
    HttpServer* server = new HttpServer(path);
    server->open().wait();

    Kitsunemimi::Config::initConfig("/tmp/test-config.conf");
    std::vector<std::string> groupNames = {"self"};
    Kitsunemimi::Sakura::MessagingController::initializeMessagingController("contr1", groupNames);

    return true;
}

/**
 * @brief Session_Test::getTestTree
 * @return
 */
const std::string
Gateway::getTestTree()
{
    const std::string tree = "[\"test\"]\n"
                             "- input = \"{{}}\"\n"
                             "- test_output = \"\"\n"
                             "\n"
                             "test1(\"this is a test\")\n"
                             "->test2:\n"
                             "   - input = input\n"
                             "   - output >> test_output\n";
    return tree;
}

/**
 * @brief Session_Test::getTestConfig
 * @return
 */
const std::string
Gateway::getTestConfig()
{
    const std::string config = "[DEFAULT]\n"
                               "port = 12345\n"
                               "\n"
                               "\n"
                               "[self]\n"
                               "port = 12345\n"
                               "address = \"127.0.0.1\"\n";
    return config;
}
