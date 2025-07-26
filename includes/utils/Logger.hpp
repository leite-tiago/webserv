/**
 * Logger.hpp
 * Basic logging utility.
 * Supports different log levels and output formats.
 * If you want a parameter highlighted, use Logger::param("your parameter") method.
 */
#pragma once

#include "../webserv.hpp"

// Colors
#define RESET	"\033[39m"
#define BLACK	"\033[30m"
#define DARK_RED	"\033[31m"
#define DARK_GREEN	"\033[32m"
#define DARK_YELLOW	"\033[33m"
#define DARK_BLUE	"\033[34m"
#define DARK_MAGENTA	"\033[35m"
#define DARK_CYAN	"\033[36m"
#define LIGHT_GRAY	"\033[37m"
#define DARK_GRAY	"\033[90m"
#define RED	"\033[91m"
#define GREEN	"\033[92m"
#define ORANGE	"\033[93m"
#define BLUE	"\033[94m"
#define MAGENTA	"\033[95m"
#define CYAN	"\033[96m"
#define WHITE	"\033[97m"

namespace Logger {

	/**
	 * Função template para formatar parâmetros com cor de destaque
	 * @param value: O valor a ser formatado (pode ser qualquer tipo que suporte <<)
	 * @return: String formatada com cor laranja e reset ao final
	 */
	template<typename T>
	std::string param(const T& value) {
		std::stringstream ss;
		ss << ORANGE << value << RESET;
		return ss.str();
	}

	/**
	 * Função para obter string de erro formatada
	 * Utiliza errno para obter a descrição do último erro do sistema
	 * @return: String de erro formatada com cor de destaque
	 */
	inline std::string errstr() {
		return param(std::strerror(errno));
	}

	/**
	 * Classe Stream - Responsável por gerenciar diferentes tipos de log
	 * Cada instância representa um nível de log diferente (info, debug, warning, etc.)
	 * Permite formatação personalizada com cores e cabeçalhos
	 */
	class Stream {
		private:
			std::string header;		// Cabeçalho do log (ex: "[INFO]", "[ERROR]")
			std::ostream& out;		// Stream de saída (geralmente std::cout ou std::cerr)
			std::string color;		// Cor associada a este tipo de log
			bool enabled;			// Se este tipo de log está habilitado
			std::stringstream vstream;	// Stream virtual para quando o log está desabilitado

		public:
			/**
			 * Construtor com stream de saída personalizada
			 * @param out: Stream de saída (ex: std::cout, std::cerr)
			 * @param header: Texto do cabeçalho do log
			 * @param color: Código de cor ANSI para este tipo de log
			 * @param enabled: Se este logger está ativo
			 */
			 Stream(
				std::ostream& out,
				const std::string& header,
				const std::string& color,
				bool enabled = true
			 );

			/**
			 * Construtor usando std::cout como saída padrão
			 * @param header: Texto do cabeçalho do log
			 * @param color: Código de cor ANSI para este tipo de log
			 * @param enabled: Se este logger está ativo
			 */
			 Stream(
				const std::string& header,
				const std::string& color,
				bool enabled = true
			 );

			// Destrutor
			~Stream();

			// Construtor de cópia
			Stream(const Stream& other);

			// Operador de atribuição
			Stream& operator=(const Stream& other);

			/**
			 * Operador de inserção (<<) - usado para escrever no log
			 * @param value: Valor a ser logado (qualquer tipo que suporte <<)
			 * @return: Referência para o stream de saída
			 * Se o logger estiver desabilitado, usa um stream virtual (sem saída)
			 */
			template<typename T>
			std::ostream& operator<<(const T& value) {
				if (!this->enabled) {
					this->vstream.clear();
					return this->vstream;
				};
				return this->out << this->buildHeader() << " " << value;
			}

			/**
			 * Operador de extração (>>) - atalho para formatar parâmetros
			 * @param value: Valor a ser formatado com cor de destaque
			 * @return: String formatada
			 * Permite uso como: logger >> "parametro" para destacar texto
			 */
			template<typename T>
			std::string operator>>(const T& value) const {
				return Logger::param(value);
			}

			private:
			/**
			 * Constrói o cabeçalho completo do log
			 * Inclui timestamp, cor e texto do cabeçalho
			 * @return: String formatada do cabeçalho
			 */
			std::string buildHeader() const;

			/**
			 * Obtém timestamp atual formatado
			 * @return: String com data/hora atual
			 */
			std::string getTime() const;
	};

	// Instâncias pré-definidas de loggers para diferentes níveis
	// Estas são declaradas como extern, implementadas em Logger.cpp

	extern Stream info;		// Logger para informações gerais (cor azul)
	extern Stream debug;	// Logger para debug/desenvolvimento (cor cinza)
	extern Stream warning;	// Logger para avisos (cor amarela)
	extern Stream error;	// Logger para erros (cor vermelha)
	extern Stream success;	// Logger para operações bem-sucedidas (cor verde)

	extern Stream child;	// Logger especial para processos filho
}

