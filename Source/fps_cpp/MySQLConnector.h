// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

class MySQLConnector
{
public:
    MySQLConnector();
    ~MySQLConnector();

    bool Connect();
    void Disconnect();

    bool RegisterUser(const std::string& Username, const std::string& Password);
    bool VerifyUser(const std::string& Username, const std::string& Password);

private:
    sql::mysql::MySQL_Driver* Driver;
    std::unique_ptr<sql::Connection> Connection;
};
