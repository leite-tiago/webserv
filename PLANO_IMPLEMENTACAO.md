# 📋 PLANO DE IMPLEMENTAÇÃO - WEBSERV

> Projeto 42: Implementação de um servidor HTTP em C++98

---

## ✅ JÁ IMPLEMENTADO

- [x] Estrutura básica do projeto (Makefile, diretórios)
- [x] Padrão Singleton (Instance.hpp)
- [x] Sistema de Settings (carrega YAML)
- [x] Logger básico
- [x] Parser YAML (Yaml.hpp/cpp)
- [x] Utilitários básicos (Utils)

---

## 🚀 FASE 1: CONFIGURAÇÃO DO SERVIDOR

### Parser do Ficheiro de Configuração (nginx-like)

- [ ] Implementar ConfigParser class
- [ ] Parsear múltiplos server blocks
- [ ] Configuração de portas e hosts
- [ ] Configuração de server_names
- [ ] Error pages personalizadas
- [ ] Client body size limit (max_body_size)
- [ ] Default server por host:port

### Estrutura de Routes/Location

- [ ] Classe Route/Location
- [ ] Métodos HTTP permitidos (GET, POST, DELETE)
- [ ] HTTP redirects (301, 302)
- [ ] Root directory configuration
- [ ] Directory listing (on/off)
- [ ] Index files (default files)
- [ ] CGI configuration por extensão
- [ ] Upload directory configuration

---

## 🔥 FASE 2: CORE DO SERVIDOR HTTP (CRÍTICO)

### Socket Management

- [ ] Criar sockets com `socket()`
- [ ] Bind a portas/hosts com `bind()`
- [ ] Colocar em listening mode com `listen()`
- [ ] Aceitar conexões com `accept()`
- [ ] Configurar socket options com `setsockopt()` (SO_REUSEADDR)
- [ ] Tornar sockets NON-BLOCKING com `fcntl()` (F_SETFL, O_NONBLOCK)
- [ ] Suporte a múltiplas portas simultaneamente

### I/O Multiplexing ⚠️ CRÍTICO

- [ ] Escolher entre poll(), select(), kqueue() ou epoll()
- [ ] Implementar loop principal com apenas 1 poll()
- [ ] Monitorizar LEITURA e ESCRITA simultaneamente
- [ ] NUNCA fazer read/write sem passar pelo poll()
- [ ] Gestão de timeouts (requests não podem travar indefinidamente)
- [ ] Handle POLLIN (dados para ler)
- [ ] Handle POLLOUT (pronto para escrever)
- [ ] Handle POLLHUP/POLLERR (erros/disconnect)

### Connection Management

- [ ] Classe Connection para gerir cada cliente
- [ ] Accept de novas conexões
- [ ] Gestão de múltiplas conexões simultâneas
- [ ] State machine (READING_REQUEST, WRITING_RESPONSE, etc)
- [ ] Deteção de disconnect de clientes
- [ ] Cleanup de recursos (close de FDs)
- [ ] Buffer management (partial reads/writes)

---

## 📡 FASE 3: HTTP PROTOCOL

### HTTP Request Parser

- [ ] Classe HTTP::Request
- [ ] Parse da request line (método, URI, versão HTTP)
- [ ] Parse de headers (chave: valor)
- [ ] Parse de body (Content-Length)
- [ ] Parse de chunked transfer encoding
- [ ] Validação de requests
- [ ] Limites de tamanho (URI max, headers max)
- [ ] Handle de partial requests (buffers)

### HTTP Response Builder

- [ ] Classe HTTP::Response
- [ ] Construir status line (HTTP/1.1 200 OK)
- [ ] Adicionar headers (Content-Type, Content-Length, Date, Server)
- [ ] Adicionar body
- [ ] Chunked transfer encoding para CGI sem Content-Length
- [ ] Keep-Alive vs Connection: close

### HTTP Methods Implementation

#### GET Method
- [ ] Servir ficheiros estáticos
- [ ] Handle de queries (query string)
- [ ] Cache headers (Last-Modified, ETag)

#### POST Method
- [ ] Receber dados no body
- [ ] Handle de forms (application/x-www-form-urlencoded)
- [ ] Upload de ficheiros (multipart/form-data)
- [ ] Passar dados ao CGI

