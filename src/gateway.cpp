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

#include <libKitsunemimiPersistence/logger/logger.h>
#include <libKitsunemimiConfig/config_handler.h>

#include <libKitsunemimiCommon/buffer/data_buffer.h>

#include <libKitsunemimiSakuraLang/sakura_lang_interface.h>
#include <libKitsunemimiSakuraLang/blossom.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

#include <libKitsunemimiPersistence/files/text_file.h>

using Kitsunemimi::Sakura::SakuraLangInterface;

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
                        const uint16_t port)
{
    std::string errorMessage = "";

    SakuraLangInterface* interface = SakuraLangInterface::getInstance();
    interface->addBlossom("test1", "test2",  new TestBlossom());
    interface->addTree("test-tree", getTestTree(), errorMessage);

    Kitsunemimi::Persistence::writeFile("/tmp/ToriiGateway.conf",
                                        getTestConfig(),
                                        errorMessage,
                                        true);


    Kitsunemimi::Config::initConfig("/tmp/ToriiGateway.conf");
    std::vector<std::string> groupNames = {"ToriiGateway"};
    Kitsunemimi::Sakura::MessagingController::initializeMessagingController("ToriiGateway", groupNames);


    HttpServer* server = new HttpServer(address, port);
    server->startListener();

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
                               "[ToriiGateway]\n"
                               "port = 12345\n"
                               "address = \"127.0.0.1\"\n";
    return config;
}
