#include <boost/program_options.hpp>
#include <iostream>

#include "BasicUser.hpp"
#include "ProxyServer.hpp"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");

    desc.add_options()("mode", po::value<std::string>()->required(),
                       "Mode Selection Proxy/User")(
        "channel", po::value<std::string>()->default_value("0"),
        "Channel for communication (default is '0')");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (po::error &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        std::cerr << desc << "\n";
        return 1;
    }

    std::string mode = vm["mode"].as<std::string>();
    std::string channel = vm["channel"].as<std::string>();

    try
    {
        if (mode == "Proxy")
        {
            ProxyServer proxy(12345);
            proxy.start();
        }
        else if (mode == "User")
        {
            BasicUser user(channel);
            try
            {
                user.start();
            }
            catch (const boost::system::system_error &e)
            {
                if (e.code() == boost::asio::error::connection_refused)
                {
                    std::cerr << "ERROR: Connection refused. The proxy server "
                                 "may not be running yet.\n";
                }
                else
                {
                    std::cerr << "ERROR: " << e.what() << "\n";
                }
                return 1;
            }
        }
        else
        {
            throw po::validation_error(
                po::validation_error::invalid_option_value, "mode");
        }
    }
    catch (const po::validation_error &e)
    {
        std::cerr << "ERROR: Invalid mode. Use --mode Proxy or --mode User.\n";
        std::cerr << "Allowed options:\n" << desc << "\n";
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
