#include <iostream>
#include <exception>

class ErrorSocket : public std::exception {
    public:
        virtual const char * what() const throw() {
            std::cout << "Error: Fail socket" << std::endl;
            perror("Socket");
            return ("");
        }
};

class ErrorSetSockOpt : public std::exception {
    public:
        virtual const char * what() const throw() {
            std::cout << "Error: Fail SetSockOpt" << std::endl;
            perror("SetSockOpt");
            return ("");
        }
};

class ErrorBind : public std::exception {
    public:
        virtual const char * what() const throw() {
            std::cout << "Error: Fail bind" << std::endl;
            perror("Bind");
            return ("");
        }
};

class ErrorListen : public std::exception {
    public:
        virtual const char * what() const throw() {
            std::cout << "Error: Fail listen" << std::endl;
            perror("Listen");
            return ("");
        }
};