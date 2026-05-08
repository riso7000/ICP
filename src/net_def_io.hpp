/**
 * @file net_def_io.hpp
 * @authors xvargas00
 * @brief Provides interface for handling network definition files.
 */

#ifndef NET_DEF_IO_HPP
#define NET_DEF_IO_HPP

#include <string>

int read_netdef(std::string path);
int write_netdef(PetriScene& netdef);

#endif
