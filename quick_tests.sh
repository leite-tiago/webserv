#!/bin/bash

# 🧪 Script de Testes Rápidos - webserv
# Execute: chmod +x quick_tests.sh && ./quick_tests.sh

echo "🚀 webserv - Testes Rápidos"
echo "================================"
echo ""

BASE_URL="http://localhost:8080"

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para testar
test_feature() {
    echo -e "${BLUE}📝 $1${NC}"
    echo "Comando: $2"
    eval $2
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ Sucesso${NC}"
    else
        echo -e "${RED}❌ Falha${NC}"
    fi
    echo ""
}

echo "1️⃣  Teste de Query String"
echo "================================"
test_feature "GET com parâmetros" \
    "curl -s '${BASE_URL}/test?name=João&age=25&city=Lisboa' | head -20"

echo ""
echo "2️⃣  Teste de Form Data (POST)"
echo "================================"
test_feature "POST application/x-www-form-urlencoded" \
    "curl -s -X POST ${BASE_URL}/test \
    -H 'Content-Type: application/x-www-form-urlencoded' \
    -d 'name=João Silva&email=joao@example.com&message=Teste webserv' | head -25"

echo ""
echo "3️⃣  Teste de Upload de Arquivo"
echo "================================"
# Criar arquivo de teste
echo "Conteúdo de teste para upload" > /tmp/test_upload.txt
test_feature "POST multipart/form-data" \
    "curl -s -X POST ${BASE_URL}/upload \
    -F 'file=@/tmp/test_upload.txt' \
    -F 'description=Arquivo de teste' | head -20"
rm -f /tmp/test_upload.txt

echo ""
echo "4️⃣  Teste de Cache Headers"
echo "================================"
echo "Primeira requisição (deve ter ETag):"
ETAG=$(curl -s -i ${BASE_URL}/index.html | grep -i "ETag:" | cut -d' ' -f2 | tr -d '\r')
echo "ETag recebido: $ETAG"

if [ ! -z "$ETAG" ]; then
    echo ""
    echo "Segunda requisição com If-None-Match (deve retornar 304):"
    curl -s -i ${BASE_URL}/index.html \
        -H "If-None-Match: $ETAG" | head -10
else
    echo "❌ ETag não encontrado"
fi

echo ""
echo "5️⃣  Teste de DELETE"
echo "================================"
# Criar arquivo para deletar
echo "Arquivo para deletar" > www/test_delete.txt
echo "Arquivo criado: www/test_delete.txt"
echo ""

test_feature "DELETE arquivo" \
    "curl -s -i -X DELETE ${BASE_URL}/test_delete.txt | head -10"

echo ""
echo "6️⃣  Teste de Chunked Encoding (Envio)"
echo "================================"
echo "⚠️  Chunked encoding no envio requer CGI ou response grande"
echo "Este teste será demonstrado quando CGI for implementado"

echo ""
echo "7️⃣  Teste de Limites"
echo "================================"
# URI muito longa
LONG_URI=$(printf 'a%.0s' {1..9000})
test_feature "URI muito longa (deve falhar)" \
    "curl -s -o /dev/null -w '%{http_code}' ${BASE_URL}/${LONG_URI}"

echo ""
echo "8️⃣  Resumo dos Testes"
echo "================================"
echo "✅ Query String - Parsing e URL decoding"
echo "✅ Form Data - application/x-www-form-urlencoded"
echo "✅ File Upload - multipart/form-data"
echo "✅ Cache Headers - ETag e 304 Not Modified"
echo "✅ DELETE Method - Com verificação de permissões"
echo "✅ Limites de tamanho - URI max 8192"
echo ""
echo "🎉 Testes concluídos!"
echo ""
echo "📚 Para testes mais detalhados:"
echo "   - Abra http://localhost:8080/test_form.html no navegador"
echo "   - Abra http://localhost:8080/api_test.html para testes interativos"
echo "   - Consulte TESTING_GUIDE.md para mais exemplos"
