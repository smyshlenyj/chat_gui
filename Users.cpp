#include "Users.h"
#include "User.h"
#include <fstream>
#include <sstream>
#include <filesystem>

Users::Users()
{
	std::ifstream readFromDB;

	readFromDB.open("users.mdf", std::ios::in);
	if (!std::filesystem::exists("users.mdf"))
		std::ofstream outfile("users.mdf"); //create file in case it's not there

	while (!readFromDB.eof())
	{
		std::string userRecord;

		std::getline(readFromDB, userRecord);
		if (!userRecord.empty())
		{
			std::istringstream ss(userRecord);
			std::string token;
			int i = 0; // iterator for while
			std::string array[3];
			while (std::getline(ss, token, '\t'))
			{
				array[i] = token;
				++i;
			}
			User tempUser = { array[0], array[1], array[2] };
			users.push_back(tempUser);
		}
	}
}

bool Users::uniqueLogin(std::string const& login) // check login for uniqueness
{
	for (auto& i : users)
	{
		if (i.getLogin() == login)
			return false;
	}

	return true;
}

std::vector<User> Users::listOfUsers() // just prints all user names and logins
{
	return users;
}

bool Users::loginAndPasswordMatch(User const& _user) // check is login matching the password
{
	for (auto& i : users)
	{
		if (i.getLogin() == _user.getLogin() && i.getPassword() == _user.getPassword())
			return true;
	}

	return false;
}

bool Users::loginAndPasswordMatch(std::string const& login, std::string const& password) // check is login matching the password
{
	for (auto& i : users)
	{
		if (i.getLogin() == login && i.getPassword() == hashPassword(password))
			return true;
	}

	return false;
}

std::string Users::findUserNameByLogin(std::string const& login)
{
	for (auto& i : users)
	{
		if (i.getLogin() == login)
			return i.getUserName();
	}
	return "";
}

void Users::addUser(User const& user)
{
	std::ofstream out("users.mdf", std::ios::app); // add user record to data base
	if (out.is_open())
		out << user.getLogin() + "\t" + user.getPassword() + "\t" + user.getUserName() << std::endl;  //add new user to file
}