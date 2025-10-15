# 🧪 Sistema de Testes Automáticos - webserv

Este diretório contém um sistema completo de testes automáticos para o webserv.

## 📋 Índice

- [Início Rápido](#início-rápido)
- [Scripts Disponíveis](#scripts-disponíveis)
- [Usando o Makefile](#usando-o-makefile)
- [Testes Individuais](#testes-individuais)
- [Testes de Performance](#testes-de-performance)
- [Testes com Valgrind](#testes-com-valgrind)

---

## 🚀 Início Rápido

### 1. Compilar o servidor

```bash
cd /home/strodrig/webserv
make
```

### 2. Iniciar o servidor em um terminal

```bash
./webserv config/default.conf
```

### 3. Em outro terminal, executar os testes

```bash
# Testes funcionais completos
./tests/run_tests.sh

# OU usando o Makefile
cd tests
make test
```

---

## 📁 Scripts Disponíveis

### 1. `run_tests.sh` - Testes Funcionais Completos

Testa **todas** as funcionalidades implementadas:

```bash
chmod +x tests/run_tests.sh
./tests/run_tests.sh
```

**O que é testado:**
- ✅ Query String parsing
- ✅ Form Data (POST)
- ✅ File Upload (multipart/form-data)
- ✅ Cache Headers (ETag, Last-Modified, 304)
- ✅ DELETE Method
- ✅ Limites de tamanho (URI, Headers)
- ✅ Content-Type
- ✅ Error handling (404, 403, 405)

**Saída:**
- Relatório colorido com ✓ (passou) ou ✗ (falhou)
- Contador de testes passados/falhados
- Taxa de sucesso percentual

---

### 2. `stress_tests.sh` - Testes de Performance

Testa o servidor sob carga:

```bash
chmod +x tests/stress_tests.sh
./tests/stress_tests.sh
```

**O que é testado:**
- 100 requisições sequenciais
- Requisições concorrentes
- Upload de arquivos grandes (1MB, 5MB)
- 50+ conexões simultâneas
- Testes com Apache Bench (se instalado)
- Testes com Siege (se instalado)

---

### 3. `test_feature.sh` - Testes Individuais

Testa uma funcionalidade específica com saída detalhada:

```bash
chmod +x tests/test_feature.sh

# Testar query strings
./tests/test_feature.sh query

# Testar upload de arquivos
./tests/test_feature.sh upload

# Testar cache
./tests/test_feature.sh cache

# Testar DELETE
./tests/test_feature.sh delete

# Todas as funcionalidades
./tests/test_feature.sh all
```

**Funcionalidades disponíveis:**
- `query` - Query string parsing
- `form` - Form data (POST)
- `upload` - File upload
- `cache` - Cache headers
- `delete` - DELETE method
- `chunked` - Chunked encoding
- `limits` - Size limits
- `all` - Todos os testes

---

## 🛠️ Usando o Makefile

O Makefile fornece comandos convenientes para executar os testes:

```bash
cd tests

# Ver todos os comandos disponíveis
make help

# Testes funcionais
make test

# Testes de stress
make stress

# Todos os testes
make test-all

# Teste rápido (verificação básica)
make test-quick

# Cobertura de funcionalidades
make test-coverage

# Iniciar servidor para testes
make test-server

# Limpar arquivos de teste
make test-clean
```

---

## 🔍 Testes Individuais por Funcionalidade

### Teste de Query String

```bash
./tests/test_feature.sh query
```

**Exemplos testados:**
- `?name=João&age=25&city=Lisboa`
- `?message=Olá%20Mundo` (URL encoding)
- Múltiplos parâmetros
- Query vazia

---

### Teste de Form Data

```bash
./tests/test_feature.sh form
```

**Exemplos testados:**
- Form simples com name, email, message
- Caracteres especiais (+ e %XX)
- Múltiplos campos

---

### Teste de Upload

```bash
./tests/test_feature.sh upload
```

**Exemplos testados:**
- Upload de arquivo texto pequeno
- Upload de arquivo binário (50KB)
- Upload múltiplo de arquivos
- Upload com campos adicionais

---

### Teste de Cache

```bash
./tests/test_feature.sh cache
```

**O que é verificado:**
- Header `ETag` está presente
- Header `Last-Modified` está presente
- Header `Cache-Control` está presente
- Requisição com `If-None-Match` retorna 304
- 304 não tem body

---

### Teste de DELETE

```bash
./tests/test_feature.sh delete
```

**O que é testado:**
- DELETE de arquivo existente → 204 No Content
- Arquivo é removido do sistema de arquivos
- DELETE de arquivo inexistente → 404
- DELETE de arquivo sem permissão → 403

---

## ⚡ Testes de Performance

### Com Apache Bench (ab)

```bash
# Instalar (se necessário)
sudo apt install apache2-utils

# 1000 requisições, 10 concorrentes
ab -n 1000 -c 10 http://localhost:8080/index.html

# POST com dados
ab -n 100 -c 10 -p post_data.txt -T "application/x-www-form-urlencoded" http://localhost:8080/test
```

### Com Siege

```bash
# Instalar (se necessário)
sudo apt install siege

# 10 usuários, 100 requisições cada
siege -c 10 -r 100 http://localhost:8080/index.html

# Teste de 30 segundos
siege -c 20 -t 30S http://localhost:8080/index.html
```

---

## 🔬 Testes com Valgrind (Memory Leaks)

### Método 1: Manual

```bash
# Terminal 1: Iniciar servidor com valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./webserv config/default.conf

# Terminal 2: Executar testes
./tests/run_tests.sh

# Terminal 1: Parar servidor (Ctrl+C) e verificar log
cat valgrind-out.txt
```

### Método 2: Usando Makefile

```bash
cd tests
make test-valgrind
```

Este comando:
1. Inicia o servidor com valgrind
2. Executa todos os testes
3. Para o servidor
4. Exibe o relatório do valgrind

**O que procurar no relatório:**
- `definitely lost: 0 bytes` ✅ (bom)
- `indirectly lost: 0 bytes` ✅ (bom)
- `possibly lost: 0 bytes` ✅ (bom)
- `still reachable: X bytes` ⚠️ (aceitável se pequeno)

---

## 📊 Interpretando os Resultados

### Testes Funcionais

```
✓ PASSOU: Query string básica processada
✗ FALHOU: Upload de arquivo médio realizado
  Esperado: 201
  Recebido: 500
```

- **✓ Verde** = Teste passou
- **✗ Vermelho** = Teste falhou (com detalhe do erro)
- **⊘ Amarelo** = Teste pulado
- **⚠ Amarelo** = Aviso (passou mas com observação)

### Resumo Final

```
Testes Executados: 45
Testes Passou: 43
Testes Falhou: 2
Taxa de Sucesso: 95%
```

---

## 🐛 Debugging de Testes Falhados

### 1. Ver logs do servidor

```bash
# Aumentar verbosidade dos logs
# Verifique a saída do servidor enquanto os testes rodam
```

### 2. Executar teste individual

```bash
# Teste específico com saída detalhada
./tests/test_feature.sh upload
```

### 3. Teste manual com curl verbose

```bash
curl -v -X POST http://localhost:8080/upload \
  -F "file=@test.txt"
```

### 4. Verificar arquivos criados

```bash
# Arquivos de upload
ls -la ./uploads/

# Logs de teste
ls -la /tmp/webserv_tests_*
```

---

## 📝 Checklist de Testes

Antes de submeter o projeto:

- [ ] `make test` - Todos os testes funcionais passam
- [ ] `make stress` - Servidor aguenta carga
- [ ] `make test-valgrind` - Zero memory leaks
- [ ] Teste manual com navegador (Chrome/Firefox)
- [ ] Teste com telnet
- [ ] Teste comparativo com NGINX

---

## 🔧 Troubleshooting

### Problema: "Servidor não está rodando"

**Solução:**
```bash
# Terminal 1
./webserv config/default.conf

# Terminal 2
./tests/run_tests.sh
```

### Problema: "Permission denied"

**Solução:**
```bash
chmod +x tests/*.sh
```

### Problema: Testes falham aleatoriamente

**Possíveis causas:**
- Servidor sobrecarregado
- Porta 8080 já em uso
- Firewall bloqueando conexões

**Solução:**
```bash
# Verificar se porta está livre
lsof -i :8080

# Matar processos na porta
killall webserv
```

### Problema: Upload de arquivos falha

**Verificar:**
```bash
# Diretório de upload existe?
mkdir -p ./uploads
chmod 755 ./uploads

# Limite de tamanho configurado?
# Ver config/default.conf: max_body_size
```

---

## 📚 Recursos Adicionais

- **TESTING_GUIDE.md** - Guia completo de testes manuais
- **www/test_form.html** - Interface web para testes
- **www/api_test.html** - Testes JavaScript interativos
- **IMPLEMENTATION_SUMMARY.md** - Documentação técnica

---

## 🎯 Próximos Passos

Depois que todos os testes passarem:

1. ✅ Executar `make test-valgrind` para verificar memory leaks
2. ✅ Testar com navegadores reais (Chrome, Firefox, Safari)
3. ✅ Comparar comportamento com NGINX
4. ✅ Executar stress tests prolongados
5. ✅ Testar edge cases específicos do subject

---

**Última atualização:** 15 Outubro 2025
**Desenvolvido para:** webserv (42 School)
