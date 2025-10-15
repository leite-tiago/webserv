#!/bin/bash

# =============================================================================
# WEBSERV - Testes de Performance e Stress
# =============================================================================
# Execute: chmod +x tests/stress_tests.sh && ./tests/stress_tests.sh
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

check_command() {
    if ! command -v $1 &> /dev/null; then
        echo -e "${RED}✗ $1 não está instalado${NC}"
        echo "Instale com: sudo apt install $2"
        return 1
    fi
    return 0
}

print_header "TESTES DE PERFORMANCE E STRESS"

echo -e "${YELLOW}Verificando ferramentas necessárias...${NC}"
echo ""

HAS_AB=false
HAS_SIEGE=false
HAS_CURL=true

if check_command "ab" "apache2-utils"; then
    HAS_AB=true
    echo -e "${GREEN}✓ Apache Bench (ab) disponível${NC}"
fi

if check_command "siege" "siege"; then
    HAS_SIEGE=true
    echo -e "${GREEN}✓ Siege disponível${NC}"
fi

echo -e "${GREEN}✓ curl disponível${NC}"
echo ""

# =============================================================================
# TESTE 1: Requisições Sequenciais
# =============================================================================

print_header "TESTE 1: Requisições Sequenciais"

echo "Enviando 100 requisições sequenciais..."
START=$(date +%s)
SUCCESS=0
FAILED=0

for i in {1..100}; do
    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL/index.html")
    if [ "$RESPONSE" = "200" ]; then
        ((SUCCESS++))
    else
        ((FAILED++))
    fi
    echo -ne "\rProgresso: $i/100 | Sucesso: $SUCCESS | Falhas: $FAILED"
done

END=$(date +%s)
DURATION=$((END - START))

echo ""
echo ""
echo -e "${GREEN}Resultado:${NC}"
echo "  Total: 100 requisições"
echo "  Sucesso: $SUCCESS"
echo "  Falhas: $FAILED"
echo "  Tempo: ${DURATION}s"
echo "  Taxa: $((100 / DURATION)) req/s"
echo ""

# =============================================================================
# TESTE 2: Requisições Concorrentes (com curl)
# =============================================================================

print_header "TESTE 2: Requisições Concorrentes (10 simultâneas)"

echo "Enviando 10 requisições concorrentes..."
START=$(date +%s)

for i in {1..10}; do
    curl -s -o /dev/null "$SERVER_URL/index.html" &
done

wait

END=$(date +%s)
DURATION=$((END - START))

echo -e "${GREEN}✓ Concluído em ${DURATION}s${NC}"
echo ""

# =============================================================================
# TESTE 3: Apache Bench
# =============================================================================

if [ "$HAS_AB" = true ]; then
    print_header "TESTE 3: Apache Bench (ab)"

    echo "Teste 3.1: 1000 requisições, 10 concorrentes"
    ab -n 1000 -c 10 -q "$SERVER_URL/index.html"
    echo ""

    echo "Teste 3.2: 500 requisições, 50 concorrentes"
    ab -n 500 -c 50 -q "$SERVER_URL/index.html"
    echo ""

    echo "Teste 3.3: POST com dados"
    echo "name=test&email=test@example.com" > /tmp/post_data.txt
    ab -n 100 -c 10 -p /tmp/post_data.txt -T "application/x-www-form-urlencoded" -q "$SERVER_URL/test"
    rm /tmp/post_data.txt
    echo ""
fi

# =============================================================================
# TESTE 4: Siege
# =============================================================================

if [ "$HAS_SIEGE" = true ]; then
    print_header "TESTE 4: Siege"

    echo "Teste 4.1: 10 usuários, 100 requisições"
    siege -c 10 -r 10 -b "$SERVER_URL/index.html"
    echo ""

    echo "Teste 4.2: Teste de 30 segundos"
    siege -c 20 -t 30S -b "$SERVER_URL/index.html"
    echo ""
fi

# =============================================================================
# TESTE 5: Upload de Arquivos Grandes
# =============================================================================

print_header "TESTE 5: Upload de Arquivos"

echo "Criando arquivo de 1MB..."
dd if=/dev/zero of=/tmp/test_1mb.bin bs=1M count=1 2>/dev/null