#### DELETE Method
- [ ] Apagar ficheiros
- [ ] Verificar permissões
- [ ] Return 204 No Content ou 200 OK

### HTTP Status Codes (expandir Settings.cpp)

- [ ] 2xx Success (200 OK, 201 Created, 204 No Content)
- [ ] 3xx Redirects (301 Moved Permanently, 302 Found, 304 Not Modified)
- [ ] 4xx Client Errors (400, 403, 404, 405, 413, 414)
- [ ] 5xx Server Errors (500, 501, 502, 503, 504, 505)

---

## 📁 FASE 4: FILE SERVING (STATIC WEBSITE)

### Static File Handler

- [ ] Classe FileHandler
- [ ] Servir ficheiros de um diretório root
- [ ] Aplicar MIME types (usar httpMimeType())
- [ ] File permissions check com `access()`
- [ ] File stats com `stat()`
- [ ] Ler ficheiros com `open()` e `read()`
- [ ] Return 404 se ficheiro não existir
- [ ] Return 403 se sem permissões

### Directory Handling

- [ ] Directory listing com `opendir()`, `readdir()`, `closedir()`
- [ ] Gerar HTML para listagem de diretórios
- [ ] Servir index files (index.html, index.htm)
- [ ] Desativar directory listing se configurado

### Path Resolution

- [ ] Resolver paths relativos
- [ ] Combinar root + URI path
- [ ] Prevenir directory traversal (../)
- [ ] URL decoding (%20 → espaço, etc)
- [ ] Handle de trailing slashes

---

## 🔧 FASE 5: CGI (COMMON GATEWAY INTERFACE)

### CGI Executor

- [ ] Classe CGI::Executor
- [ ] Detetar se request precisa de CGI (baseado em extensão)
- [ ] `fork()` para criar processo filho
- [ ] `pipe()` para criar pipes (stdin/stdout)
- [ ] `dup2()` para redirecionar stdin/stdout
- [ ] `execve()` para executar CGI (php-cgi, python, etc)
- [ ] `waitpid()` para esperar pelo processo
- [ ] Handle de timeout de CGI (kill processo se demorar muito)
- [ ] `chdir()` para mudar para diretório correto

### CGI Environment Variables

- [ ] REQUEST_METHOD
- [ ] QUERY_STRING
- [ ] PATH_INFO (full path do script)
- [ ] SCRIPT_NAME
- [ ] CONTENT_TYPE
- [ ] CONTENT_LENGTH
- [ ] SERVER_NAME
- [ ] SERVER_PORT
- [ ] SERVER_PROTOCOL
- [ ] HTTP_* headers (converter headers para env vars)

### CGI I/O Handling

- [ ] Enviar request body para stdin do CGI
- [ ] Handle de chunked requests (unchunk antes de enviar)
- [ ] Ler response do stdout do CGI
- [ ] Parse de CGI headers (Status, Content-Type, etc)
- [ ] Handle CGI output sem Content-Length (ler até EOF)
- [ ] Handle de partial reads/writes
- [ ] Non-blocking I/O com o CGI

---

## 📤 FASE 6: FILE UPLOAD

### Upload Handler

- [ ] Parse de Content-Type: multipart/form-data
- [ ] Extrair boundary do Content-Type
- [ ] Parse de partes multipart
- [ ] Parse de Content-Disposition header (filename)
- [ ] Guardar ficheiros no diretório configurado
- [ ] Gerar nomes de ficheiros únicos (timestamp + random)
- [ ] Validação de tamanho (max body size)
- [ ] Return 201 Created com localização do ficheiro
- [ ] Handle de múltiplos ficheiros

---

## ❌ FASE 7: ERROR HANDLING

### Error Pages

- [ ] Default error pages (HTML básico)
- [ ] Custom error pages (configuráveis por código)
- [ ] Templates de error pages
- [ ] Substituição de variáveis (%code%, %message%)

### Error Management

- [ ] SEM CRASHES em NENHUMA circunstância
- [ ] Try-catch para todas as operações críticas
- [ ] Memory leak check (valgrind, leaks)
- [ ] Handle de EWOULDBLOCK/EAGAIN (non-blocking)
- [ ] Handle de EPIPE (broken pipe)
- [ ] Handle de SIGPIPE (ignorar com signal())
- [ ] NUNCA verificar errno depois de read/write
- [ ] Logging de erros
- [ ] Graceful shutdown

