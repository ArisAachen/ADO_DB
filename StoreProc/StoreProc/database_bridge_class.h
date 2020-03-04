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
		//��������
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

		//ȡ�������ļ�
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

		//ȡ�õ�ַIP
		std::wstring get_database_addr()
		{
			return database_addr_;
		}
		
		//ȡ���û���
		std::wstring get_user_id()
		{
			return user_id_;
		}

		//ȡ������
		std::wstring get_password()
		{
			return password_;
		}

		//�������ݿ��ַ
		void set_database_addr_port(std::wstring database_addr,size_t port)
		{
			database_addr_ = database_addr;
			port_ = port;
		}

		//�����û���
		void set_user_id(std::wstring user_id)
		{
			user_id_ = user_id;
		}

		//��������
		void set_password(std::wstring password)
		{
			password_ = password;
		}

		//���ó�ʱʱ��
		void set_time_out(std::size_t time_out)
		{
			time_out_ = time_out;
		}

		//���ݳ�Ա
	private:
		std::wstring database_name_{L""};        //���ݿ�����
		std::wstring database_addr_{L""};        //���ݿ��ַ
		std::size_t  port_{0};                   //���ݿ��ַ

		std::wstring user_id_{L""};              //�û���
		std::wstring password_{L""};             //����

		std::size_t time_out_{0};                //��ʱʱ��
	};


	class DatabaseEngine
	{
		//��������
	public:
		DatabaseEngine() = delete;
		DatabaseEngine(DatabaseConfig && database_config)
		{
			database_config_ = std::forward(database_config);
		}

		//�������ݿ�IP�Ͷ˿�
		void set_database_addr_port(std::wstring database_addr, std::size_t port)
		{
			database_config_.set_database_addr_port(database_addr, port);
		}

		//�����û���������
		void set_account_message(std::wstring user_id, std::wstring password)
		{
			database_config_.set_user_id(user_id);
			database_config_.set_password(password)��
		}

		//���ó�ʱʱ��
		void set_time_out(std:size_t time_out)
		{
			database_config_.set_time_out(time_out);
		}


		//˽�б���
	private:
		DatabaseConfig database_config_;

		//ADO����
		_ConnectionPtr    connection_;            //���Ӷ���
		_CommandPtr	      command_;               //�������
		_RecordSetPtr     recordset_;             //��¼������
	};










}




#endif // !__DATABASE_BRIDGE_H__
