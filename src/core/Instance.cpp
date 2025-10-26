/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Instance.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:46 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:47 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Instance.cpp
 * Implementation of Instance singleton pattern
 */
#include "includes/core/Instance.hpp"

// Static member definition
std::map<std::size_t, void*> Instance::instances_;
