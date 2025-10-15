# 📋 GUIA DE TESTES - AVALIAÇÃO 42 WEBSERV

## ⚠️ REGRAS CRÍTICAS (GRADE 0 AUTOMÁTICO)

### ✅ VERIFICADO E CORRIGIDO:
- ✅ **errno**: NUNCA verificar errno após read/recv/write/send → **CORRIGIDO**
- ✅ **poll()**: Usar apenas 1 poll() (ou equivalente) → **VERIFICADO** (linha 165)
- ✅ **return values**: Verificar tanto -1 quanto 0 → **IMPLEMENTADO**
- ✅ **I/O operations**: Todas passam por poll() → **ARQUITETURA CORRETA**

---

## 🚀 1. PREPARAÇÃO

### Iniciar o Servidor:
```bash
cd /Users/tiagoleite/42/webserv
make re
./webserv config/cgi.conf
```

### Executar Suite de Testes Automáticos:
```bash
./tests/evaluation_tests.sh
```

---

## 🔍 2. TESTES MANUAIS OBRIGATÓRIOS

### 2.1 Verificação de Código
```bash
# Verificar que existe apenas 1 poll()
grep -rn "poll(" src/ includes/ | grep -v "poll()" | wc -l

# Verificar que não há errno após I/O
grep -A3 -B3 "recv\|send\|read\|write" src/ | grep errno

# Resultado esperado: NENHUM errno após operações de I/O
```

### 2.2 Métodos HTTP Básicos

**GET:**
```bash
curl -i http://localhost:8080/
curl -i http://localhost:8080/about.html
curl -i http://localhost:8080/nonexistent.html  # Deve retornar 404
```

**POST:**
```bash
curl -i -X POST http://localhost:8080/cgi-bin/test.py -d "name=Test&data=Hello"
```

**DELETE:**
```bash
# Primeiro fazer upload
echo "test" > /tmp/test.txt
curl -X POST http://localhost:8080/upload -F "file=@/tmp/test.txt"

# Depois deletar
curl -i -X DELETE http://localhost:8080/uploads/test.txt
```

**Método Desconhecido (deve retornar 501):**
```bash
curl -i -X UNKNOWN http://localhost:8080/
```

### 2.3 Upload e Download de Arquivos

```bash
# Criar arquivo de teste
echo "Test upload content" > /tmp/upload_test.txt

# Upload
curl -X POST http://localhost:8080/upload -F "file=@/tmp/upload_test.txt"

# Verificar que foi uploadado
curl http://localhost:8080/uploads/

# Download
curl http://localhost:8080/uploads/upload_test.txt
```

### 2.4 CGI Tests

**CGI com GET:**
```bash
curl http://localhost:8080/cgi-bin/hello.py
```

**CGI com POST:**
```bash
curl -X POST http://localhost:8080/cgi-bin/test.py -d "key=value&test=data"
```

**CGI com Query String:**
```bash
curl "http://localhost:8080/cgi-bin/hello.py?name=Test&id=123"
```

### 2.5 Códigos de Status

```bash
# 200 OK
curl -i http://localhost:8080/

# 404 Not Found
curl -i http://localhost:8080/nonexistent

# 405 Method Not Allowed
curl -i -X POST http://localhost:8080/about.html

# 413 Request Entity Too Large
dd if=/dev/zero of=/tmp/large.bin bs=1M count=20
curl -i -X POST http://localhost:8080/upload -F "file=@/tmp/large.bin"
```

---

## 🌐 3. TESTES COM NAVEGADOR

### 3.1 Abrir no Browser
1. Abrir: http://localhost:8080/
2. **Abrir DevTools** (F12) → Network tab
3. Verificar:
   - Headers da request/response
   - Status codes corretos
   - Content-Type correto

### 3.2 Testar Features
- [ ] Navegar entre páginas (/, /about.html)
- [ ] Testar upload de arquivo pelo formulário
- [ ] Clicar em "View Uploads" - deve mostrar directory listing
- [ ] Testar CGI através do link
- [ ] Tentar acessar URL inválida → deve mostrar 404 customizado

### 3.3 Verificar Responsividade
- [ ] Recarregar página múltiplas vezes rapidamente
- [ ] Abrir múltiplas tabs simultaneamente
- [ ] Servidor não deve crashar

---

## 📝 4. TESTES COM TELNET/NETCAT

### Teste Básico:
```bash
telnet localhost 8080
GET / HTTP/1.1
Host: localhost

# (Pressionar Enter 2x)
```

### Request Malformada (não deve crashar):
```bash
echo "INVALID REQUEST" | nc localhost 8080
```

### Request Incompleta (não deve crashar):
```bash
echo "GET / HTTP/1.1" | nc localhost 8080
# Não enviar Host header - servidor deve lidar com isso
```

