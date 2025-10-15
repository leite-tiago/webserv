# 📋 Exemplos de Comandos de Teste - webserv

Copie e cole estes comandos para testar cada funcionalidade.

---

## 🔍 Query Strings

### Básico
```bash
curl "http://localhost:8080/test?name=João&age=25"
```

### Com caracteres especiais
```bash
curl "http://localhost:8080/test?message=Olá%20Mundo&emoji=%F0%9F%9A%80"
```

### Múltiplos parâmetros
```bash
curl "http://localhost:8080/test?search=webserv&category=HTTP&limit=10&page=1&sort=desc"
```

### Ver headers da resposta
```bash
curl -i "http://localhost:8080/test?key=value"
```

---

## 📝 Form Data (POST)

### Form simples
```bash
curl -X POST http://localhost:8080/test \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "name=João Silva&email=joao@example.com&message=Teste"
```

### Form com múltiplos campos
```bash
curl -X POST http://localhost:8080/test \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=testuser&password=secret123&remember=true&role=admin"
```

### Form com caracteres especiais
```bash
curl -X POST http://localhost:8080/test \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "text=Olá+Mundo&email=test%40email.com&url=http%3A%2F%2Fexample.com"
```

### Ver resposta completa
```bash
curl -v -X POST http://localhost:8080/test \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "field1=value1&field2=value2"
```

---

## 📤 Upload de Arquivos

### Upload de arquivo único
```bash
# Criar arquivo de teste
echo "Este é um arquivo de teste" > test.txt

# Upload
curl -X POST http://localhost:8080/upload \
  -F "file=@test.txt" \
  -F "description=Meu arquivo de teste"
```

### Upload de arquivo binário
```bash
# Criar arquivo binário de 1MB
dd if=/dev/zero of=test.bin bs=1M count=1

# Upload
curl -X POST http://localhost:8080/upload \
  -F "file=@test.bin"
```

### Upload múltiplo
```bash
curl -X POST http://localhost:8080/upload \
  -F "file1=@file1.txt" \
  -F "file2=@file2.jpg" \
  -F "file3=@file3.pdf"
```

### Upload com campos adicionais
```bash
curl -X POST http://localhost:8080/upload \
  -F "file=@document.pdf" \
  -F "title=Documento Importante" \
  -F "category=docs" \
  -F "public=true" \
  -F "tags=test,upload,webserv"
```

### Ver progresso do upload
```bash
curl -# -X POST http://localhost:8080/upload \
  -F "file=@largefile.zip"
```

---

## 💾 Cache Headers

### Primeira requisição (obter ETag)
```bash
curl -i http://localhost:8080/index.html
```

### Ver apenas headers
```bash
curl -I http://localhost:8080/index.html
```

### Requisição condicional (304 Not Modified)
```bash
# Passo 1: Obter ETag
ETAG=$(curl -s -I http://localhost:8080/index.html | grep -i "ETag:" | cut -d' ' -f2 | tr -d '\r')

# Passo 2: Usar If-None-Match
curl -i http://localhost:8080/index.html \
  -H "If-None-Match: $ETAG"
```

### Ver todos os cache headers
```bash
curl -I http://localhost:8080/index.html | grep -i -E "(etag|last-modified|cache-control)"
```

---

## 🗑️ DELETE Method

### DELETE arquivo existente
```bash
# Criar arquivo
echo "Arquivo para deletar" > www/test_delete.txt

# Deletar
curl -X DELETE http://localhost:8080/test_delete.txt

# Verificar status
curl -i -X DELETE http://localhost:8080/test_delete.txt
```

### DELETE arquivo inexistente (404)
```bash
curl -i -X DELETE http://localhost:8080/arquivo_inexistente.txt
```

### DELETE com verbose
```bash
curl -v -X DELETE http://localhost:8080/test.txt
```

---

## 🔄 Chunked Transfer Encoding

### Enviar (requer script Python ou telnet)
```bash
# Criar script Python
cat > send_chunked.py << 'EOF'
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('localhost', 8080))

request = """POST /test HTTP/1.1\r
Host: localhost\r
Transfer-Encoding: chunked\r
\r
5\r
Hello\r
6\r
 World\r
1\r
!\r
0\r
\r
"""

sock.send(request.encode())
response = sock.recv(4096)
print(response.decode())
sock.close()
EOF

# Executar
python3 send_chunked.py
```

---

## 📏 Testes de Limites

### URI normal
```bash
curl http://localhost:8080/test?param=value
```