echo "Teste 5.1: Upload de arquivo 1MB"
START=$(date +%s)
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST "$SERVER_URL/upload" -F "file=@/tmp/test_1mb.bin")
END=$(date +%s)
DURATION=$((END - START))

if [ "$RESPONSE" = "201" ] || [ "$RESPONSE" = "200" ]; then
    echo -e "${GREEN}✓ Upload bem-sucedido em ${DURATION}s (status: $RESPONSE)${NC}"
else
    echo -e "${RED}✗ Upload falhou (status: $RESPONSE)${NC}"
fi

echo ""
echo "Criando arquivo de 5MB..."
dd if=/dev/zero of=/tmp/test_5mb.bin bs=1M count=5 2>/dev/null

echo "Teste 5.2: Upload de arquivo 5MB"
START=$(date +%s)
RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST "$SERVER_URL/upload" -F "file=@/tmp/test_5mb.bin")
END=$(date +%s)
DURATION=$((END - START))

if [ "$RESPONSE" = "201" ] || [ "$RESPONSE" = "200" ]; then
    echo -e "${GREEN}✓ Upload bem-sucedido em ${DURATION}s (status: $RESPONSE)${NC}"
else
    echo -e "${RED}✗ Upload falhou (status: $RESPONSE)${NC}"
fi

rm -f /tmp/test_*.bin
echo ""

# =============================================================================
# TESTE 6: Múltiplas Conexões Simultâneas
# =============================================================================

print_header "TESTE 6: Conexões Simultâneas"

echo "Teste 6.1: 50 requisições simultâneas"
START=$(date +%s)

for i in {1..50}; do
    curl -s -o /dev/null "$SERVER_URL/index.html" &
done

wait

END=$(date +%s)
DURATION=$((END - START))
echo -e "${GREEN}✓ 50 conexões concluídas em ${DURATION}s${NC}"
echo ""

# =============================================================================
# TESTE 7: Teste de Memória (Memory Leaks)
# =============================================================================

print_header "TESTE 7: Teste de Memória"

echo "Este teste requer que o servidor esteja rodando com valgrind:"
echo ""
echo -e "${YELLOW}valgrind --leak-check=full --show-leak-kinds=all ./webserv config/default.conf${NC}"
echo ""
echo "Enviando 100 requisições para verificar memory leaks..."

for i in {1..100}; do
    curl -s -o /dev/null "$SERVER_URL/index.html" > /dev/null 2>&1
    echo -ne "\rRequisições: $i/100"
done

echo ""
echo ""
echo -e "${GREEN}✓ 100 requisições enviadas${NC}"
echo "Verifique a saída do valgrind para memory leaks"
echo ""

# =============================================================================
# RESUMO
# =============================================================================

print_header "RESUMO DOS TESTES DE STRESS"

echo "Testes executados:"
echo "  ✓ Requisições sequenciais (100)"
echo "  ✓ Requisições concorrentes (10)"

if [ "$HAS_AB" = true ]; then
    echo "  ✓ Apache Bench (ab)"
fi

if [ "$HAS_SIEGE" = true ]; then
    echo "  ✓ Siege"
fi

echo "  ✓ Upload de arquivos grandes"
echo "  ✓ Conexões simultâneas (50)"
echo ""

echo -e "${BOLD}Recomendações:${NC}"
echo "1. Execute o servidor com valgrind para detectar memory leaks"
echo "2. Monitore o uso de CPU e memória com 'top' ou 'htop'"
echo "3. Verifique os logs do servidor para erros"
echo "4. Teste com diferentes configurações de max_body_size"
echo ""

if [ "$HAS_AB" = false ]; then
    echo -e "${YELLOW}Instale Apache Bench para mais testes:${NC}"
    echo "  sudo apt install apache2-utils"
    echo ""
fi

if [ "$HAS_SIEGE" = false ]; then
    echo -e "${YELLOW}Instale Siege para mais testes:${NC}"
    echo "  sudo apt install siege"
    echo ""
fi

echo -e "${GREEN}${BOLD}Testes de stress concluídos!${NC}"
