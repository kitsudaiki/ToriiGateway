#ifndef ARGS_H
#define ARGS_H

#include <libKitsunemimiArgs/arg_parser.h>

namespace ToriiGateway
{

bool
registerArguments(Kitsunemimi::Args::ArgParser &argparser)
{
    return argparser.registerString("config,c",
                                    "absolute path to config-file",
                                    false);
}

}

#endif // ARGS_H
