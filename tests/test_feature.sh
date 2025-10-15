#!/bin/bash

# =============================================================================
# WEBSERV - Testes Individuais por Funcionalidade
# =============================================================================
# Execute: chmod +x tests/test_feature.sh && ./tests/test_feature.sh [feature]
#
# Funcionalidades disponíveis:
#   query      - Query string parsing
#   form       - Form data (POST)
#   upload     - File upload
#   cache      - Cache headers
#   delete     - DELETE method
#   chunked    - Chunked encoding
#   limits     - Size limits
# =============================================================================

SERVER_URL="http://localhost:8080"
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'
BOLD='\033[1m'

print_header() {
    echo ""
    echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${BLUE}  $1${NC}"
    echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════════════${NC}"
    echo ""
}

test_query() {
    print_header "TESTE: Query String Parsing"

    echo "Teste 1: Query string simples"
    echo "URL: $SERVER_URL/test?name=João&age=25&city=Lisboa"
    echo ""
    curl -v "$SERVER_URL/test?name=João&age=25&city=Lisboa" 2>&1 | head -30
    echo ""

    echo "Teste 2: Query com caracteres especiais"
    echo "URL: $SERVER_URL/test?message=Olá%20Mundo&emoji=%F0%9F%9A%80"
    echo ""
    curl -v "$SERVER_URL/test?message=Olá%20Mundo&emoji=%F0%9F%9A%80" 2>&1 | head -30
    echo ""

    echo "Teste 3: Múltiplos parâmetros"
    echo "URL: $SERVER_URL/test?a=1&b=2&c=3&d=4&e=5"
    echo ""
    curl "$SERVER_URL/test?a=1&b=2&c=3&d=4&e=5" 2>&1 | head -20
}

test_form() {
    print_header "TESTE: Form Data (POST)"

    echo "Teste 1: Form simples"
    echo "Content-Type: application/x-www-form-urlencoded"
    echo "Data: name=João Silva&email=joao@example.com"
    echo ""
    curl -v -X POST "$SERVER_URL/test" \
        -H "Content-Type: application/x-www-form-urlencoded" \
        -d "name=João Silva&email=joao@example.com&message=Teste do webserv" 2>&1
    echo ""

    echo "Teste 2: Form com caracteres especiais"
    curl -X POST "$SERVER_URL/test" \
        -H "Content-Type: application/x-www-form-urlencoded" \
        -d "text=Olá+Mundo&special=test%40email.com" 2>&1 | head -30
}

test_upload() {
    print_header "TESTE: File Upload"

    # Criar arquivos de teste
    echo "Criando arquivos de teste..."
    echo "Este é um arquivo de teste pequeno" > /tmp/test_small.txt
    dd if=/dev/zero of=/tmp/test_medium.bin bs=1K count=50 2>/dev/null
    echo ""

    echo "Teste 1: Upload de arquivo texto"
    echo "File: test_small.txt"
    echo ""
    curl -v -X POST "$SERVER_URL/upload" \
        -F "file=@/tmp/test_small.txt" \
        -F "description=Arquivo de teste" 2>&1
    echo ""

    echo "Teste 2: Upload de arquivo binário (50KB)"
    echo "File: test_medium.bin"
    echo ""
    curl -X POST "$SERVER_URL/upload" \
        -F "file=@/tmp/test_medium.bin" \
        -F "username=testuser" 2>&1 | head -30
    echo ""

    echo "Teste 3: Upload múltiplo"
    echo "Files: test_small.txt + test_medium.bin"
    echo ""
    curl -X POST "$SERVER_URL/upload" \
        -F "file1=@/tmp/test_small.txt" \
        -F "file2=@/tmp/test_medium.bin" 2>&1 | head -30

    # Limpar
    rm -f /tmp/test_small.txt /tmp/test_medium.bin
}

test_cache() {
    print_header "TESTE: Cache Headers"

    echo "Teste 1: Primeira requisição (deve incluir ETag e Last-Modified)"
    echo ""
    RESPONSE=$(curl -v "$SERVER_URL/index.html" 2>&1)
    echo "$RESPONSE" | head -40
    echo ""

    # Extrair ETag
    ETAG=$(echo "$RESPONSE" | grep -i "ETag:" | cut -d' ' -f3 | tr -d '\r')
    echo -e "${GREEN}ETag extraído: $ETAG${NC}"
    echo ""

    if [ ! -z "$ETAG" ]; then
        echo "Teste 2: Requisição com If-None-Match (deve retornar 304)"
        echo "If-None-Match: $ETAG"
        echo ""
        curl -v "$SERVER_URL/index.html" -H "If-None-Match: $ETAG" 2>&1 | head -30
    else
        echo -e "${RED}ETag não encontrado, pulando teste 2${NC}"
    fi
}

