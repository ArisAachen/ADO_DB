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
		//函数定义
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

		//取得配置文件
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

		//取得超时时间
		std::size_t get_time_out()
		{
			return time_out_;
		}

		//设置数据库地址
		void set_database_addr_port(std::wstring database_addr,std::size_t port)
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
			database_config_ = std::forward<DatabaseConfig>(database_config);
		}

		virtual~DatabaseEngine()
		{
			connection_.Release();
			command_.Release();
			recordset_.Release();
		}

		//组件相关
	public:
		//初始化组件
		void InitEngine()
		{
			//初始化组件
			connection_.CreateInstance(__uuidof(Connection));
			command_.CreateInstance(__uuidof(Command));
			recordset_.CreateInstance(__uuidof(Recordset));

			if (connection_ == nullptr || command_ == nullptr || recordset_ == nullptr)
				std::runtime_error(R"(Component init failed)");
		}

		//数据库操作相关
		void OpenDataBase()
		{
			CloseDataBase();

			//设置对象
			if (database_config_.get_time_out() != 0)
				connection_->put_ConnectionTimeout(database_config_.get_time_out());
			connection_->CursorLocation = adUseClient;
			
			//开启同步数据库连接
			connection_->Open((_bstr_t)database_config_.get_database_config().c_str(), (_bstr_t)database_config_.get_user_id().c_str(),
				(_bstr_t)database_config_.get_password().c_str(), adConnectUnspecified);

			//设置命名连接
			command_->ActiveConnection = connection_;
		}

		//关闭数据库
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

		//执行存储过程
		void ExecuteProcess(std::wstring process,bool need_return)
		{
			//关闭记录集
			CloseRecordSet();

			//设置参数
			command_->CommandText = process.c_str();
			
			//参数判断
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

		//执行语句
		void ExecuteSentence(std::wstring sentence)
		{
			command_->CommandText = L"";
			recordset_->CursorLocation = adUseClient;
			connection_->Execute(sentence.c_str(), nullptr, adExecuteRecord);
		}

		//清除当前，返回下一个
		void NextRecordSet()
		{
			_variant_t var_next(0L);
			recordset_->NextRecordset(&var_next);
		}

		//指针指向下一个
		void MoveToNext()
		{
			recordset_->MoveNext();
		}

		//指针指向第一个
		void MoveToFirst()
		{
			recordset_->MoveFirst();
		}

		//参数获取
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

		//获取值
		void get_recordset_value(std::wstring parameter_name, _variant_t& value)
		{
			FieldsPtr field = recordset_->GetFields();
			value = field->GetItem(parameter_name.c_str())->GetValue();
		}

		//数据库参数重置
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
	
		//数据库参数设置
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

		//数据库参数设置
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

		//公共调用
	private:
		void CreateParameter(std::wstring name, DataTypeEnum type, std::size_t size, _variant_t value, ParameterDirectionEnum direction)
		{
			_ParameterPtr parameter = command_->CreateParameter((_bstr_t)name.c_str(), type, direction, size, value);
			command_->Parameters->Append(parameter);
		}

		//数据库登录相关
	public:
		//设置数据库IP和端口
		void set_database_addr_port(std::wstring database_addr, std::size_t port)
		{
			database_config_.set_database_addr_port(database_addr, port);
		}
		//设置用户名和密码
		void set_account_message(std::wstring user_id, std::wstring password)
		{
			database_config_.set_user_id(user_id);
		}
		//设置超时时间
		void set_timeout(std::size_t timeout)
		{
			database_config_.set_time_out(timeout);
		}



		//私有变量
	private:
		DatabaseConfig database_config_;

		//ADO对象
		_ConnectionPtr       connection_;
		_CommandPtr          command_;
		_RecordsetPtr        recordset_;
	};
}




#endif // !__DATABASE_BRIDGE_H__
