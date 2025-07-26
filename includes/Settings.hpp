/**
 * Settings.hpp
 * Classe Settings usando Instance que carrega e interpreta um arquivo de configurações YAML.
 * Isso é útil para armazenar configurações usadas em todo o sistema, como códigos de status HTTP.
 * Possui alguns métodos auxiliares para tipos mime e códigos de status, todo o resto é acessado via get<T>.
 */
#pragma once

#include "webserv.hpp"

/**
 * Classe Settings - Gerenciador de configurações do sistema
 * Implementa o padrão Singleton através da classe Instance
 * Carrega e interpreta um arquivo YAML com configurações globais
 * Fornece acesso tipado às configurações através de caminhos hierárquicos
 */
class Settings {
	// Caminho para o arquivo de configuração YAML
	static const char* path;

	public:
		/**
		 * Verifica se as configurações foram carregadas corretamente
		 * @return: true se o arquivo de configuração foi carregado com sucesso
		 */
		bool isValid() const;

		/**
		 * Método template para acessar configurações por caminho hierárquico
		 * Permite acesso a valores aninhados usando notação de ponto (ex: "server.port")
		 * @param path: Caminho para a configuração (ex: "misc.timeout", "server.ports.0")
		 * @return: Valor da configuração convertido para o tipo T
		 *
		 * Exemplos de uso:
		 * - get<int>("server.port") -> retorna porta como inteiro
		 * - get<string>("server.host") -> retorna host como string
		 * - get<bool>("debug.enabled") -> retorna flag booleana
		 */
		template <typename T>
		T get(const std::string& path) const {
		// Divide o caminho em partes usando "." como separador
		std::vector<std::string> parts = Utils::split(path, ".");
		const YAML::Node* node = &this->config;

		// Navega pela hierarquia YAML seguindo o caminho
		for (
		  size_t i = 0;
		  i < parts.size() && node->isValid();
		  ++i
		  ) {
		  const std::string& key = parts[i];

		  // Verifica se é um índice numérico para arrays/sequências
		  if (
			Utils::isInteger(key, true) &&
			node->is<YAML::Types::Sequence>() &&
			node->has(std::atoi(key.c_str())))
			node = &node->get(std::atoi(key.c_str()));
		  else
			// Acessa por chave string para mapas/objetos
			node = &node->get(key);
		}
		return node->as<T>();
		}

		/**
		 * Obtém a mensagem de status HTTP correspondente a um código
		 * @param code: Código de status HTTP (ex: 200, 404, 500)
		 * @return: String com a mensagem do status (ex: "OK", "Not Found", "Internal Server Error")
		 */
		const std::string httpStatusCode(int code) const;

		/**
		 * Obtém o tipo MIME baseado na extensão do arquivo
		 * @param ext: Extensão do arquivo (ex: "html", "css", "js", "png")
		 * @return: String com o tipo MIME correspondente (ex: "text/html", "image/png")
		 */
		const std::string& httpMimeType(const std::string& ext) const;

	private:
		/**
		 * Construtor privado - apenas Instance pode criar uma instância
		 * Carrega e parseia o arquivo de configuração YAML
		 */
		Settings();

		// Árvore de configuração YAML parseada
		const YAML::Node config;

		// Instance é friend para poder chamar o construtor privado
		friend class Instance;
};


