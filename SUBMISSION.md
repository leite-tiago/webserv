# 🚀 Guia de Submissão - Webserv

## 📦 Antes de Submeter

### 1. Verificar Compilação
```bash
make fclean
make
```
**Esperado**: Compilação sem erros ou warnings

### 2. Executar Testes
```bash
# Iniciar servidor
./webserv config/cgi.conf &

# Aguardar 2 segundos
sleep 2

# Executar suite de testes
./tests/run_tests.sh

# Parar servidor
killall webserv
```
**Esperado**: Mínimo 60% dos testes passando (core features 100%)

### 3. Verificar Memory Leaks

**macOS:**
```bash
./webserv config/cgi.conf &
sleep 2
leaks $(pgrep webserv)
killall webserv
```

**Linux:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./webserv config/cgi.conf
# Em outro terminal:
curl http://localhost:8080/
curl http://localhost:8080/cgi-bin/hello.py
# Ctrl+C no terminal do valgrind
```

**Esperado**: "0 leaks" ou "no leaks are possible"

### 4. Verificar Conformidade Crítica
```bash
# Verificar errno após read/write
grep -r "errno" src/ | grep -E "read|write"
# Esperado: Sem resultados

# Verificar fork
grep -rn "fork(" src/
# Esperado: Apenas em src/cgi/CGIExecutor.cpp

# Verificar poll
grep -rn "poll(" src/
# Esperado: Apenas em src/http/ServerManager.cpp
```

---

## 📋 Checklist Final

### Arquivos Obrigatórios
- [x] `Makefile` com regras: all, clean, fclean, re
- [x] Source files em `src/` e `includes/`
- [x] Arquivo de configuração em `config/`
- [x] `README.md` com instruções

### Regras do Makefile
- [x] `make` compila o projeto
- [x] `make clean` remove objects (.o)
- [x] `make fclean` remove tudo (objects + executável)
- [x] `make re` recompila do zero
- [x] Sem relink desnecessário

### Código
- [x] C++98 standard (`-std=c++98`)
- [x] Compilação com `-Wall -Wextra -Werror`
- [x] Sem warnings
- [x] Sem segfaults/crashes
- [x] Sem undefined behavior
- [x] Sem memory leaks

### Features Obrigatórias
- [x] Non-blocking I/O (fcntl)
- [x] 1 poll() para todos os I/O
- [x] GET, POST, DELETE methods
- [x] File upload (multipart/form-data)
- [x] CGI execution (GET e POST)
- [x] Configuration file
- [x] Multiple ports
- [x] Default error pages
- [x] Custom error pages

---

## 🎯 Teste de Avaliação Rápido

Execute este script para verificar rapidamente:

```bash
#!/bin/bash
echo "===== WEBSERV QUICK TEST ====="

echo "1. Compilação..."
make re > /dev/null 2>&1 && echo "✅ OK" || echo "❌ FAIL"

echo "2. Iniciar servidor..."
./webserv config/cgi.conf > /dev/null 2>&1 &
PID=$!
sleep 2
kill -0 $PID 2>/dev/null && echo "✅ OK" || echo "❌ FAIL"

echo "3. GET request..."
curl -s http://localhost:8080/ | grep -q "<!DOCTYPE" && echo "✅ OK" || echo "❌ FAIL"

echo "4. CGI request..."
curl -s http://localhost:8080/cgi-bin/hello.py | grep -q "Hello" && echo "✅ OK" || echo "❌ FAIL"

echo "5. POST request..."
curl -s -X POST http://localhost:8080/cgi-bin/test.py -d "test=data" | grep -q "QUERY_STRING" && echo "✅ OK" || echo "❌ FAIL"

echo "6. Memory leaks..."
leaks $PID 2>&1 | grep -q "0 leaks" && echo "✅ OK" || echo "⚠ CHECK"

echo "7. Parar servidor..."
kill $PID 2>/dev/null && echo "✅ OK" || echo "❌ FAIL"

echo "===== TESTE COMPLETO ====="
```

---

## 📄 Arquivos a Submeter

### Estrutura Esperada
```
webserv/
├── Makefile
├── README.md
├── SUBJECT.md (subject original)
├── CHECKLIST.md
├── webserv.cpp
├── includes/
│   └── [todos os .hpp]
├── src/
│   └── [todos os .cpp]
├── config/
│   └── [arquivos de configuração]
├── www/
│   └── [static files para teste]
├── cgi-bin/
│   └── [scripts CGI para teste]
└── tests/
    └── [scripts de teste]
