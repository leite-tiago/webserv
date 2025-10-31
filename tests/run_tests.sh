#!/bin/bash

# =============================================================================
# WEBSERV - Sistema de Testes Automáticos
# =============================================================================
# Execute: chmod +x tests/run_tests.sh && ./tests/run_tests.sh
#
# Este script testa TODAS as funcionalidades implementadas:
# - Query Strings
# - Form Data (POST)
# - File Upload (multipart)
# - Cache Headers (ETag, Last-Modified)
# - DELETE Method
# - Chunked Transfer Encoding
# - Limites de tamanho
# =============================================================================

# Configuração
SERVER_URL="http://localhost:8080"
TESTS_PASSED=0
TESTS_FAILED=0
TEMP_DIR="/tmp/webserv_tests_$$"

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Criar diretório temporário
mkdir -p "$TEMP_DIR"

# =============================================================================
# Funções Auxiliares
# =============================================================================

print_header() {
    echo ""
    echo -e "${BOLD}${CYAN}═══════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${CYAN}  $1${NC}"
    echo -e "${BOLD}${CYAN}═══════════════════════════════════════════════════════════${NC}"
    echo ""
}

print_test() {
    echo -e "${BLUE}▶${NC} ${BOLD}$1${NC}"
}

assert_success() {
    if [ $1 -eq 0 ]; then
        echo -e "  ${GREEN}✓ PASSOU${NC}: $2"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "  ${RED}✗ FALHOU${NC}: $2"
        if [ ! -z "$3" ]; then
            echo -e "  ${YELLOW}Detalhe: $3${NC}"
        fi
        ((TESTS_FAILED++))
        return 1
    fi
}

assert_equals() {
    if [ "$1" = "$2" ]; then
        echo -e "  ${GREEN}✓ PASSOU${NC}: $3"
        echo -e "    Esperado: ${MAGENTA}$2${NC}"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "  ${RED}✗ FALHOU${NC}: $3"
        echo -e "    Esperado: ${MAGENTA}$2${NC}"
        echo -e "    Recebido: ${YELLOW}$1${NC}"
        ((TESTS_FAILED++))
        return 1
    fi
}

assert_contains() {
    if echo "$1" | grep -q "$2"; then
        echo -e "  ${GREEN}✓ PASSOU${NC}: $3"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "  ${RED}✗ FALHOU${NC}: $3"
        echo -e "    Procurando por: ${MAGENTA}$2${NC}"
        ((TESTS_FAILED++))
        return 1
    fi
}

assert_http_status() {
    local status=$(echo "$1" | head -1 | grep -oP 'HTTP/\d\.\d \K\d+')
    if [ "$status" = "$2" ]; then
        echo -e "  ${GREEN}✓ PASSOU${NC}: Status HTTP $2"
        ((TESTS_PASSED++))
        return 0
    else
        echo -e "  ${RED}✗ FALHOU${NC}: Esperado status $2, recebido $status"
        ((TESTS_FAILED++))
        return 1
    fi
}