test_delete() {
    print_header "TESTE: DELETE Method"

    # Criar arquivos de teste
    echo "Criando arquivos de teste..."
    echo "Arquivo 1" > www/test_delete_1.txt
    echo "Arquivo 2" > www/test_delete_2.txt
    echo "Arquivo protegido" > www/test_protected.txt
    chmod 444 www/test_protected.txt
    echo ""

    echo "Teste 1: DELETE arquivo existente (deve retornar 204)"
    echo "File: www/test_delete_1.txt"
    echo ""
    curl -v -X DELETE "$SERVER_URL/test_delete_1.txt" 2>&1
    echo ""

    if [ ! -f "www/test_delete_1.txt" ]; then
        echo -e "${GREEN}✓ Arquivo foi deletado com sucesso${NC}"
    else
        echo -e "${RED}✗ Arquivo ainda existe!${NC}"
    fi
    echo ""

    echo "Teste 2: DELETE arquivo inexistente (deve retornar 404)"
    echo ""
    curl -v -X DELETE "$SERVER_URL/arquivo_inexistente.txt" 2>&1 | head -20
    echo ""

    echo "Teste 3: DELETE arquivo sem permissão (deve retornar 403)"
    echo "File: www/test_protected.txt (chmod 444)"
    echo ""
    curl -v -X DELETE "$SERVER_URL/test_protected.txt" 2>&1 | head -20

    # Limpar
    rm -f www/test_*.txt
}

test_chunked() {
    print_header "TESTE: Chunked Transfer Encoding"

    echo "⚠️  NOTA: Navegadores não suportam envio de chunked encoding"
    echo "Este teste deve ser feito manualmente com telnet ou script Python"
    echo ""

    echo "Exemplo com telnet:"
    echo ""
    echo -e "${YELLOW}telnet localhost 8080${NC}"
    echo "POST /test HTTP/1.1"
    echo "Host: localhost"
    echo "Transfer-Encoding: chunked"
    echo ""
    echo "5"
    echo "Hello"
    echo "6"
    echo " World"
    echo "0"
    echo ""
    echo ""

    echo "Exemplo com script Python:"
    cat > /tmp/test_chunked.py << 'EOF'
import socket

def send_chunked():
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

send_chunked()
EOF

    echo ""
    echo "Execute: python3 /tmp/test_chunked.py"
}

test_limits() {
    print_header "TESTE: Size Limits"

    echo "Teste 1: URI normal (deve funcionar)"
    echo "URI: /test?param=value"
    echo ""
    curl -v "$SERVER_URL/test?param=value" 2>&1 | head -20
    echo ""

    echo "Teste 2: URI muito longa (> 8192 bytes, deve retornar erro)"
    LONG_URI=$(printf 'a%.0s' {1..9000})
    echo "URI length: 9000 bytes"
    echo ""
    STATUS=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/$LONG_URI")
    echo "Status: $STATUS"
    if [ "$STATUS" = "414" ] || [ "$STATUS" = "400" ]; then
        echo -e "${GREEN}✓ Limite de URI funcionando corretamente${NC}"
    else
        echo -e "${YELLOW}⚠ Status inesperado: $STATUS${NC}"
    fi
    echo ""

    echo "Teste 3: Header muito longo"
    LONG_VALUE=$(printf 'x%.0s' {1..9000})
    STATUS=$(curl -s -o /dev/null -w "%{http_code}" -H "X-Long-Header: $LONG_VALUE" "$SERVER_URL/test")
    echo "Header length: 9000 bytes"
    echo "Status: $STATUS"
    if [ "$STATUS" = "431" ] || [ "$STATUS" = "400" ]; then
        echo -e "${GREEN}✓ Limite de header funcionando corretamente${NC}"
    else
        echo -e "${YELLOW}⚠ Status inesperado: $STATUS${NC}"
    fi
}

# Menu de ajuda
show_help() {
    echo "Uso: $0 [feature]"
    echo ""
    echo "Funcionalidades disponíveis:"
    echo "  query      - Query string parsing"
    echo "  form       - Form data (POST)"
    echo "  upload     - File upload"
    echo "  cache      - Cache headers"
    echo "  delete     - DELETE method"
    echo "  chunked    - Chunked encoding"
    echo "  limits     - Size limits"
    echo "  all        - Todos os testes"
    echo ""
    echo "Exemplo:"
    echo "  $0 query"
    echo "  $0 upload"
    echo "  $0 all"
}

# Main
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

case "$1" in
    query)
        test_query
        ;;
    form)
        test_form
        ;;
    upload)
        test_upload
        ;;
    cache)
        test_cache
        ;;
    delete)
        test_delete
        ;;
    chunked)
        test_chunked
        ;;
    limits)
        test_limits
        ;;
    all)
        test_query
        test_form
        test_upload
        test_cache
        test_delete
        test_chunked
        test_limits
        ;;
    *)
        echo -e "${RED}Funcionalidade desconhecida: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}${BOLD}Teste concluído!${NC}"