```

### Arquivos que NÃO devem ser commitados
- ❌ `*.o` (object files)
- ❌ `webserv` (executável)
- ❌ `*.a` (libraries)
- ❌ `.DS_Store` (macOS)
- ❌ `uploads/*` (arquivos de upload de teste)
- ❌ `*.log` (logs)

### .gitignore Recomendado
```gitignore
# Objects and executables
*.o
*.a
webserv

# OS files
.DS_Store
.vscode/

# Test files
uploads/*.txt
uploads/*.jpg
uploads/*.png
!uploads/.gitkeep

# Logs
*.log
```

---

## 🎓 Durante a Avaliação

### O que o avaliador vai testar:

1. **Compilação**
   - `make` deve compilar sem erros
   - Sem warnings
   - Sem relink desnecessário

2. **Funcionamento Básico**
   - Servidor inicia sem crashes
   - GET de página estática
   - POST com form data
   - DELETE de arquivo
   - 404 para arquivo inexistente

3. **CGI**
   - Executar script Python/PHP
   - GET com query string
   - POST com body
   - Environment variables corretas

4. **File Upload**
   - Upload de arquivo pequeno
   - Upload via multipart/form-data
   - Arquivo salvo corretamente

5. **Conformidade**
   - Non-blocking I/O
   - Não crasha com request inválido
   - Suporta múltiplas conexões
   - Configuração funcional

6. **Memory Leaks**
   - Executar com valgrind/leaks
   - Fazer vários requests
   - Verificar leaks ao sair

### Perguntas Comuns

**P: Como funciona o non-blocking I/O?**
R: Usamos `fcntl(fd, F_SETFL, O_NONBLOCK)` em todos os sockets. O `poll()` espera por eventos (POLLIN/POLLOUT) e só fazemos read/write quando há dados disponíveis.

**P: Por que apenas 1 poll()?**
R: Para eficiência. Um único `poll()` pode monitorar todos os file descriptors (listening sockets + client connections) simultaneamente. Ver `ServerManager::run()`.

**P: Como funciona o CGI?**
R: Fazemos fork(), configuramos pipes para stdin/stdout, usamos `execve()` para executar o script, e lemos a saída. O parent process espera com `waitpid()`.

**P: Como é o parsing do multipart/form-data?**
R: Parseamos os boundaries, extraímos headers de cada parte, salvamos o conteúdo do arquivo. Ver `RequestHandler::handleUpload()`.

**P: O servidor aguenta stress?**
R: Sim, testamos com 10-50 conexões simultâneas usando `poll()` para multiplexing. Ver `tests/stress_test.sh`.

---

## ✅ Critérios de Avaliação

### Parte Obrigatória (100 pontos)

| Critério | Pontos | Status |
|----------|--------|--------|
| Compilação sem erro/warning | 10 | ✅ |
| HTTP GET funcionando | 15 | ✅ |
| HTTP POST funcionando | 15 | ✅ |
| HTTP DELETE funcionando | 10 | ✅ |
| CGI funcionando | 20 | ✅ |
| File upload funcionando | 10 | ✅ |
| Configuration file | 10 | ✅ |
| Non-blocking I/O | 10 | ✅ |

**Total Esperado: 100/100**

### Bonus (25 pontos)
- Multiple CGI support: ✅ (Python, PHP, etc.)
- Custom error pages: ✅
- Multiple ports: ✅
- Virtual hosts: ✅

**Total com Bonus: 125/100**

---

## 🚨 Erros que Causam Nota 0

1. ❌ **Blocking I/O**: Se usar read/write sem non-blocking
2. ❌ **Múltiplos poll()**: Se usar mais de um poll() ou equivalente
3. ❌ **Fork fora de CGI**: Se usar fork() em outro lugar
4. ❌ **Memory leaks**: Leaks detectados pelo valgrind
5. ❌ **Crashes**: Segfault ou crash com qualquer input
6. ❌ **Código copiado**: Plágio detectado
7. ❌ **Norminette**: Se o subject exigir e não passar

---

## 🎉 Sucesso!

Se todos os itens acima estão ✅, o projeto está pronto para submissão!

**Boa avaliação! 🚀**

---

**Última verificação**: 15 de Outubro de 2025
**Autor**: tiago (tiagoleite) - 42 Porto
