/**
 * Route.cpp
 * Implementação da classe Route
 */
#include "includes/Route.hpp"
#include "includes/utils/Logger.hpp"
#include <iostream>
#include <algorithm>

// Constructors
Route::Route()
	: _path("/")
	, _redirect("")
	, _root("")
	, _directoryListing(false)
	, _cgiEnabled(false)
	, _cgiPath("")
	, _cgiExtension("")
	, _uploadEnabled(false)
	, _uploadPath("") {
	// Por default, permitir GET
	_allowedMethods.push_back("GET");
}

Route::Route(const std::string& path)
	: _path(path)
	, _redirect("")
	, _root("")
	, _directoryListing(false)
	, _cgiEnabled(false)
	, _cgiPath("")
	, _cgiExtension("")
	, _uploadEnabled(false)
	, _uploadPath("") {
	// Por default, permitir GET
	_allowedMethods.push_back("GET");
}

Route::~Route() {}

Route::Route(const Route& other) {
	*this = other;
}

Route& Route::operator=(const Route& other) {
	if (this != &other) {
		_path = other._path;
		_allowedMethods = other._allowedMethods;
		_redirect = other._redirect;
		_root = other._root;
		_directoryListing = other._directoryListing;
		_indexFiles = other._indexFiles;
		_cgiEnabled = other._cgiEnabled;
		_cgiPath = other._cgiPath;
		_cgiExtension = other._cgiExtension;
		_uploadEnabled = other._uploadEnabled;
		_uploadPath = other._uploadPath;
	}
	return *this;
}

// Getters
const std::string& Route::getPath() const { return _path; }
const std::vector<std::string>& Route::getAllowedMethods() const { return _allowedMethods; }
const std::string& Route::getRedirect() const { return _redirect; }
const std::string& Route::getRoot() const { return _root; }
bool Route::isDirectoryListingEnabled() const { return _directoryListing; }
const std::vector<std::string>& Route::getIndexFiles() const { return _indexFiles; }
bool Route::isCgiEnabled() const { return _cgiEnabled; }
const std::string& Route::getCgiPath() const { return _cgiPath; }
const std::string& Route::getCgiExtension() const { return _cgiExtension; }
bool Route::isUploadEnabled() const { return _uploadEnabled; }
const std::string& Route::getUploadPath() const { return _uploadPath; }

// Setters
void Route::setPath(const std::string& path) {
	_path = path;
}

void Route::addAllowedMethod(const std::string& method) {
	// Verificar se já existe
	for (size_t i = 0; i < _allowedMethods.size(); ++i) {
		if (_allowedMethods[i] == method)
			return;
	}
	_allowedMethods.push_back(method);
}

void Route::setRedirect(const std::string& redirect) {
	_redirect = redirect;
}

void Route::setRoot(const std::string& root) {
	_root = root;
}

void Route::setDirectoryListing(bool enabled) {
	_directoryListing = enabled;
}

void Route::addIndexFile(const std::string& indexFile) {
	_indexFiles.push_back(indexFile);
}

void Route::setCgiEnabled(bool enabled) {
	_cgiEnabled = enabled;
}

void Route::setCgiPath(const std::string& cgiPath) {
	_cgiPath = cgiPath;
}

void Route::setCgiExtension(const std::string& extension) {
	_cgiExtension = extension;
}

void Route::setUploadEnabled(bool enabled) {
	_uploadEnabled = enabled;
}

void Route::setUploadPath(const std::string& uploadPath) {
	_uploadPath = uploadPath;
}

// Validation
bool Route::isMethodAllowed(const std::string& method) const {
	for (size_t i = 0; i < _allowedMethods.size(); ++i) {
		if (_allowedMethods[i] == method)
			return true;
	}
	return false;
}

bool Route::isValid() const {
	// Uma route precisa pelo menos de um path
	if (_path.empty())
		return false;

	// Se CGI está enabled, precisa de cgiPath e extension
	if (_cgiEnabled && (_cgiPath.empty() || _cgiExtension.empty()))
		return false;

	// Se upload está enabled, precisa de uploadPath
	if (_uploadEnabled && _uploadPath.empty())
		return false;

	return true;
}

// Debug
void Route::print() const {
	std::cout << "  Route: " << _path << std::endl;

	std::cout << "    Allowed methods: ";
	for (size_t i = 0; i < _allowedMethods.size(); ++i) {
		std::cout << _allowedMethods[i];
		if (i < _allowedMethods.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	if (!_redirect.empty())
		std::cout << "    Redirect: " << _redirect << std::endl;

	if (!_root.empty())
		std::cout << "    Root: " << _root << std::endl;

	std::cout << "    Directory listing: " << (_directoryListing ? "on" : "off") << std::endl;

	if (!_indexFiles.empty()) {
		std::cout << "    Index files: ";
		for (size_t i = 0; i < _indexFiles.size(); ++i) {
			std::cout << _indexFiles[i];
			if (i < _indexFiles.size() - 1)
				std::cout << ", ";
		}
		std::cout << std::endl;
	}

	if (_cgiEnabled) {
		std::cout << "    CGI enabled: yes" << std::endl;
		std::cout << "    CGI path: " << _cgiPath << std::endl;
		std::cout << "    CGI extension: " << _cgiExtension << std::endl;
	}

	if (_uploadEnabled) {
		std::cout << "    Upload enabled: yes" << std::endl;
		std::cout << "    Upload path: " << _uploadPath << std::endl;
	}
}