### URI longa (teste de limite)
```bash
# Gerar URI de 9000 caracteres
LONG_PATH=$(printf 'a%.0s' {1..9000})
curl -s -o /dev/null -w "%{http_code}" "http://localhost:8080/$LONG_PATH"
```

### Header grande
```bash
LONG_VALUE=$(printf 'x%.0s' {1..9000})
curl -s -o /dev/null -w "%{http_code}" \
  -H "X-Long-Header: $LONG_VALUE" \
  http://localhost:8080/test
```

### Muitos headers
```bash
curl http://localhost:8080/test \
  -H "X-Header-1: value1" \
  -H "X-Header-2: value2" \
  -H "X-Header-3: value3" \
  # ... adicionar até 110 headers
```

---

## 🧪 Testes Diversos

### GET básico
```bash
curl http://localhost:8080/index.html
```

### HEAD request
```bash
curl -I http://localhost:8080/index.html
```

### POST vazio
```bash
curl -X POST http://localhost:8080/test
```

### POST com JSON (não processado, apenas recebido)
```bash
curl -X POST http://localhost:8080/test \
  -H "Content-Type: application/json" \
  -d '{"name":"João","age":25}'
```

### Timeout test (requisição lenta)
```bash
curl --max-time 5 http://localhost:8080/index.html
```

### Ver tempo de resposta
```bash
curl -o /dev/null -s -w "Time: %{time_total}s\nStatus: %{http_code}\n" \
  http://localhost:8080/index.html
```

---

## 📊 Benchmarking

### Apache Bench - Teste básico
```bash
ab -n 100 -c 10 http://localhost:8080/index.html
```

### Apache Bench - Teste pesado
```bash
ab -n 10000 -c 100 http://localhost:8080/index.html
```

### Apache Bench - POST
```bash
echo "name=test&email=test@example.com" > post_data.txt
ab -n 100 -c 10 -p post_data.txt \
  -T "application/x-www-form-urlencoded" \
  http://localhost:8080/test
```

### Siege - Teste básico
```bash
siege -c 10 -r 100 http://localhost:8080/index.html
```

### Siege - Teste de tempo
```bash
siege -c 20 -t 1M http://localhost:8080/index.html
```

---

## 🔬 Debugging

### Ver todos os headers (request e response)
```bash
curl -v http://localhost:8080/index.html
```

### Salvar response em arquivo
```bash
curl http://localhost:8080/index.html > output.html
```

### Ver apenas status code
```bash
curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/index.html
```

### Follow redirects
```bash
curl -L http://localhost:8080/redirect
```

### Especificar User-Agent
```bash
curl -A "Mozilla/5.0 (WebservTest/1.0)" http://localhost:8080/test
```

### Custom headers
```bash
curl http://localhost:8080/test \
  -H "X-Custom-Header: value" \
  -H "X-Request-ID: 12345" \
  -H "X-API-Key: secret"
```

---

## 🎯 Testes Específicos do Subject

### Teste com navegador real
```bash
# Abrir no navegador padrão (Linux)
xdg-open http://localhost:8080/test_form.html

# Ou abrir manualmente:
# Chrome/Firefox: http://localhost:8080/test_form.html
```

### Teste com telnet (requisição manual)
```bash
telnet localhost 8080
```
Depois digite:
```
GET /test?name=test HTTP/1.1
Host: localhost
Connection: close

```

### Comparar com NGINX
```bash
# Requisição para webserv
curl -i http://localhost:8080/index.html

# Requisição para NGINX (se instalado)
curl -i http://localhost:80/index.html

# Comparar headers
diff <(curl -I http://localhost:8080/index.html) \
     <(curl -I http://localhost:80/index.html)
```

---

## 💡 Dicas

### Testar rapidamente todos os métodos
```bash
# GET
curl http://localhost:8080/index.html

# POST
curl -X POST http://localhost:8080/test -d "test=data"

# DELETE
echo "temp" > www/temp.txt && curl -X DELETE http://localhost:8080/temp.txt

# HEAD
curl -I http://localhost:8080/index.html
```

### Loop de testes (stress simples)
```bash
for i in {1..100}; do
  curl -s -o /dev/null -w "%{http_code}\n" http://localhost:8080/index.html
done
```

### Testar concorrência (10 requisições paralelas)
```bash
for i in {1..10}; do
  curl -s http://localhost:8080/index.html &
done
wait
```

---

**Copie e cole estes comandos para testar rapidamente o webserv!** 🚀
