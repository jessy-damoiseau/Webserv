# Webserv

<img src="https://raw.githubusercontent.com/isocpp/logos/master/cpp_logo.png"  width="30" height="30" /> <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/6/61/HTML5_logo_and_wordmark.svg/640px-HTML5_logo_and_wordmark.svg.png"  width="40" height="40" />

## Overview

 - Write a http server in c++98
 - Base file configuration should follow nginx model
 - Server should work as per RFC standard for HTTP/1.1
 - A request to the server should never hang forever
 - Server must be compatible with the web browser of our choice (mozilla and chrome)
 - HTTP response status codes must be accurate
 - Server must have default error pages if none are provided
 - Server must be able to serve a fully static website
 - Clients must be able to upload files (binary files are not managed)
 - GET, POST, and DELETE methods are required
 - Server must stay available at all cost (stress test using siege)
 - Implement multiplexed socket management using either select, poll, epoll or kqueue
