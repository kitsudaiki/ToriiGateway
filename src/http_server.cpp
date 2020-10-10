#include "http_server.h"

#include <libKitsunemimiCommon/common_items/data_items.h>

#include <libKitsunemimiSakuraMessaging/messaging_controller.h>
#include <libKitsunemimiSakuraMessaging/messaging_client.h>

HttpServer::HttpServer(const std::string &url)
    : http_listener(url)
{
    support(methods::GET, std::bind(&HttpServer::handle_get, this, std::placeholders::_1));
    support(methods::PUT, std::bind(&HttpServer::handle_put, this, std::placeholders::_1));
    support(methods::POST, std::bind(&HttpServer::handle_post, this, std::placeholders::_1));
    support(methods::DEL, std::bind(&HttpServer::handle_delete, this, std::placeholders::_1));
}

HttpServer::~HttpServer()
{

}

void
HttpServer::startListener()
{
    open().wait();
}

void
HttpServer::closeListener()
{
    close().wait();
}


void
HttpServer::handle_error(pplx::task<void>& t)
{
    try {
        t.get();
    } catch(...) {
        // Ignore the error, Log it if a logger is available
    }
}


void
HttpServer::handle_get(const http_request &message)
{
    std::cout<<"GET: "<<message.to_string()<<std::endl;
    std::string path = http::uri::decode(message.relative_uri().path());
    std::cout<<"path: "<<path<<std::endl;
    auto paths = http::uri::split_path(path);

    Kitsunemimi::Sakura::MessagingClient* client = Kitsunemimi::Sakura::MessagingController::getInstance()->getClient("contr1");
    std::string errorMessage = "";
    Kitsunemimi::DataMap inputValues;
    inputValues.insert("input", new Kitsunemimi::DataValue(42));
    inputValues.insert("test_output", new Kitsunemimi::DataValue(""));

    Kitsunemimi::DataMap resultingItem;
    client->triggerSakuraFile(resultingItem,
                              "test-tree",
                              inputValues,
                              errorMessage);


    message.reply(status_codes::OK, resultingItem.toString());
    return;
};


void
HttpServer::handle_post(const http_request &message)
{
    std::cout<<"POST: "<<message.to_string()<<std::endl;
    message.reply(status_codes::OK);
    return ;
};


void
HttpServer::handle_delete(const http_request &message)
{
    std::cout<<"DELETE: "<<message.to_string()<<std::endl;
    std::string rep = U("WRITE YOUR OWN DELETE OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};


void
HttpServer::handle_put(const http_request &message)
{
    std::cout<<"PUT: "<<message.to_string()<<std::endl;
    std::string rep = U("WRITE YOUR OWN PUT OPERATION");
    message.reply(status_codes::OK, rep);
    return;
};