---

## 🌐 FASE 8: VIRTUAL HOSTS

### Multi-Server Support

- [ ] Múltiplos servers na configuração
- [ ] Mesmo host:port com diferentes server_names
- [ ] Default server para cada host:port
- [ ] Routing baseado no header Host
- [ ] Fallback para default server
- [ ] Testing com diferentes Host headers

---

## 🏗️ ESTRUTURA DE CLASSES SUGERIDA

```
classes/
├── Server.hpp/cpp
│   └── Configuração de um server block
│
├── VirtualHost.hpp/cpp
│   └── Server + routes + socket
│
├── Route.hpp/cpp
│   └── Configuração de uma location/route
│
├── Connection.hpp/cpp
│   ├── Socket fd
│   ├── Request
│   ├── Response
│   └── State machine
│
├── HTTP/
│   ├── Request.hpp/cpp
│   │   ├── Method, URI, Version
│   │   ├── Headers (map)
│   │   └── Body (buffer)
│   │
│   ├── Response.hpp/cpp
│   │   ├── Status code
│   │   ├── Headers
│   │   └── Body
│   │
│   └── Parser.hpp/cpp
│       └── Parse requests
│
├── CGI/
│   └── Executor.hpp/cpp
│       ├── Environment setup
│       ├── Process management
│       └── I/O pipes
│
├── FileHandler.hpp/cpp
│   ├── Static file serving
│   ├── Directory listing
│   └── Upload handling
│
├── ConfigParser.hpp/cpp
│   └── Parse nginx-like config
│
└── ServerManager.hpp/cpp (já existe)
    └── Main server loop
```

---

## 📅 ORDEM DE IMPLEMENTAÇÃO RECOMENDADA

1. **ConfigParser** - Ler e parsear ficheiro de configuração
2. **Socket Management** - Criar e configurar sockets (non-blocking)
3. **Poll Loop** - Loop principal com poll/select/kqueue
4. **HTTP Request Parser** - Parsear requests HTTP
5. **HTTP Response Builder** - Construir responses
6. **Static File Handler** - Servir ficheiros estáticos (GET)
7. **Error Pages** - Páginas de erro
8. **POST Handler** - Receber dados
9. **File Upload** - Upload de ficheiros
10. **DELETE Handler** - Apagar ficheiros
11. **CGI Executor** - Executar CGI scripts
12. **Virtual Hosts** - Suporte a múltiplos servers
13. **Testing & Stress Testing** - Garantir robustez

---

## ⚠️ PONTOS CRÍTICOS DO ENUNCIADO

> **ATENÇÃO:** Violar estes pontos = NOTA 0

- ⛔ **NON-BLOCKING OBRIGATÓRIO** - Todos os FDs não-bloqueantes
- ⛔ **1 POLL() APENAS** - Para todas as operações I/O
- ⛔ **NUNCA READ/WRITE SEM POLL()** - Consumir recursos = nota 0
- ⛔ **SEM CRASHES** - Em NENHUMA circunstância (out of memory, etc)
- ⛔ **SEM FORK** - Exceto para CGI
- ⛔ **Compatível com browsers normais** - Chrome, Firefox, Safari
- ⛔ **Comparar comportamento com NGINX** - Usar como referência
- ⛔ **errno PROIBIDO** - Depois de read/write
- ⛔ **C++98 STANDARD** - Compilar com -std=c++98

---

## 🧪 CHECKLIST DE TESTES

### Testes Básicos

- [ ] Compilar sem warnings (Wall, Wextra, Werror)
- [ ] Verificar memory leaks (valgrind/leaks)
- [ ] Testar com ficheiro de config válido
- [ ] Testar com ficheiro de config inválido
- [ ] Testar sem ficheiro de config (default)

### Testes de Funcionalidade

- [ ] Servir página HTML estática
- [ ] Servir CSS, JS, imagens
- [ ] Testar GET com browser (Chrome, Firefox)
- [ ] Testar POST com form HTML
- [ ] Upload de ficheiro pequeno
- [ ] Upload de ficheiro grande (>1MB)
- [ ] DELETE de ficheiro
- [ ] Directory listing
- [ ] Index file serving
- [ ] Error pages (404, 403, 500)
- [ ] CGI com PHP
- [ ] CGI com Python
- [ ] Redirects (301, 302)

