# 🐛 PROBLEMAS ENCONTRADOS NOS TESTES

## ✅ CORRIGIDO - Status Code 501 vs 405
**Problema:** Métodos desconhecidos (UNKNOWN, FOOBAR) retornavam 405 em vez de 501
**Causa:** isMethodAllowed() era chamado antes de verificar se o método é implementado
**Solução:** Mover verificação de método implementado para ANTES de verificar rota
**Status:** ✅ CORRIGIDO - Ordem invertida em RequestHandler::handle()

---

## ❌ TODO - POST em Arquivo Estático
**Problema:** POST em /about.html retorna 200 em vez de 405
**Causa:** handlePost() aceita qualquer POST mesmo sem lógica específica
**Expectativa da Avaliação:** POST em arquivos estáticos deve retornar 405
**Solução Proposta:**
- Verificar se o path é um arquivo estático (.html, .css, .js)
- Se for arquivo estático E rota não tem CGI/upload enabled, retornar 405
**Status:** ❌ PENDENTE

---

## ❌ TODO - Directory Listing
**Problema:** GET /uploads/ retorna 404 em vez de mostrar directory listing
**Causa:** Provável problema em resolveFilePath() ou autoindex handling
**Expectativa:** autoindex: on deve listar arquivos do diretório
**Solução Proposta:**
- Verificar se isDirectory() funciona corretamente
- Verificar se autoindex está sendo respeitado
- Verificar generateDirectoryListing()
**Status:** ❌ PENDENTE

---

## ❌ TODO - DELETE de Arquivo Uploaded
**Problema:** DELETE retorna 404 em vez de 200
**Causa:** Arquivo pode não estar sendo salvo corretamente no upload
**Solução Proposta:**
- Verificar handleFileUpload() - onde os arquivos são salvos
- Verificar handleDelete() - se está procurando no path correto
- Testar manualmente: upload -> listar -> delete
**Status:** ❌ PENDENTE

---

## ❌ TODO - File Download após Upload
**Problema:** GET /uploads/ retorna 404 após upload
**Causa:** Relacionado ao Directory Listing - arquivos não estão no path esperado
**Solução Proposta:**
- Verificar se upload_store: ./uploads está criando pasta correta
- Verificar se arquivos estão em www/uploads/ ou apenas uploads/
- Corrigir path resolution
**Status:** ❌ PENDENTE

---

## ⚠️ SKIP - Client Body Size Limit
**Problema:** Request grande não retornou código HTTP
**Causa:** Provavelmente timeout ou problema no parsing
**Expectativa:** Retornar 413 Request Entity Too Large
**Solução Proposta:**
- Verificar se client_max_body_size está sendo respeitado
- Adicionar verificação em Connection::readRequest()
- Retornar 413 quando Content-Length > limit
**Status:** ⚠️ INVESTIGAR

---

## ⚠️ SKIP - Netcat Raw Request
**Problema:** Test com nc falhou
**Causa:** Provavelmente format do response ou timing
**Solução Proposta:**
- Testar manualmente: `echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080`
- Verificar se response tem HTTP/1.1 200 na primeira linha
**Status:** ⚠️ INVESTIGAR

---

## ⚠️ INFO - Siege não instalado
**Problema:** siege não está instalado
**Solução:** `brew install siege`
**Status:** ⚠️ DEPENDÊNCIA EXTERNA

---

## 📊 RESUMO

| Categoria | Quantidade |
|-----------|------------|
| ✅ Corrigido | 1 |
| ❌ Pendente | 4 |
| ⚠️ Investigar | 3 |
| **Total** | **8** |

---

## 🎯 PRIORIDADE DE CORREÇÃO

1. **ALTA:** Directory Listing (afeta múltiplos testes)
2. **ALTA:** File Upload Path (afeta download e delete)
3. **MÉDIA:** POST em arquivo estático (edge case discutível)
4. **MÉDIA:** Client Body Size Limit (security feature)
5. **BAIXA:** Netcat test (provavelmente timing issue)

---

## 📝 PRÓXIMOS PASSOS

1. Recompilar com correção do 501
2. Testar método desconhecido: `curl -X UNKNOWN http://localhost:8080/`
3. Investigar directory listing: `curl http://localhost:8080/uploads/`
4. Testar upload manualmente: `curl -X POST http://localhost:8080/upload -F "file=@test.txt"`
5. Listar arquivos após upload: `ls -la www/uploads/`
6. Corrigir path resolution se necessário
