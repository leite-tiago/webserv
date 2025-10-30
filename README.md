# Webserv

> *"This is when you finally understand why URLs start with HTTP"*

A custom HTTP/1.1 web server implementation written in C++98, capable of serving static websites, handling file uploads, processing CGI scripts, and managing multiple virtual hosts.

[![42 Project](https://img.shields.io/badge/42-Lisboa-blue)](https://www.42lisboa.com/)
[![C++98](https://img.shields.io/badge/C++-98-blue.svg)](https://en.cppreference.com/w/cpp/98)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## 📋 Table of Contents

- [About](#-about)
- [Features](#-features)
- [Requirements](#-requirements)
- [Installation](#-installation)
- [Usage](#-usage)
- [Configuration](#-configuration)
- [Testing](#-testing)
- [Project Structure](#-project-structure)
- [Implementation Details](#-implementation-details)
- [Examples](#-examples)
- [Troubleshooting](#-troubleshooting)
- [Resources](#-resources)
- [Authors](#-authors)

## 🌐 About

Webserv is a 42 School project that challenges students to build an HTTP server from scratch. The server implements core HTTP/1.1 functionality, including:

- Non-blocking I/O using `poll()` (or equivalent)
- Multiple virtual hosts and port binding
- Static file serving with directory listing
- File upload and deletion
- CGI execution (Python, PHP, etc.)
- Custom error pages
- HTTP redirections

This project provides deep insights into how web servers work, network programming, and the HTTP protocol.

## ✨ Features

### Core HTTP Functionality
- ✅ **HTTP/1.1 Protocol** - Full implementation of GET, POST, and DELETE methods
- ✅ **Non-blocking I/O** - Uses `poll()`/`epoll`/`kqueue` for efficient connection handling
- ✅ **Persistent Connections** - Keep-alive support
- ✅ **Chunked Transfer Encoding** - Handles chunked requests and responses

### Server Configuration
- ✅ **Multiple Virtual Hosts** - Host multiple websites on different domains/ports
- ✅ **Multiple Ports** - Listen on multiple ports simultaneously
- ✅ **Custom Error Pages** - Define custom 404, 500, etc. error pages
- ✅ **Request Size Limits** - Configure maximum client body size
- ✅ **Directory Listing** - Optional autoindex feature

### Routing & File Handling
- ✅ **Static File Serving** - Serve HTML, CSS, JS, images, and other static content
- ✅ **File Uploads** - Handle multipart/form-data file uploads
- ✅ **File Deletion** - DELETE method support
- ✅ **HTTP Redirections** - Configure URL redirects
- ✅ **Index Files** - Automatic serving of index.html

### CGI Support
- ✅ **CGI Execution** - Execute Python, PHP, and other CGI scripts
- ✅ **Environment Variables** - Full CGI environment setup
- ✅ **POST/GET Support** - Handle form submissions via CGI

### Browser Compatibility
- ✅ **Modern Browsers** - Compatible with Chrome, Firefox, Safari, etc.
- ✅ **NGINX-like Behavior** - Configuration and behavior inspired by NGINX

## 📦 Requirements

### System Requirements
- **OS**: Linux, macOS, or other UNIX-like system
- **Compiler**: g++ or clang++ with C++98 support
- **Make**: GNU Make

### Optional (for CGI)
- Python 3.x (for Python CGI scripts)
- PHP-CGI (for PHP CGI scripts)

## 🚀 Installation

1. **Clone the repository**
```bash
git clone https://github.com/leite-tiago/webserv.git
cd webserv
```

2. **Compile the project**
```bash
make
```

This will create the `webserv` executable.

3. **Clean build files** (optional)
```bash
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Rebuild from scratch
```

## 💻 Usage

### Basic Usage

Run the server with the default configuration:
```bash
./webserv
```

Run with a custom configuration file:
```bash
./webserv config/default.conf
```

### Accessing the Server

Once running, open your browser and navigate to:
```
http://localhost:8080
```

### Testing with curl

```bash
# GET request
curl http://localhost:8080/

# POST request with file upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# DELETE request
curl -X DELETE http://localhost:8080/uploads/test.txt
```

### Stopping the Server

Press `Ctrl+C` to gracefully stop the server.

## ⚙️ Configuration

The server uses a configuration file with NGINX-like syntax. See `config/default.conf` for a complete example.

### Basic Server Block

```nginx
server {
    listen 8080;
    host 127.0.0.1;
    server_name localhost;

    client_max_body_size 10M;

    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;

    location / {
        root ./www;
        index index.html;
        allow_methods GET POST;
        autoindex off;
    }
}
```

### Configuration Directives

#### Server Context

| Directive | Description | Example |
|-----------|-------------|---------|
| `listen` | Port to listen on | `listen 8080;` |
| `host` | IP address to bind to | `host 127.0.0.1;` |
| `server_name` | Virtual host names | `server_name localhost example.com;` |
| `client_max_body_size` | Maximum request body size | `client_max_body_size 10M;` |
| `error_page` | Custom error pages | `error_page 404 /404.html;` |

#### Location Context

| Directive | Description | Example |
|-----------|-------------|---------|
| `root` | Root directory for requests | `root ./www;` |
| `index` | Default index file | `index index.html;` |
| `allow_methods` | Allowed HTTP methods | `allow_methods GET POST DELETE;` |
| `autoindex` | Directory listing | `autoindex on;` |
| `return` | HTTP redirect | `return http://example.com;` |
| `upload_enable` | Enable file uploads | `upload_enable on;` |
| `upload_path` | Upload directory | `upload_path ./uploads;` |
| `cgi_pass` | CGI interpreter path | `cgi_pass /usr/bin/python3;` |
| `cgi_ext` | CGI file extension | `cgi_ext .py;` |

### Multiple Servers Example

```nginx
# Main website
server {
    listen 8080;
    host 127.0.0.1;
    server_name localhost;

    location / {
        root ./www;
        index index.html;
        allow_methods GET POST;
    }
}

# API server
server {
    listen 8081;
    host 127.0.0.1;
    server_name api.local;

    location / {
        root ./api;
        allow_methods GET POST DELETE;
    }
}
```

## 🧪 Testing

The project includes several testing utilities in the `tests/` directory.

### Quick Start Tests

```bash
cd tests
./run_tests.sh
```

### Feature Tests

Test specific features:
```bash
./test_feature.sh GET
./test_feature.sh POST
./test_feature.sh DELETE
./test_feature.sh CGI
```

### Stress Tests

Test server stability under load:
```bash
./stress_tests.sh
```

### Manual Testing with curl

See `tests/CURL_EXAMPLES.md` for detailed curl examples.

### Browser Testing

1. Start the server: `./webserv`
2. Open `http://localhost:8080/test_form.html`
3. Test file uploads, form submissions, etc.

## 📁 Project Structure

```
webserv/
├── includes/           # Header files
│   ├── webserv.hpp    # Main header with includes
│   ├── config/        # Configuration parsing
│   ├── core/          # Core server components
│   ├── http/          # HTTP protocol implementation
│   ├── network/       # Socket and connection handling
│   ├── cgi/           # CGI execution
│   └── utils/         # Utilities (logging, etc.)
├── src/               # Source files
│   ├── webserv.cpp    # Main entry point
│   ├── config/        # Configuration implementation
│   ├── core/          # Core implementation
│   ├── http/          # HTTP implementation
│   ├── network/       # Network implementation
│   ├── cgi/           # CGI implementation
│   └── utils/         # Utilities implementation
├── config/            # Configuration files
│   ├── default.conf   # Default server configuration
│   └── cgi.conf       # CGI-focused configuration
├── www/               # Web root directory
│   ├── index.html     # Main page
│   ├── errors/        # Error pages
│   └── cgi-bin/       # CGI scripts
├── tests/             # Testing utilities
├── uploads/           # Upload directory
├── Makefile           # Build configuration
└── README.md          # This file
```

## 🔧 Implementation Details

### Architecture

The server follows a modular architecture:

1. **Core Layer** (`core/`)
   - `Instance`: Main server instance and event loop
   - `Settings`: Global server settings

2. **Network Layer** (`network/`)
   - `Socket`: Socket creation and binding
   - `Connection`: Client connection management

3. **HTTP Layer** (`http/`)
   - `ServerManager`: Manages multiple virtual servers
   - `Request`: HTTP request parsing
   - `Response`: HTTP response generation
   - `RequestHandler`: Routes requests to appropriate handlers

4. **Configuration Layer** (`config/`)
   - `ConfigParser`: Parses configuration files
   - `Config`: Global configuration
   - `Server`: Server block configuration
   - `Route`: Location block configuration

5. **CGI Layer** (`cgi/`)
   - `CGIExecutor`: Executes CGI scripts with proper environment

### Key Technical Decisions

- **Non-blocking I/O**: All file descriptors are set to non-blocking mode
- **poll() Event Loop**: Single `poll()` call handles all I/O operations
- **State Machine**: Connection handling uses state machines for request/response processing
- **Memory Management**: RAII principles, no memory leaks
- **Error Handling**: Comprehensive error handling without relying on `errno` after read/write

### HTTP Methods Implementation

#### GET
- Serves static files from configured root directory
- Supports range requests
- Directory listing when autoindex is enabled

#### POST
- Handles multipart/form-data for file uploads
- Processes application/x-www-form-urlencoded forms
- Supports chunked transfer encoding
- CGI execution for dynamic content

#### DELETE
- Removes files from configured upload directories
- Returns appropriate status codes

## 📚 Examples

### Example 1: Simple Static Website

1. Create your HTML files in `www/`
2. Configure the server:
```nginx
server {
    listen 8080;
    location / {
        root ./www;
        index index.html;
        allow_methods GET;
    }
}
```
3. Run: `./webserv config.conf`

### Example 2: File Upload Service

1. Create upload directory: `mkdir -p www/uploads`
2. Configure:
```nginx
location /upload {
    root ./www/uploads;
    allow_methods GET POST DELETE;
    upload_enable on;
    upload_path ./www/uploads;
    autoindex on;
}
```
3. Access: `http://localhost:8080/test_form.html`

### Example 3: Python CGI

1. Create a Python script in `www/cgi-bin/hello.py`:
```python
#!/usr/bin/env python3
print("Content-Type: text/html")
print()
print("<h1>Hello from Python CGI!</h1>")
```

2. Make it executable: `chmod +x www/cgi-bin/hello.py`

3. Configure:
```nginx
location /cgi-bin {
    root ./www/cgi-bin;
    allow_methods GET POST;
    cgi_pass /usr/bin/python3;
    cgi_ext .py;
}
```

4. Access: `http://localhost:8080/cgi-bin/hello.py`

## 🐛 Troubleshooting

### Common Issues

**Server won't start**
- Check if the port is already in use: `lsof -i :8080`
- Verify configuration file syntax
- Ensure you have permissions to bind to the port

**404 Not Found**
- Check the `root` directive in your configuration
- Verify file paths are correct and files exist
- Check file permissions

**500 Internal Server Error**
- Check server logs for detailed error messages
- Verify CGI script has execute permissions
- Ensure CGI interpreter path is correct

**File Upload Fails**
- Check `client_max_body_size` setting
- Verify upload directory exists and is writable
- Ensure `upload_enable on` is set

**CGI Doesn't Work**
- Verify CGI script has execute permissions: `chmod +x script.py`
- Check CGI interpreter path: `which python3`
- Ensure CGI script has proper shebang line
- Check script output format (headers + blank line + content)

### Debug Mode

For detailed logging, check the console output when running the server.

## 📖 Resources

### HTTP Protocol
- [RFC 7230 - HTTP/1.1 Message Syntax and Routing](https://tools.ietf.org/html/rfc7230)
- [RFC 7231 - HTTP/1.1 Semantics and Content](https://tools.ietf.org/html/rfc7231)
- [MDN HTTP Documentation](https://developer.mozilla.org/en-US/docs/Web/HTTP)

### CGI
- [RFC 3875 - The CGI Specification](https://tools.ietf.org/html/rfc3875)
- [CGI Programming Tutorial](https://www.tutorialspoint.com/python/python_cgi_programming.htm)

### Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html)

### Reference Implementations
- [NGINX Documentation](https://nginx.org/en/docs/)
- [Apache HTTP Server](https://httpd.apache.org/docs/)

## 👥 Authors

- **Tiago Leite** - [@leite-tiago](https://github.com/leite-tiago)

## 📝 License

This project is part of the 42 School curriculum. Feel free to use it for educational purposes.

---

**Made with ❤️ at 42 Lisboa**
