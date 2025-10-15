# 🚀 GUIA RÁPIDO - Como Testar o webserv

## ⚡ 3 Passos Rápidos

### 1️⃣ Compilar
```bash
make
```

### 2️⃣ Iniciar Servidor (Terminal 1)
```bash
./webserv config/default.conf
```

### 3️⃣ Executar Testes (Terminal 2)
```bash
./tests/run_tests.sh
```

---

## 📋 Comandos Úteis

### Testes Completos
```bash
# Dar permissão aos scripts
chmod +x tests/*.sh

# Testes funcionais (principais)
./tests/run_tests.sh

# Testes de performance/stress
./tests/stress_tests.sh

# Usando Makefile
cd tests && make test
```

### Testes Individuais
```bash
# Testar apenas query strings
./tests/test_feature.sh query

# Testar apenas upload
./tests/test_feature.sh upload

# Testar cache
./tests/test_feature.sh cache

# Ver todas opções
./tests/test_feature.sh
```

### Teste Rápido (5 segundos)
```bash
cd tests && make test-quick
```

---

## 🌐 Testes no Navegador

Abra no seu navegador:

```
http://localhost:8080/test_form.html
http://localhost:8080/api_test.html
```

---

## 🔬 Verificar Memory Leaks

```bash
# Opção 1: Automático
cd tests && make test-valgrind

# Opção 2: Manual
valgrind --leak-check=full ./webserv config/default.conf
# Em outro terminal:
./tests/run_tests.sh
```

---

## ✅ O Que é Testado

- ✅ Query strings (`?name=value&key=data`)
- ✅ POST com form data
- ✅ Upload de arquivos (multipart)
- ✅ Cache headers (ETag, Last-Modified)
- ✅ DELETE method
- ✅ Chunked encoding
- ✅ Limites de tamanho
- ✅ Error handling

---

## 📊 Resultado Esperado

```
═══════════════════════════════════════════════════════════
  RESULTADOS FINAIS
═══════════════════════════════════════════════════════════

Testes Executados: 45
Testes Passou: 45
Testes Falhou: 0
Taxa de Sucesso: 100%

═══════════════════════════════════════════════════════════
   ✓✓✓ TODOS OS TESTES PASSARAM! ✓✓✓
═══════════════════════════════════════════════════════════
```

---

## 🐛 Problemas Comuns

### "Servidor não está rodando"
```bash
# Inicie o servidor primeiro
./webserv config/default.conf
```

### "Permission denied"
```bash
chmod +x tests/*.sh
```

### Porta 8080 já em uso
```bash
# Matar processos
killall webserv
# Ou mudar porta no config/default.conf
```

---

## 📚 Documentação Completa

- `tests/README.md` - Documentação completa dos testes
- `TESTING_GUIDE.md` - Guia de testes manuais
- `IMPLEMENTATION_SUMMARY.md` - Funcionalidades implementadas

---

**Pronto! Agora você pode testar todas as funcionalidades do webserv! 🎉**
