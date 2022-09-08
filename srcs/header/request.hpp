#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "webserv.hpp"

class request {

	public:
		request();
		request(const request &inst);
		request(const std::string &str);
		~request();

		request &operator=(request const &inst);

		int		clientSocket;
		
		char	buffer[BUFFER_SIZE + 1];
		int		sizeBuffer;
		
		size_t								getStatus() const;
		std::string							getRequest() const;
		std::string							getMethod() const;
		std::string							getQuery() const;
		std::string							getPath() const;
		std::string							getVersion() const;
		std::string							getBody() const;
		std::string							getString() const;
		std::vector<std::string>			getLstMethod() const;
		std::map<std::string, std::string>	getHeader() const;

		void		setStatus(size_t status);
		void		setVersion(const std::string &version);
		void		setRequest(char *request, int size);

		void		clearHeader();
		void		initMethod();
		void		parse();
		void		clear();
		void		parseLine(const std::string &str);
		std::string	getNextLine(std::string &str, size_t &i);

		class ErrorMethodSpace;
		class ErrorMethod;
		class ErrorPathAndVersion;
		class ErrorVersion;
		class ErrorHttpVersion;

	private:
		size_t		_status;
		std::string _request;
		
		std::string _method;
		std::string _query;
		std::string _path;
		std::string _version;
		std::string _body;
		std::string _string;

		std::vector<std::string> _lstMethod;
		std::map<std::string, std::string> _header;


};

class request::ErrorMethodSpace: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Method Space : no space after method" << std::endl;
            return ("");
        }
};
class request::ErrorMethod: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Method : method invalid" << std::endl;
            return ("");
        }
};
class request::ErrorPathAndVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Path / Verions : no path/version" << std::endl;
            return ("");
        }
};
class request::ErrorVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error Version : no version" << std::endl;
            return ("");
        }
};
class request::ErrorHttpVersion: public std::exception {
	 public:
        virtual const char * what() const throw() {
            std::cerr << "Error HTTP Version : bad HTTP version" << std::endl;
            return ("");
        }
};





#endif
