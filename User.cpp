#include <iostream>
#include "User.h"
#include "SHA256.h"

// struct constructors for different purposes:
User::User(std::string const& _login, std::string const& _password, std::string const& _userName) :
	login(_login), password(_password), userName(_userName) {}
User::User(std::string const& _login, std::string const& _password) :
	login(_login), password(_password) {}
User::User(std::string const& login) :
	login("_all"), password(""), userName("PUBLIC GROUP") {}

std::string hashPassword(const std::string& _password)
{
	SHA256 sha;
	sha.update(_password);
	uint8_t* digest = sha.digest();
	std::string result = SHA256::toString(digest);
	delete[] digest;
	return result;
}

std::string User::getLogin()	const noexcept { return login; }
std::string User::getPassword() const noexcept { return password; }
std::string User::getUserName() const noexcept { return userName; }

void User::setLogin(std::string const& _login) { login = _login; }
void User::setPassword(std::string const& _password) { password = hashPassword(_password); }
void User::setUserName(std::string const& _userName) { userName = _userName; }