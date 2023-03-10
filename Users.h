#pragma once
#include <vector>
#include <string>
#include "User.h"

struct Users   // read and store all user records
{
private:
	std::vector<User> users;

public:
	Users();

	bool uniqueLogin(std::string const&);
	std::vector<User> listOfUsers();
	bool loginAndPasswordMatch(User const&); // check is login matching the password
	bool loginAndPasswordMatch(std::string const&, std::string const&); // check is login matching the password
	std::string findUserNameByLogin(std::string const&);
	void addUser(User const&);
};