#ifndef __DATABASE_BRIDGE_H__
#define __DATABASE_BRIDGE_H__

#include <iostream>
#include <string>

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" 
rename("EOF", "AdoEOF")
rename("BOF", "AdoBOF")

namespace AdoSpace
{
	class DatabaseConfig
	{
		//函数定义
	public:
		DatabaseConfig() = delete;
		DatabaseConfig(std::wstring database_addr, std::size_t port, std::wstring user_id, std::wstring passowrd, std::wstring database_name)
		{
			database_name_ = database_name;
			database_addr_ = database_addr;
			port_ = port;

			user_id_ = user_id;
			password_ = passowrd;
		}

		//取得配置文件
		std::wstring get_database_config()
		{
			if (database_addr_ == L"" || port_ == 0 || user_id_ == L"" || password_ == L"")
				throw std::logic_error("Please init config first");
				return L"";

			std::wstring database_config = L"Provider=SQLOLEDB;";
			database_config.append(L"Initial Catalog=" + database_name_ + L";");
			database_config.append(L"Data Source=" + database_addr_ + L"," + std::to_wstring(port_) + L";");
			return database_config;
		}

		//取得地址IP
		std::wstring get_database_addr()
		{
			return database_addr_;
		}
		
		//取得用户名
		std::wstring get_user_id()
		{
			return user_id_;
		}

		//取得密码
		std::wstring get_password()
		{
			return password_;
		}

		//设置数据库地址
		void set_database_addr_port(std::wstring database_addr,size_t port)
		{
			database_addr_ = database_addr;
			port_ = port;
		}

		//设置用户名
		void set_user_id(std::wstring user_id)
		{
			user_id_ = user_id;
		}

		//设置密码
		void set_password(std::wstring password)
		{
			password_ = password;
		}

		//设置超时时间
		void set_time_out(std::size_t time_out)
		{
			time_out_ = time_out;
		}

		//数据成员
	private:
		std::wstring database_name_{L""};        //数据库名字
		std::wstring database_addr_{L""};        //数据库地址
		std::size_t  port_{0};                   //数据库地址

		std::wstring user_id_{L""};              //用户名
		std::wstring password_{L""};             //密码

		std::size_t time_out_{0};                //超时时间
	};


	class DatabaseEngine
	{
		//函数定义
	public:
		DatabaseEngine() = delete;
		DatabaseEngine(DatabaseConfig && database_config)
		{
			database_config_ = std::forward(database_config);
		}

		//设置数据库IP和端口
		void set_database_addr_port(std::wstring database_addr, std::size_t port)
		{
			database_config_.set_database_addr_port(database_addr, port);
		}

		//设置用户名和密码
		void set_account_message(std::wstring user_id, std::wstring password)
		{
			database_config_.set_user_id(user_id);
			database_config_.set_password(password)；
		}

		//设置超时时间
		void set_time_out(std:size_t time_out)
		{
			database_config_.set_time_out(time_out);
		}


		//私有变量
	private:
		DatabaseConfig database_config_;

		//ADO对象
		_ConnectionPtr    connection_;            //连接对象
		_CommandPtr	      command_;               //命令对象
		_RecordSetPtr     recordset_;             //记录集对象
	};










}




#endif // !__DATABASE_BRIDGE_H__
