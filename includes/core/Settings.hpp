/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:40:33 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:40:34 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Settings.hpp
 * Classe Settings para configurações globais do sistema.
 * Fornece funções auxiliares para status HTTP e tipos MIME.
 */
#pragma once

#include <string>
#include <map>
#include "includes/core/Instance.hpp"

/**
 * Classe Settings - Gerenciador de configurações do sistema
 * Implementa o padrão Singleton através da classe Instance
 * Fornece acesso a configurações globais (HTTP status codes, MIME types)
 */
class Settings {
	public:
		/**
		 * Verifica se as configurações estão válidas
		 * @return: true sempre (configurações hard-coded)
		 */
		bool isValid() const;

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
		 */
		Settings();

		// Instance é friend para poder chamar o construtor privado
		friend class Instance;
};


