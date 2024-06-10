// Fill out your copyright notice in the Description page of Project Settings.


#include "MySQLConnector.h"
#include <iostream>

MySQLConnector::MySQLConnector()
    : Driver(sql::mysql::get_mysql_driver_instance()), Connection(nullptr)
{
}

MySQLConnector::~MySQLConnector()
{
    Disconnect();
}

bool MySQLConnector::Connect()
{
    try
    {
        Connection.reset(Driver->connect("tcp://127.0.0.1:3306", "root", "your_password"));
        Connection->setSchema("GameDB");
        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what() << std::endl;
        return false;
    }
}

void MySQLConnector::Disconnect()
{
    if (Connection)
    {
        Connection->close();
    }
}

bool MySQLConnector::RegisterUser(const std::string& Username, const std::string& Password)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> PreparedStatement(Connection->prepareStatement("INSERT INTO Users (Username, Password) VALUES (?, ?)"));
        PreparedStatement->setString(1, Username);
        PreparedStatement->setString(2, Password);
        PreparedStatement->execute();
        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what() << std::endl;
        return false;
    }
}

bool MySQLConnector::VerifyUser(const std::string& Username, const std::string& Password)
{
    try
    {
        std::unique_ptr<sql::PreparedStatement> PreparedStatement(Connection->prepareStatement("SELECT Password FROM Users WHERE Username = ?"));
        PreparedStatement->setString(1, Username);
        std::unique_ptr<sql::ResultSet> ResultSet(PreparedStatement->executeQuery());

        if (ResultSet->next())
        {
            return ResultSet->getString("Password") == Password;
        }
        return false;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what() << std::endl;
        return false;
    }
}