---

## 🔄 5. TESTES DE CONFIGURAÇÃO

### 5.1 Múltiplos Servidores
Verificar se config/cgi.conf tem múltiplos servers configurados.

### 5.2 Páginas de Erro Customizadas
```bash
curl http://localhost:8080/nonexistent
# Deve mostrar página 404 customizada em www/errors/404.html
```

### 5.3 Client Body Size Limit
```bash
# Criar arquivo grande (> 10MB)
dd if=/dev/zero of=/tmp/huge.bin bs=1M count=15

# Tentar upload - deve retornar 413
curl -i -X POST http://localhost:8080/upload -F "file=@/tmp/huge.bin"
```

### 5.4 Directory Listing
```bash
curl http://localhost:8080/uploads/
# Deve listar arquivos se autoindex: on
```

### 5.5 Default Index File
```bash
curl http://localhost:8080/
# Deve servir index.html automaticamente
```

---

## 💪 6. TESTE DE STRESS (SIEGE)

### Instalar Siege (se necessário):
```bash
brew install siege
```

### Teste de Disponibilidade (> 99.5%):
```bash
siege -b -c 10 -t 15S http://localhost:8080/
```

**Verificar:**
- Availability deve ser > 99.5%
- Servidor não deve crashar
- Nenhuma conexão deve ficar pendurada

### Teste de Longa Duração:
```bash
# Rodar por 5 minutos
siege -b -c 20 -t 5M http://localhost:8080/

# Monitorar memória em outro terminal
watch -n 1 "ps aux | grep webserv | grep -v grep"
```

**Verificar:**
- Memória não deve aumentar indefinidamente (leak)
- CPU deve estabilizar
- Servidor deve continuar responsivo

---

## ✅ 7. CHECKLIST FINAL PRÉ-AVALIAÇÃO

### Código:
- [ ] Apenas 1 poll() em todo o código
- [ ] ZERO verificações de errno após I/O operations
- [ ] Todos read/recv/write/send verificam -1 e 0
- [ ] Nenhum I/O sem passar por poll()
- [ ] Compila sem warnings (-Wall -Wextra -Werror)
- [ ] Código em C++98

### Funcionalidades:
- [ ] GET, POST, DELETE funcionam
- [ ] Upload e download de arquivos
- [ ] CGI executa scripts Python
- [ ] Status codes corretos (200, 404, 405, 413, 501)
- [ ] Páginas de erro customizadas
- [ ] Directory listing
- [ ] Client body size limit funciona

### Performance:
- [ ] Siege availability > 99.5%
- [ ] Sem memory leaks
- [ ] Sem crashes em requests malformadas
- [ ] Múltiplas conexões simultâneas

### Configuração:
- [ ] Config file é parseado corretamente
- [ ] Múltiplos servers suportados
- [ ] Diferentes ports funcionam
- [ ] Error pages customizadas funcionam

---

## 🎯 8. PONTOS DE ATENÇÃO DO AVALIADOR

### O que o avaliador VAI verificar:

1. **poll() único**:
   ```bash
   grep -rn "poll(" src/ | wc -l  # Deve ser 1
   ```

2. **errno proibido**:
   ```bash
   grep -A5 "recv\|send" src/ | grep errno  # Deve ser 0
   ```

3. **Non-blocking I/O**:
   - Ver fcntl(F_SETFL, O_NONBLOCK) no código

4. **Stress test**:
   - Rodar siege e verificar > 99.5%

5. **Browser test**:
   - Abrir no navegador e verificar que funciona

---

## 📊 RESULTADOS ESPERADOS

### Suite Automática:
```
Passed: 15-20
Failed: 0
Total:  15-20

✓ ALL TESTS PASSED! PROJECT READY!
```

### Siege:
```
Availability:       100.00 %
Response time:      0.02 secs
Transaction rate:   500+ trans/sec
```

### Navegador:
- Todas páginas carregam corretamente
- Upload funciona
- CGI funciona
- Nenhum erro no console

---

## ⚠️ PROBLEMAS COMUNS E SOLUÇÕES

### "Connection refused":
→ Servidor não está rodando. Execute `./webserv config/cgi.conf`

### "Address already in use":
→ Porta 8080 ocupada. Matar processo: `lsof -ti:8080 | xargs kill -9`

### CGI não funciona:
→ Verificar permissões: `chmod +x www/cgi-bin/*.py`

### Upload falha:
→ Verificar que pasta existe: `mkdir -p www/uploads`

---

## 🎓 NOTAS FINAIS

Este servidor está pronto para avaliação. Todos os requisitos obrigatórios foram implementados e testados. As violações críticas (errno após I/O) foram identificadas e **CORRIGIDAS**.

**Boa sorte na avaliação! 🚀**
