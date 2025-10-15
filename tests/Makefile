# =============================================================================
# WEBSERV - Makefile de Testes
# =============================================================================
# Comandos disponíveis:
#   make test          - Executa testes funcionais
#   make stress        - Executa testes de stress/performance
#   make test-all      - Executa todos os testes
#   make test-valgrind - Executa servidor com valgrind e roda testes
#   make test-clean    - Limpa arquivos de teste
# =============================================================================

.PHONY: test stress test-all test-valgrind test-clean help

# Configuração
SERVER = ../webserv
CONFIG = ../config/default.conf
TESTS_DIR = .
SERVER_URL = http://localhost:8080

# Cores
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
RED = \033[0;31m
NC = \033[0m

help:
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(BLUE)  WEBSERV - Sistema de Testes$(NC)"
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  $(GREEN)make test$(NC)          - Testes funcionais automáticos"
	@echo "  $(GREEN)make stress$(NC)        - Testes de performance/stress"
	@echo "  $(GREEN)make test-all$(NC)      - Todos os testes"
	@echo "  $(GREEN)make test-valgrind$(NC) - Testes com valgrind (memory leaks)"
	@echo "  $(GREEN)make test-clean$(NC)    - Limpar arquivos de teste"
	@echo "  $(GREEN)make test-server$(NC)   - Iniciar servidor para testes"
	@echo ""

# Executar testes funcionais
test:
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(BLUE)  Executando Testes Funcionais$(NC)"
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@chmod +x $(TESTS_DIR)/run_tests.sh
	@if pgrep -f "webserv" > /dev/null; then \
		$(TESTS_DIR)/run_tests.sh; \
	else \
		echo "$(RED)✗ Servidor não está rodando!$(NC)"; \
		echo "Inicie o servidor com: make test-server"; \
		echo "Em outro terminal execute: make test"; \
		exit 1; \
	fi

# Executar testes de stress
stress:
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(BLUE)  Executando Testes de Stress$(NC)"
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@chmod +x $(TESTS_DIR)/stress_tests.sh
	@if pgrep -f "webserv" > /dev/null; then \
		$(TESTS_DIR)/stress_tests.sh; \
	else \
		echo "$(RED)✗ Servidor não está rodando!$(NC)"; \
		echo "Inicie o servidor com: make test-server"; \
		exit 1; \
	fi

# Executar todos os testes
test-all: test stress
	@echo ""
	@echo "$(GREEN)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  Todos os testes concluídos!$(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════$(NC)"

# Iniciar servidor para testes
test-server:
	@echo "$(BLUE)Iniciando servidor de testes...$(NC)"
	@echo "Servidor: $(SERVER)"
	@echo "Config: $(CONFIG)"
	@echo ""
	@echo "$(YELLOW)Pressione Ctrl+C para parar o servidor$(NC)"
	@echo ""
	@$(SERVER) $(CONFIG)

# Testes com valgrind
test-valgrind:
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo "$(BLUE)  Testes com Valgrind (Memory Leak Detection)$(NC)"
	@echo "$(BLUE)═══════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Iniciando servidor com valgrind...$(NC)"
	@echo "Isso pode ser lento. Aguarde..."
	@echo ""
	@valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=valgrind-out.txt \
		$(SERVER) $(CONFIG) & \
	SERVER_PID=$$!; \
	sleep 2; \
	echo "$(GREEN)Servidor iniciado (PID: $$SERVER_PID)$(NC)"; \
	echo "Executando testes..."; \
	chmod +x $(TESTS_DIR)/run_tests.sh; \
	$(TESTS_DIR)/run_tests.sh; \
	echo ""; \
	echo "$(YELLOW)Parando servidor...$(NC)"; \
	kill $$SERVER_PID; \
	sleep 1; \
	echo ""; \
	echo "$(GREEN)Relatório do Valgrind:$(NC)"; \
	cat valgrind-out.txt; \
	echo ""; \
	echo "Relatório salvo em: valgrind-out.txt"

# Limpar arquivos de teste
test-clean:
	@echo "$(YELLOW)Limpando arquivos de teste...$(NC)"
	@rm -rf /tmp/webserv_tests_*
	@rm -f www/test_*.txt
	@rm -f www/temp*.txt
	@rm -f valgrind-out.txt
	@rm -rf uploads/*
	@echo "$(GREEN)✓ Limpeza concluída$(NC)"

# Teste rápido (apenas verificação básica)
test-quick:
	@echo "$(BLUE)Teste Rápido$(NC)"
	@echo ""
	@echo "1. Verificando servidor..."
	@if curl -s -o /dev/null -w "%{http_code}" $(SERVER_URL) | grep -q "200"; then \
		echo "$(GREEN)✓ Servidor respondendo$(NC)"; \
	else \
		echo "$(RED)✗ Servidor não está respondendo$(NC)"; \
		exit 1; \
	fi
	@echo ""
	@echo "2. Teste GET..."
	@if curl -s $(SERVER_URL)/index.html | grep -q "html"; then \
		echo "$(GREEN)✓ GET funcionando$(NC)"; \
	else \
		echo "$(RED)✗ GET falhou$(NC)"; \
	fi
	@echo ""
	@echo "3. Teste POST..."
	@if curl -s -X POST $(SERVER_URL)/test -d "test=data" | grep -q "POST"; then \
		echo "$(GREEN)✓ POST funcionando$(NC)"; \
	else \
		echo "$(RED)✗ POST falhou$(NC)"; \
	fi
	@echo ""
	@echo "4. Teste Query String..."
	@if curl -s "$(SERVER_URL)/test?name=test" | grep -q "name"; then \
		echo "$(GREEN)✓ Query String funcionando$(NC)"; \
	else \
		echo "$(RED)✗ Query String falhou$(NC)"; \
	fi
	@echo ""
	@echo "$(GREEN)✓ Teste rápido concluído!$(NC)"

# Estatísticas de cobertura
test-coverage:
	@echo "$(BLUE)Estatísticas de Funcionalidades$(NC)"
	@echo ""
	@echo "Testando funcionalidades implementadas:"
	@echo ""
	@TOTAL=0; PASSED=0; \
	echo -n "  Query String............ "; \
	if curl -s "$(SERVER_URL)/test?key=value" > /dev/null 2>&1; then \
		echo "$(GREEN)✓$(NC)"; PASSED=$$((PASSED+1)); \
	else \
		echo "$(RED)✗$(NC)"; \
	fi; \
	TOTAL=$$((TOTAL+1)); \
	echo -n "  Form Data (POST)........ "; \
	if curl -s -X POST $(SERVER_URL)/test -d "test=data" > /dev/null 2>&1; then \
		echo "$(GREEN)✓$(NC)"; PASSED=$$((PASSED+1)); \
	else \
		echo "$(RED)✗$(NC)"; \
	fi; \
	TOTAL=$$((TOTAL+1)); \
	echo -n "  File Upload............. "; \
	echo "test" > /tmp/test_upload.txt; \
	if curl -s -X POST $(SERVER_URL)/upload -F "file=@/tmp/test_upload.txt" > /dev/null 2>&1; then \
		echo "$(GREEN)✓$(NC)"; PASSED=$$((PASSED+1)); \
	else \
		echo "$(RED)✗$(NC)"; \
	fi; \
	rm -f /tmp/test_upload.txt; \
	TOTAL=$$((TOTAL+1)); \
	echo -n "  Cache Headers........... "; \
	if curl -s -i $(SERVER_URL)/index.html | grep -q "ETag"; then \
		echo "$(GREEN)✓$(NC)"; PASSED=$$((PASSED+1)); \
	else \
		echo "$(RED)✗$(NC)"; \
	fi; \
	TOTAL=$$((TOTAL+1)); \
	echo -n "  DELETE Method........... "; \
	echo "test" > www/temp_del.txt; \
	if curl -s -X DELETE $(SERVER_URL)/temp_del.txt > /dev/null 2>&1; then \
		echo "$(GREEN)✓$(NC)"; PASSED=$$((PASSED+1)); \
	else \
		echo "$(RED)✗$(NC)"; \
	fi; \
	TOTAL=$$((TOTAL+1)); \
	echo ""; \
	PERCENT=$$((PASSED * 100 / TOTAL)); \
	echo "Cobertura: $$PASSED/$$TOTAL ($$PERCENT%)"

.DEFAULT_GOAL := help
