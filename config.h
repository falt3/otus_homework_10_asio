#pragma once

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

namespace opt = boost::program_options;


struct Config {
    bool parse (int argc, const char* argv[]);

    int port        = 9000;
    int blockSize   = 3;
};

bool Config::parse(int argc, const char* argv[]) 
{
    // -- разбор параметров
    opt::options_description desc("All options");
    desc.add_options()
        ("port,p", opt::value<int>()->default_value(9000), "порт")
        ("size,s", opt::value<int>()->default_value(3), "размер блока")
        ("help,h", "справка")
    ;
    opt::variables_map vm;
    opt::store(opt::parse_command_line(argc, argv, desc), vm);
    opt::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    if (vm.count("port"))
         port = vm["port"].as<int>();

    if (vm.count("size"))
        blockSize = vm["size"].as<int>();

    return true;
}

