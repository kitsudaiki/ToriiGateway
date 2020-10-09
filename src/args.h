#ifndef ARGS_H
#define ARGS_H

#include <libKitsunemimiArgs/arg_parser.h>

namespace ToriiGateway
{

bool
registerArguments(Kitsunemimi::Args::ArgParser &argparser)
{
    argparser.registerString("config,c",
                             "absolute path to config-file",
                             true);

    argparser.registerPlain("debug,d",
                            "enable debug-mode");
    return true;
}

}

#endif // ARGS_H
