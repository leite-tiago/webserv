# 🚀 Webserv - HTTP/1.1 Server

**Projeto 42 School**: Implementação de um servidor HTTP em C++98 compatível com HTTP/1.1.

[![C++98](https://img.shields.io/badge/C%2B%2B-98-blue.svg)](https://en.cppreference.com/w/cpp/98)
[![HTTP/1.1](https://img.shields.io/badge/HTTP-1.1-green.svg)](https://tools.ietf.org/html/rfc7230)
[![CGI/1.1](https://img.shields.io/badge/CGI-1.1-orange.svg)](https://tools.ietf.org/html/rfc3875)

## 📋 Índice

- [Características](#-características)
- [Requisitos](#-requisitos)
- [Compilação](#-compilação)
- [Uso](#-uso)
- [Configuração](#-configuração)
- [Testes](#-testes)
- [Arquitetura](#-arquitetura)

---

## ✨ Características

### Core Features
- ✅ **HTTP/1.1** compliant server
- ✅ **Non-blocking I/O** com `fcntl()` e `O_NONBLOCK`
- ✅ **I/O Multiplexing** com `poll()` (apenas 1 instância)
- ✅ **Múltiplas conexões** simultâneas
- ✅ **Virtual Hosts** (server_name support)
- ✅ **Configuração nginx-like**

### HTTP Methods
- ✅ **GET** - Serve static files, directory listing, CGI execution
- ✅ **POST** - Form data, file upload (multipart/form-data), CGI
- ✅ **DELETE** - File deletion with permission checks

### Advanced Features
- ✅ **CGI Execution** (Python, PHP, etc.) com RFC 3875 compliance
- ✅ **File Upload** com parsing de multipart/form-data
- ✅ **Custom Error Pages** configuráveis
- ✅ **Directory Listing** com autoindex
- ✅ **MIME Type Detection**
- ✅ **Request Timeouts**
- ✅ **Graceful Shutdown** (SIGINT/SIGTERM)

---

## 🔧 Requisitos

### Sistema
- **OS**: Linux ou macOS
- **Compiler**: g++ ou clang++ com suporte a C++98
- **Make**: GNU Make

### Dependências
- Python 3 (para testes CGI)
- curl (para testes)
- valgrind (opcional, para memory leak check)

---

## 🔨 Compilação

```bash
# Compilar
make

# Recompilar tudo
make re

# Limpar objetos
make clean

# Limpar tudo
make fclean
```

### Flags de Compilação
- `-Wall -Wextra -Werror` - Warnings como erros
- `-std=c++98` - Standard C++98
- `-I.` - Include path

---

## 🚀 Uso

### Iniciar o Servidor

```bash
# Com configuração personalizada
./webserv config/cgi.conf

# Com configuração padrão
./webserv config/default.conf

# Sem argumentos (usa configuração padrão se existir)
./webserv
```

### Parar o Servidor

```bash
# Ctrl+C no terminal
# ou
killall webserv
```

---

## ⚙️ Configuração

### Estrutura do Ficheiro de Configuração

O servidor usa uma sintaxe similar ao NGINX:

```nginx
server {
    listen 127.0.0.1:8080;
    server_name localhost;
    
    # Tamanho máximo do body (10MB)
    client_max_body_size 10485760;
    
    # Páginas de erro personalizadas
    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;
    
    # Location para ficheiros estáticos
    location / {
        root ./www;
        methods GET POST;
        autoindex on;
        index index.html;
    }
    
    # Location para CGI
    location /cgi-bin {
        root ./cgi-bin;
        methods GET POST;
        cgi_pass /usr/bin/python3;
        cgi_ext .py;
    }
    
    # Location para uploads
    location /upload {
        root ./www;
        methods POST DELETE;
        upload_enable on;
        upload_store ./uploads;
    }
}
```

### Diretivas Suportadas

#### Server Block
- `listen` - Host e porta (ex: `127.0.0.1:8080` ou `8080`)
- `server_name` - Nome(s) do servidor
- `client_max_body_size` - Tamanho máximo do request body (bytes)
- `error_page` - Páginas de erro personalizadas

#### Location Block
- `root` - Diretório raiz para esta location
- `methods` - Métodos HTTP permitidos
- `autoindex` - Directory listing (on/off)
- `index` - Ficheiros index
- `redirect` - HTTP redirect
- `cgi_pass` - Path do executável CGI
- `cgi_ext` - Extensão para CGI (ex: `.py`, `.php`)
- `upload_enable` - Ativar upload (on/off)
- `upload_store` - Diretório para uploads

---

## 🧪 Testes

### Testes Básicos com curl

```bash
# GET de página estática
curl http://localhost:8080/

# GET com query string
curl "http://localhost:8080/test?name=value&foo=bar"

# POST com form data
curl -X POST http://localhost:8080/test \
  -d "name=John&email=john@example.com"

# File upload
curl -X POST http://localhost:8080/upload \
  -F "file=@myfile.txt"

# DELETE
curl -X DELETE http://localhost:8080/upload/myfile.txt

# CGI Python
curl http://localhost:8080/cgi-bin/hello.py

# CGI com POST
curl -X POST http://localhost:8080/cgi-bin/test.py \
  -d "name=Test&message=Hello"
```

### Testes com Browser

Abrir no browser:
- http://localhost:8080/ - Homepage
- http://localhost:8080/cgi_test.html - Testes CGI
- http://localhost:8080/test_form.html - Formulários de teste

### Teste de Conexões Simultâneas

```bash
# 10 conexões simultâneas
for i in {1..10}; do
  curl -s http://localhost:8080/ > /dev/null &
done
wait
```

### Memory Leak Check

```bash
# Com valgrind (Linux)
valgrind --leak-check=full --show-leak-kinds=all \
  ./webserv config/cgi.conf

# Com leaks (macOS)
leaks -atExit -- ./webserv config/cgi.conf
```

### Testes Automatizados

```bash
# Scripts de teste incluídos
cd tests
./run_tests.sh
./stress_tests.sh
```

---

## 🏗️ Arquitetura

### Estrutura de Classes

```
webserv/
├── Config System
│   ├── ConfigParser - Parse nginx-like config
│   ├── Server - Server block configuration
│   └── Route - Location/route configuration
│
├── HTTP Core
│   ├── ServerManager - Main poll() loop
│   ├── Socket - Socket wrapper (non-blocking)
│   ├── Connection - Connection state machine
│   ├── Request - HTTP request parser
│   ├── Response - HTTP response builder
│   └── RequestHandler - Request processing
│
├── CGI
│   └── CGIExecutor - fork + execve + pipes
│
└── Utils
    ├── Logger - Logging system
    ├── Settings - Global settings
    └── Utils - Helper functions
```

### Fluxo de Processamento

```
1. ServerManager::run()
   ↓
2. poll() - Wait for events
   ↓
3. POLLIN on listening socket
   → Accept connection
   → Create Connection object
   ↓
4. POLLIN on client socket
   → Read request
   → Parse HTTP::Request
   ↓
5. Request complete
   → RequestHandler::handle()
   → Generate HTTP::Response
   ↓
6. POLLOUT on client socket
   → Write response
   → Close connection
```

### I/O Multiplexing

- **1 poll()** para todos os file descriptors
- **Non-blocking** sockets com `fcntl()`
- **State machine** em cada Connection
- **Timeout handling** para requests lentos

### CGI Execution

```
1. Detect CGI by extension
   ↓
2. fork() child process
   ↓
3. Setup pipes (stdin/stdout)
   ↓
4. dup2() - Redirect I/O
   ↓
5. chdir() - Change to script dir
   ↓
6. execve() - Execute CGI
   ↓
7. Parent reads output
   ↓
8. waitpid() - Wait for child
   ↓
9. Parse CGI output
   ↓
10. Return HTTP::Response
```

---

## 📊 Conformidade com o Subject

### ✅ Requisitos Obrigatórios

- ✅ Non-blocking I/O em todos os file descriptors
- ✅ Apenas 1 `poll()` (ou equivalente) para todas as operações
- ✅ Nunca fazer `read()` ou `write()` sem passar pelo `poll()`
- ✅ Verificação de `errno` **proibida** após `read()`/`write()`
- ✅ `fork()` **apenas** para CGI
- ✅ Sem crashes em nenhuma circunstância
- ✅ Compatível com browsers (Chrome, Firefox, Safari)
- ✅ Comportamento similar ao NGINX
- ✅ C++98 standard

### ✅ Features Implementadas

- ✅ Serve fully static website
- ✅ GET, POST, DELETE methods
- ✅ File upload (multipart/form-data)
- ✅ CGI execution (Python, PHP, etc.)
- ✅ Multiple ports listening
- ✅ Virtual hosts (server_name)
- ✅ Custom error pages
- ✅ Configuration file
- ✅ Default error pages

---

## 📁 Estrutura de Diretórios

```
webserv/
├── Makefile
├── README.md (este ficheiro)
├── USAGE.md (guia detalhado)
├── webserv.cpp (main)
│
├── includes/
│   ├── webserv.hpp
│   ├── Config.hpp, Server.hpp, Route.hpp
│   ├── ConfigParser.hpp
│   ├── Socket.hpp, Connection.hpp
│   ├── http/
│   │   ├── Request.hpp, Response.hpp
│   │   ├── RequestHandler.hpp
│   │   └── ServerManager.hpp
│   ├── cgi/
│   │   └── CGIExecutor.hpp
│   └── utils/
│       ├── Logger.hpp
│       └── Utils.hpp
│
├── src/
│   ├── Config.cpp, Server.cpp, Route.cpp
│   ├── ConfigParser.cpp
│   ├── Socket.cpp, Connection.cpp
│   ├── ServerManager.cpp
│   ├── http/
│   │   ├── Request.cpp, Response.cpp
│   │   └── RequestHandler.cpp
│   ├── cgi/
│   │   └── CGIExecutor.cpp
│   └── utils/
│       ├── Logger.cpp
│       └── misc.cpp
│
├── config/
│   ├── default.conf
│   ├── simple.conf
│   └── cgi.conf
│
├── www/ (documentroot)
│   ├── index.html
│   ├── about.html
│   ├── cgi_test.html
│   └── test_form.html
│
├── cgi-bin/ (scripts CGI)
│   ├── hello.py
│   └── test.py
│
├── uploads/ (file uploads)
│
└── tests/
    ├── run_tests.sh
    └── stress_tests.sh
```

---

## 👥 Autores

- **tiago** (tiagoleite) - 42 Porto

---

## 📄 Licença

Este projeto faz parte do currículo da 42 School.

---

## 🎯 Status do Projeto

**Progresso**: ~92% completo

### ✅ Completo
- Core HTTP server (poll loop, non-blocking I/O)
- GET, POST, DELETE methods
- File upload (multipart/form-data)
- CGI executor (RFC 3875)
- Configuration parser
- Error handling

### ⚠️ Por Implementar (Opcional)
- Chunked transfer encoding
- Keep-alive connections
- URL decoding avançado
- Directory traversal protection adicional

---

## 📚 Referências

- [RFC 7230 - HTTP/1.1: Message Syntax and Routing](https://tools.ietf.org/html/rfc7230)
- [RFC 7231 - HTTP/1.1: Semantics and Content](https://tools.ietf.org/html/rfc7231)
- [RFC 3875 - CGI Version 1.1](https://tools.ietf.org/html/rfc3875)
- [NGINX Documentation](https://nginx.org/en/docs/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)

---

**Made with ❤️ at 42 Porto** 🚀
