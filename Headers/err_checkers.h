#pragma once

#ifndef ERRCHECKERS_H
#define ERRCHECKERS_H

#include <string>

#define CHECK_HANDLE_CREATION(retCode)                                                                                 \
    {                                                                                                                  \
        if (retCode == CM_FILE_DOES_NOT_EXIST) {                                                                       \
            std::cout << "Activation key does not exist. Please run the post installation script found"                \
                      << " in $CUBEMOS_SKEL_SDK / scripts to activate the license and "                                \
                      << " use it in your application." << std::endl;                                                  \
            CHECK_SUCCESS(retCode);                                                                                    \
        }                                                                                                              \
    }

#define CHECK_SUCCESS(retCode)                                                                                         \
    {                                                                                                                  \
        if (retCode != CM_SUCCESS) {                                                                                   \
            std::cerr << "Operation in file \"" << __FILE__ << "\" at line \"" << __LINE__                             \
                      << "\" failed with return code:  " << retCode << std::endl                                       \
                      << "Press any key to exit.." << std::endl;                                                       \
                                                                                                                       \
            std::cin.get();                                                                                            \
            return -1;                                                                                                 \
        }                                                                                                              \
    }

#define EXIT_PROGRAM(errMsg)                                                                                           \
    {                                                                                                                  \
        std::cerr << errMsg;                                                                                           \
        std::cin.get();                                                                                                \
        return -1;                                                                                                     \
    }

#endif 
