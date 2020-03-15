#ifndef __DATABASE_BRIDGE_H__
#define __DATABASE_BRIDGE_H__



#include <iostream>
#include <string>
#include <memory>
#import "C:\Program Files\Common Files\System\ado\msado15.dll"   no_namespace   rename("EOF",   "adoEOF")


namespace AdoSpace
{
	class DatabaseConfig
	{
		//��������
	public:
		DatabaseConfig() {}
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
			database_config.append(LR"(Initial Catalog=)" + database_name_ + LR"(;)");
			database_config.append(LR"(Data Source=)" + database_addr_ + LR"(,)" + std::to_wstring(port_) + LR"(;)");
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

		//ȡ�ó�ʱʱ��
		std::size_t get_time_out()
		{
			return time_out_;
		}

		//�������ݿ��ַ
		void set_database_addr_port(std::wstring database_addr,std::size_t port)
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
			database_config_ = std::forward<DatabaseConfig>(database_config);
		}

		virtual~DatabaseEngine()
		{
			connection_.Release();
			command_.Release();
			recordset_.Release();
		}

		//������
	public:
		//��ʼ�����
		void InitEngine()
		{
			//��ʼ�����
			connection_.CreateInstance(__uuidof(Connection));
			command_.CreateInstance(__uuidof(Command));
			recordset_.CreateInstance(__uuidof(Recordset));

			if (connection_ == nullptr || command_ == nullptr || recordset_ == nullptr)
				std::runtime_error(R"(Component init failed)");
		}

		//���ݿ�������
		void OpenDataBase()
		{
			CloseDataBase();

			//���ö���
			if (database_config_.get_time_out() != 0)
				connection_->put_ConnectionTimeout(database_config_.get_time_out());
			connection_->CursorLocation = adUseClient;
			
			//����ͬ�����ݿ�����
			connection_->Open((_bstr_t)database_config_.get_database_config().c_str(), (_bstr_t)database_config_.get_user_id().c_str(),
				(_bstr_t)database_config_.get_password().c_str(), adConnectUnspecified);

			//������������
			command_->ActiveConnection = connection_;
		}

		//�ر����ݿ�
		void CloseDataBase()
		{
			try
			{
				if (recordset_->GetState() != adStateClosed)
					recordset_->Close();
				
				if (connection_->GetState() != adStateClosed)
					connection_->Close();
			}
			catch (_com_error & error)
			{
				//error.Source();
				//error.Description();
			}
		}

		//ִ�д洢����
		void ExecuteProcess(std::wstring process,bool need_return)
		{
			//�رռ�¼��
			CloseRecordSet();

			//���ò���
			command_->CommandText = process.c_str();
			
			//�����ж�
			if (need_return)
			{
				recordset_->CursorLocation = adUseClient;
				recordset_->Open((IDispatch*)command_, vtMissing, adOpenForwardOnly, adLockReadOnly, adOptionUnspecified);
			}
			else
			{
				command_->Execute(nullptr, nullptr, adExecuteNoRecords);
			}
		}

		//ִ�����
		void ExecuteSentence(std::wstring sentence)
		{
			command_->CommandText = L"";
			recordset_->CursorLocation = adUseClient;
			connection_->Execute(sentence.c_str(), nullptr, adExecuteRecord);
		}

		//�����ǰ��������һ��
		void NextRecordSet()
		{
			_variant_t var_next(0L);
			recordset_->NextRecordset(&var_next);
		}

		//ָ��ָ����һ��
		void MoveToNext()
		{
			recordset_->MoveNext();
		}

		//ָ��ָ���һ��
		void MoveToFirst()
		{
			recordset_->MoveFirst();
		}

		//������ȡ
		void get_parameter(std::wstring parameter_name, _variant_t & get_value)
		{
			try
			{
				get_value = command_->Parameters->Item[parameter_name.c_str()]->Value;
			}
			catch (_com_error error)
			{

			}
		}

		//��ȡֵ
		void get_recordset_value(std::wstring parameter_name, _variant_t& value)
		{
			FieldsPtr field = recordset_->GetFields();
			value = field->GetItem(parameter_name.c_str())->GetValue();
		}

		//���ݿ��������
	public:
		void ResetParameter()
		{
			long parameter_count = command_->Parameters->Count;

			for (auto index = parameter_count; index > 0; index--)
			{
				command_->Parameters->Delete(index - 1);
			}
		}

		void CloseRecordSet()
		{
			if (recordset_->GetState() != adStateClosed)
				recordset_->Close();
		}
	
		//���ݿ��������
	public:
		void addParameterInput(std::wstring name,int value,ParameterDirectionEnum direction = adParamInput)
		{
			this->CreateParameter(name, adInteger, sizeof(int), value, direction);
		}

		void addParameterInput(std::wstring name, long value, ParameterDirectionEnum direction = adParamInput)
		{
			this->CreateParameter(name, adInteger, sizeof(long), value, direction);
		}

		void addParameterInput(std::wstring name, double value, ParameterDirectionEnum direction = adParamInput)
		{
			this->CreateParameter(name, adDouble, sizeof(double), value, direction);
		}

		void addParameterInput(std::wstring name, std::wstring value, ParameterDirectionEnum direction = adParamInput)
		{
			this->CreateParameter(name, adVarChar, value.size(), value.c_str(), direction);
		}

		//���ݿ��������
	public:
		void addParameterOutput(std::wstring name, int value, ParameterDirectionEnum direction = adParamInputOutput)
		{
			this->CreateParameter(name, adInteger, sizeof(int), value, direction);
		}

		void addParameterOutput(std::wstring name, long value, ParameterDirectionEnum direction = adParamInputOutput)
		{
			this->CreateParameter(name, adInteger, sizeof(long), value, direction);
		}

		void addParameterOutput(std::wstring name, double value, ParameterDirectionEnum direction = adParamInputOutput)
		{
			this->CreateParameter(name, adDouble, sizeof(double), value, direction);
		}

		void addParameterOutput(std::wstring name, std::wstring value, ParameterDirectionEnum direction = adParamInputOutput)
		{
			this->CreateParameter(name, adVarChar, value.size(), value.c_str(), direction);
		}

		//��������
	private:
		void CreateParameter(std::wstring name, DataTypeEnum type, std::size_t size, _variant_t value, ParameterDirectionEnum direction)
		{
			_ParameterPtr parameter = command_->CreateParameter((_bstr_t)name.c_str(), type, direction, size, value);
			command_->Parameters->Append(parameter);
		}

		//���ݿ��¼���
	public:
		//�������ݿ�IP�Ͷ˿�
		void set_database_addr_port(std::wstring database_addr, std::size_t port)
		{
			database_config_.set_database_addr_port(database_addr, port);
		}
		//�����û���������
		void set_account_message(std::wstring user_id, std::wstring password)
		{
			database_config_.set_user_id(user_id);
		}
		//���ó�ʱʱ��
		void set_timeout(std::size_t timeout)
		{
			database_config_.set_time_out(timeout);
		}



		//˽�б���
	private:
		DatabaseConfig database_config_;

		//ADO����
		_ConnectionPtr       connection_;
		_CommandPtr          command_;
		_RecordsetPtr        recordset_;
	};
}




#endif // !__DATABASE_BRIDGE_H__