### Testes com Ferramentas

- [ ] Testar com telnet
- [ ] Testar com curl
- [ ] Testar com siege (stress test)
- [ ] Testar com ab (Apache Bench)

### Testes de Stress

- [ ] 100 conexões simultâneas
- [ ] 1000 conexões simultâneas
- [ ] Requests muito grandes (perto do limite)
- [ ] Requests inválidos (malformed)
- [ ] Timeout de requests lentos
- [ ] Kill process de CGI (timeout)

### Testes de Comparação com NGINX

- [ ] Comparar headers de response
- [ ] Comparar status codes
- [ ] Comparar comportamento de error pages
- [ ] Comparar directory listing
- [ ] Comparar CGI execution

### Testes de Configuração

- [ ] Múltiplos servers
- [ ] Múltiplas portas
- [ ] Virtual hosts (server_name)
- [ ] Diferentes routes no mesmo server
- [ ] Error pages customizadas
- [ ] Body size limits

---

## 📚 RECURSOS ÚTEIS

### RFCs (Especificações HTTP)

- RFC 7230 - HTTP/1.1: Message Syntax and Routing
- RFC 7231 - HTTP/1.1: Semantics and Content
- RFC 7232 - HTTP/1.1: Conditional Requests
- RFC 7233 - HTTP/1.1: Range Requests
- RFC 7234 - HTTP/1.1: Caching
- RFC 7235 - HTTP/1.1: Authentication
- RFC 3875 - CGI Version 1.1

### Tutoriais

- Beej's Guide to Network Programming
- NGINX configuration documentation
- HTTP Made Really Easy

### Ferramentas

- telnet - testar requests manuais
- curl - testar requests HTTP
- siege - stress testing
- Apache Bench (ab) - benchmarking
- Postman - testar API
- valgrind - memory leaks
- netstat/lsof - verificar sockets abertos

---

## 🎯 MILESTONES

### Milestone 1: Hello World Server ✅
- [ ] Servidor aceita conexões
- [ ] Responde "Hello World" a qualquer request
- [ ] Non-blocking I/O funcional
- [ ] Poll loop básico

### Milestone 2: Static File Server 🎯
- [ ] Servir ficheiros HTML, CSS, JS
- [ ] Error pages funcionais
- [ ] Directory listing
- [ ] Configuração básica

### Milestone 3: Full HTTP Server 🚀
- [ ] GET, POST, DELETE
- [ ] File upload
- [ ] Error handling robusto
- [ ] Virtual hosts

### Milestone 4: CGI Support 🔧
- [ ] CGI execution (PHP/Python)
- [ ] Environment variables corretas
- [ ] I/O handling
- [ ] Timeout handling

### Milestone 5: Production Ready 🏆
- [ ] Todos os testes passam
- [ ] Zero memory leaks
- [ ] Zero crashes
- [ ] Comparável ao NGINX
- [ ] Stress tests passam

---

## 📝 NOTAS DE DESENVOLVIMENTO

### Boas Práticas

- Commits pequenos e frequentes
- Testar cada feature antes de avançar
- Code review entre membros da equipa
- Documentar código complexo
- Usar const sempre que possível
- RAII para gestão de recursos
- Evitar raw pointers (usar smart pointers se possível em C++98)

### Debugging

- Usar Logger para debug messages
- Testar com telnet para ver requests/responses
- Usar gdb/lldb para debug
- Adicionar assertions
- Testar edge cases

### Performance

- Evitar cópias desnecessárias
- Buffers de tamanho adequado
- Reuse de buffers
- Minimize syscalls
- Profile com valgrind --tool=callgrind

---

## ✅ DEFINITION OF DONE

Uma feature está completa quando:

- [ ] Código compila sem warnings
- [ ] Código testado manualmente
- [ ] Código testado com automated tests
- [ ] Zero memory leaks
- [ ] Code review feito
- [ ] Documentação atualizada
- [ ] Comparado com NGINX (se aplicável)

---

**Última atualização:** 14 Outubro 2025
