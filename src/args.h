#ifndef ARGS_H
#define ARGS_H

#include <libKitsunemimiArgs/arg_parser.h>

namespace ToriiGateway
{

bool
registerArguments(Kitsunemimi::Args::ArgParser &argparser)
{
    argparser.registerInteger("port",
                              "port on which ToriiGateway should listen",
                              true);

    argparser.registerPlain("debug",
                            "enable debug-mode");
    return true;
}

}

#endif // ARGS_H
