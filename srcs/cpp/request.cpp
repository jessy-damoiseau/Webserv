#include "../header/request.hpp"

request::request() : _body(""), _status(200), sizeBuffer(0) { memset(buffer, 0, BUFFER_SIZE); }
request::request(const std::string &str) : _body("") , _status(200), _string(str) {
	parse();
	if (_status != 200)
		std::cout << "Error parsing request : " << _status << std::endl;
}
request::request(const request &inst) { *this = inst; }
request::~request() {}

request &request::operator=(const request &inst){ *this = inst; }

size_t								request::getStatus() const { return _status; }
std::string							request::getRequest() const { return _request; }
std::string							request::getMethod() const { return _method; }
std::string 						request::getQuery() const { return _query; }
std::string 						request::getPath() const { return _path; }
std::string							request::getVersion() const { return _version; }
std::string 						request::getBody() const { return _body; }
std::string 						request::getString() const { return _string; }
std::vector<std::string>			request::getLstMethod() const { return _lstMethod; }
std::map<std::string, std::string>	request::getHeader() const { return _header; };

void	request::setStatus(size_t status){ _status = status; }
void	request::setVersion(const std::string &version) { _version = version; }
void	request::setRequest(char *request, int size) { _request.append(request, size); }

void	request::clearHeader() { _header.clear(); }
void	request::initMethod() {
	_lstMethod.push_back("GET");
	_lstMethod.push_back("POST");
	_lstMethod.push_back("DELETE");
}

void	request::parse() {
	std::string line, key, value;
	size_t j, i = 0;

	clearHeader();
	initMethod();
	try {
		parseLine(getNextLine(_request, i));
	}
	catch (const std::exception &e){
		e.what();
		_status = 400;
	}
	while (((line = getNextLine(_request, i)) != "") && _status != 400){
		j = line.find_first_of(':');
		if (j != std::string::npos){
			key = line.substr(0, j);
			key.erase(key.find_last_not_of(" \t") + 1);
			key.erase(0, key.find_first_not_of(" \t"));
			value = line.substr(j + 1, std::string::npos);
			value.erase(value.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			_header[key] = value;
		}
		else
			_status = 400;
		if (_header.empty())
			_status = 400;
		if (_status == 200 && i != std::string::npos)
			_body = _request.substr(i, std::string::npos);
	}

}

void	request::clear() {
	clientSocket = 0;
	sizeBuffer = 0;
	_status = 200;
	_request.clear();
	_method.clear();
	_query.clear();
	_path.clear();
	_version.clear();
	_body.clear();
	_string.clear();
	_lstMethod.clear();
	_header.clear();
}

void	request::parseLine(const std::string &str) {
	size_t i = 0, j = 0;
	std::string line;

	i = str.find_first_of(' ');
	if (i == std::string::npos)
		throw request::ErrorMethodSpace();
	_method.assign(str, 0, i);
	while (_method != _lstMethod[j++] && j < _lstMethod.size());
	if (j >= _lstMethod.size())
		throw request::ErrorMethod();
	line = str.substr(i + 1, std::string::npos);
	i = line.find_first_of(' ');
	if (i == std::string::npos)
		throw request::ErrorPathAndVersion();
	i = line.find_first_of(' ');
	if (i == std::string::npos)
		throw request::ErrorVersion();
	_path.assign(line, 0, i);
	for (size_t k = 0; k < _path.size(); k++)
	{
		if (_path[k] == '%' && _path[k + 1] && _path[k + 2] && _path[k + 1] == '2' && _path[k + 2] == '0')
			_path.replace(k, 3, " ");
	}
	j = _path.find_first_of('?');
	if (j != std::string::npos){
		_query = _path.substr(j + 1, std::string::npos);
		_path = _path.substr(0, j);
	}
	line = line.substr(i + 1, std::string::npos);
	if ((line.compare(0, 8, "HTTP/1.1") == 0 || line.compare(0, 8 , "HTTP/1.0") == 0) && line.size() == 8)
		_version.assign(line, 0, 8);
	else
		throw request::ErrorHttpVersion();
}		

std::string request::getNextLine(std::string &str, size_t &i) {
	std::string ret;
	size_t j;

	if (i == std::string::npos)
		return ("");
	ret = str.substr(i , std::string::npos);
	j = ret.find_first_of('\n');
	ret = ret.substr(0, j);
	if (ret[ret.size() - 1] == '\r')
		ret.resize(ret.size() - 1);
	if (j == std::string::npos)
		i = j;
	else
		i += j + 1;
	return (ret);
}