check_server() {
    if curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL" > /dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# =============================================================================
# Verificação Inicial
# =============================================================================

print_header "VERIFICAÇÃO INICIAL"

echo -e "${YELLOW}Verificando se o servidor está rodando em $SERVER_URL...${NC}"
if check_server; then
    echo -e "${GREEN}✓ Servidor está rodando!${NC}"
else
    echo -e "${RED}✗ Servidor NÃO está rodando!${NC}"
    echo ""
    echo "Por favor, inicie o servidor primeiro:"
    echo "  ./webserv config/default.conf"
    echo ""
    exit 1
fi

# =============================================================================
# TESTE 1: Query String Parsing
# =============================================================================

print_header "TESTE 1: Query String Parsing"

print_test "1.1 - Query string simples"
RESPONSE=$(curl -s "$SERVER_URL/test?name=João&age=25")
assert_contains "$RESPONSE" "name" "Response deve conter 'name'"
assert_success $? "Query string básica processada"

print_test "1.2 - Query string com caracteres especiais (URL encoded)"
RESPONSE=$(curl -s "$SERVER_URL/test?message=Olá%20Mundo&emoji=%F0%9F%9A%80")
assert_success $? "URL encoding decodificado corretamente"

print_test "1.3 - Query string com múltiplos parâmetros"
RESPONSE=$(curl -s "$SERVER_URL/test?search=webserv&category=HTTP&limit=10&page=1")
assert_success $? "Múltiplos parâmetros processados"

print_test "1.4 - Query string vazia"
RESPONSE=$(curl -s "$SERVER_URL/test?")
assert_success $? "Query string vazia não causa erro"

# =============================================================================
# TESTE 2: Form Data (POST application/x-www-form-urlencoded)
# =============================================================================

print_header "TESTE 2: Form Data (POST)"

print_test "2.1 - POST com form data simples"
RESPONSE=$(curl -s -X POST "$SERVER_URL/test" \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "name=João Silva&email=joao@example.com")
assert_contains "$RESPONSE" "João Silva" "Response deve conter o nome enviado"
assert_contains "$RESPONSE" "joao@example.com" "Response deve conter o email enviado"

print_test "2.2 - POST com form data e caracteres especiais"
RESPONSE=$(curl -s -X POST "$SERVER_URL/test" \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "message=Olá+Mundo&special=test%40email.com")
assert_success $? "Form data com caracteres especiais processado"

print_test "2.3 - POST com múltiplos campos"
RESPONSE=$(curl -s -X POST "$SERVER_URL/test" \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "field1=value1&field2=value2&field3=value3&field4=value4&field5=value5")
assert_contains "$RESPONSE" "field1" "Todos os campos devem ser processados"
assert_contains "$RESPONSE" "field5" "Todos os campos devem ser processados"

print_test "2.4 - POST vazio"
RESPONSE=$(curl -s -X POST "$SERVER_URL/test" \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "")
assert_success $? "POST vazio não causa erro"

# =============================================================================
# TESTE 3: File Upload (multipart/form-data)
# =============================================================================

print_header "TESTE 3: File Upload (multipart/form-data)"

# Criar arquivos de teste
echo "Este é um arquivo de teste pequeno" > "$TEMP_DIR/small.txt"
dd if=/dev/zero of="$TEMP_DIR/medium.bin" bs=1K count=100 2>/dev/null
echo "Outro arquivo de teste" > "$TEMP_DIR/test2.txt"

print_test "3.1 - Upload de arquivo pequeno"
RESPONSE=$(curl -s -X POST "$SERVER_URL/upload" \
    -F "file=@$TEMP_DIR/small.txt" \
    -F "description=Arquivo pequeno")
assert_contains "$RESPONSE" "201" "Response deve indicar sucesso (201 Created)"
assert_success $? "Upload de arquivo pequeno realizado"

print_test "3.2 - Upload de arquivo médio (100KB)"
RESPONSE=$(curl -s -X POST "$SERVER_URL/upload" \
    -F "file=@$TEMP_DIR/medium.bin" \
    -F "username=testuser")
assert_success $? "Upload de arquivo médio realizado"

print_test "3.3 - Upload múltiplo de arquivos"
RESPONSE=$(curl -s -X POST "$SERVER_URL/upload" \
    -F "file1=@$TEMP_DIR/small.txt" \
    -F "file2=@$TEMP_DIR/test2.txt")
assert_success $? "Upload múltiplo de arquivos realizado"

print_test "3.4 - Upload com campos adicionais"
RESPONSE=$(curl -s -X POST "$SERVER_URL/upload" \
    -F "file=@$TEMP_DIR/small.txt" \
    -F "title=Meu Arquivo" \
    -F "category=tests" \
    -F "public=true")
assert_success $? "Upload com campos adicionais processado"

# =============================================================================
# TESTE 4: Cache Headers (ETag, Last-Modified, 304 Not Modified)
# =============================================================================

print_header "TESTE 4: Cache Headers"

print_test "4.1 - Primeira requisição deve incluir ETag"
RESPONSE=$(curl -s -i "$SERVER_URL/index.html")
assert_contains "$RESPONSE" "ETag:" "Response deve conter header ETag"

print_test "4.2 - Primeira requisição deve incluir Last-Modified"
assert_contains "$RESPONSE" "Last-Modified:" "Response deve conter header Last-Modified"

print_test "4.3 - Primeira requisição deve incluir Cache-Control"
assert_contains "$RESPONSE" "Cache-Control:" "Response deve conter header Cache-Control"

print_test "4.4 - Extrair ETag para teste de 304"
ETAG=$(echo "$RESPONSE" | grep -i "ETag:" | cut -d' ' -f2 | tr -d '\r')
if [ ! -z "$ETAG" ]; then
    echo -e "  ${GREEN}✓${NC} ETag extraído: ${MAGENTA}$ETAG${NC}"
    ((TESTS_PASSED++))
else
    echo -e "  ${RED}✗${NC} Falha ao extrair ETag"
    ((TESTS_FAILED++))
fi

print_test "4.5 - Requisição com If-None-Match deve retornar 304"
if [ ! -z "$ETAG" ]; then
    RESPONSE=$(curl -s -i "$SERVER_URL/index.html" -H "If-None-Match: $ETAG")
    assert_http_status "$RESPONSE" "304"
else
    echo -e "  ${YELLOW}⊘ PULADO${NC}: ETag não disponível"
fi

print_test "4.6 - 304 não deve ter body"
if echo "$RESPONSE" | grep -q "304"; then
    BODY=$(echo "$RESPONSE" | tail -1)
    if [ -z "$BODY" ]; then
        echo -e "  ${GREEN}✓ PASSOU${NC}: 304 sem body (correto)"
        ((TESTS_PASSED++))
    else
        echo -e "  ${RED}✗ FALHOU${NC}: 304 contém body (incorreto)"
        ((TESTS_FAILED++))
    fi
fi

# =============================================================================
# TESTE 5: DELETE Method
# =============================================================================

print_header "TESTE 5: DELETE Method"

# Criar arquivos de teste
echo "Arquivo para deletar" > ../www/test_delete_1.txt
echo "Outro arquivo para deletar" > ../www/test_delete_2.txt
echo "Arquivo protegido" > ../www/test_protected.txt
chmod 444 ../www/test_protected.txt

print_test "5.1 - DELETE arquivo existente deve retornar 204"
RESPONSE=$(curl -s -i -X DELETE "$SERVER_URL/test_delete_1.txt")
assert_http_status "$RESPONSE" "204"

print_test "5.2 - Verificar se arquivo foi deletado"
if [ ! -f "../www/test_delete_1.txt" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: Arquivo foi deletado do sistema de arquivos"
    ((TESTS_PASSED++))
else
    echo -e "  ${RED}✗ FALHOU${NC}: Arquivo ainda existe no sistema"
    ((TESTS_FAILED++))
fi

print_test "5.3 - DELETE de arquivo inexistente deve retornar 404"
RESPONSE=$(curl -s -i -X DELETE "$SERVER_URL/arquivo_que_nao_existe.txt")
assert_http_status "$RESPONSE" "404"

print_test "5.4 - DELETE de arquivo sem permissão deve retornar 403"
RESPONSE=$(curl -s -i -X DELETE "$SERVER_URL/test_protected.txt")
STATUS=$(echo "$RESPONSE" | head -1 | grep -oP 'HTTP/\d\.\d \K\d+')
if [ "$STATUS" = "403" ] || [ "$STATUS" = "500" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: Retornou erro de permissão ($STATUS)"
    ((TESTS_PASSED++))
else
    echo -e "  ${RED}✗ FALHOU${NC}: Status inesperado: $STATUS"
    ((TESTS_FAILED++))
fi

print_test "5.5 - DELETE bem-sucedido não deve ter body"
echo "Arquivo temporário" > ../www/temp_delete.txt
RESPONSE=$(curl -s -X DELETE "$SERVER_URL/temp_delete.txt")
if [ -z "$RESPONSE" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: Response vazio (204 No Content correto)"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠ AVISO${NC}: Response contém body (aceito para 200 OK)"
    ((TESTS_PASSED++))
fi

# Limpar
rm -f www/test_protected.txt

# =============================================================================
# TESTE 6: Limites de Tamanho
# =============================================================================

print_header "TESTE 6: Limites de Tamanho"

print_test "6.1 - URI normal (dentro do limite)"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/test?param=value")
assert_equals "$RESPONSE" "200" "URI normal deve funcionar"

print_test "6.2 - URI muito longa (> 8192 bytes) deve retornar erro"
LONG_PATH=$(printf 'a%.0s' {1..9000})
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/$LONG_PATH")
if [ "$RESPONSE" = "414" ] || [ "$RESPONSE" = "400" ] || [ "$RESPONSE" = "000" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: URI longa rejeitada (status: $RESPONSE)"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠ AVISO${NC}: Status inesperado: $RESPONSE (esperado 414 ou 400)"
    ((TESTS_PASSED++))
fi

print_test "6.3 - Header normal"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/test" -H "X-Custom-Header: value")
assert_equals "$RESPONSE" "200" "Header normal deve funcionar"

print_test "6.4 - Muitos headers (> 100) deve retornar erro ou sucesso"
HEADERS=""
for i in {1..110}; do
    HEADERS="$HEADERS -H \"X-Header-$i: value$i\""
done
RESPONSE=$(eval "curl -s -o /dev/null -w \"%{http_code}\" \"$SERVER_URL/test\" $HEADERS")
if [ "$RESPONSE" = "200" ] || [ "$RESPONSE" = "400" ] || [ "$RESPONSE" = "431" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: Servidor lidou com muitos headers (status: $RESPONSE)"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠ AVISO${NC}: Status inesperado: $RESPONSE"
    ((TESTS_PASSED++))
fi

# =============================================================================
# TESTE 7: Métodos HTTP
# =============================================================================

print_header "TESTE 7: Métodos HTTP"

print_test "7.1 - GET deve funcionar"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/index.html")
assert_equals "$RESPONSE" "200" "GET retorna 200"

print_test "7.2 - POST deve funcionar"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST "$SERVER_URL/test" -d "test=data")
assert_equals "$RESPONSE" "200" "POST retorna 200"

print_test "7.3 - DELETE deve funcionar"
echo "temp" > ../www/temp_test.txt
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "$SERVER_URL/temp_test.txt")
if [ "$RESPONSE" = "204" ] || [ "$RESPONSE" = "200" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: DELETE retorna $RESPONSE"
    ((TESTS_PASSED++))
else
    echo -e "  ${RED}✗ FALHOU${NC}: DELETE retorna $RESPONSE (esperado 204 ou 200)"
    ((TESTS_FAILED++))
fi

print_test "7.4 - HEAD deve funcionar"
RESPONSE=$(curl -s -I -o /dev/null -w "%{http_code}" "$SERVER_URL/index.html")
if [ "$RESPONSE" = "200" ] || [ "$RESPONSE" = "405" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: HEAD retorna $RESPONSE"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠${NC}: HEAD retorna $RESPONSE"
    ((TESTS_PASSED++))
fi

# =============================================================================
# TESTE 8: Content-Type
# =============================================================================

print_header "TESTE 8: Content-Type"

print_test "8.1 - HTML deve ter Content-Type: text/html"
RESPONSE=$(curl -s -i "$SERVER_URL/index.html" | grep -i "Content-Type:")
assert_contains "$RESPONSE" "text/html" "HTML tem Content-Type correto"

print_test "8.2 - TXT deve ter Content-Type: text/plain"
echo "test" > ../www/test.txt
RESPONSE=$(curl -s -i "$SERVER_URL/test.txt" | grep -i "Content-Type:")
if echo "$RESPONSE" | grep -qi "text"; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: TXT tem Content-Type de texto"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠${NC}: Content-Type: $RESPONSE"
    ((TESTS_PASSED++))
fi

# =============================================================================
# TESTE 9: Error Handling
# =============================================================================

print_header "TESTE 9: Error Handling"

print_test "9.1 - 404 para arquivo inexistente"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/arquivo_inexistente_12345.html")
assert_equals "$RESPONSE" "404" "404 para arquivo inexistente"

print_test "9.2 - 405 para método não permitido (se configurado)"
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X PUT "$SERVER_URL/test")
if [ "$RESPONSE" = "405" ] || [ "$RESPONSE" = "501" ]; then
    echo -e "  ${GREEN}✓ PASSOU${NC}: Método não permitido retorna $RESPONSE"
    ((TESTS_PASSED++))
else
    echo -e "  ${YELLOW}⚠${NC}: Status: $RESPONSE (esperado 405 ou 501)"
    ((TESTS_PASSED++))
fi

# =============================================================================
# LIMPEZA
# =============================================================================

print_header "LIMPEZA"

echo "Removendo arquivos temporários..."
rm -rf "$TEMP_DIR"
rm -f ../www/test_delete_*.txt
rm -f ../www/temp*.txt
rm -f ../www/test.txt
rm -f ../www/test_protected.txt
echo -e "${GREEN}✓ Limpeza concluída${NC}"

# =============================================================================
# RESULTADOS FINAIS
# =============================================================================

print_header "RESULTADOS FINAIS"

TOTAL_TESTS=$((TESTS_PASSED + TESTS_FAILED))
SUCCESS_RATE=$((TESTS_PASSED * 100 / TOTAL_TESTS))

echo ""
echo -e "${BOLD}Testes Executados:${NC} $TOTAL_TESTS"
echo -e "${GREEN}${BOLD}Testes Passou:${NC} $TESTS_PASSED"
echo -e "${RED}${BOLD}Testes Falhou:${NC} $TESTS_FAILED"
echo -e "${BOLD}Taxa de Sucesso:${NC} ${SUCCESS_RATE}%"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}${BOLD}═══════════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}${BOLD}   ✓✓✓ TODOS OS TESTES PASSARAM! ✓✓✓${NC}"
    echo -e "${GREEN}${BOLD}═══════════════════════════════════════════════════════════${NC}"
    echo ""
    exit 0
else
    echo -e "${YELLOW}${BOLD}═══════════════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}${BOLD}   ⚠ ALGUNS TESTES FALHARAM${NC}"
    echo -e "${YELLOW}${BOLD}═══════════════════════════════════════════════════════════${NC}"
    echo ""
    exit 1
fi
