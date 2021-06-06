#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class Message
{
public:
	enum class Level
	{
		Error,
		Trace,
		Info
	};
private:
	Level level;
	std::string msg;
public:
	Message(const Level& pLevel = Level::Error, const std::string& pMsg = "");

	void SetMessageLevel(const Level& level);
	void SetMessageString(const std::string& pmsg);

	const Level& GetMessageLevel() const;
	const std::string& GetMessageString() const;

};

class Log
{
private:
	static std::shared_ptr<spdlog::logger> s_Logger;
	static std::vector<Message> msgs;

public:
	Log();
	~Log();

	static void Init();
	static std::shared_ptr<spdlog::logger> GetLogger();
	static void Add(const Message& msg);
	static void Clear();

	static const std::vector<Message>& GetMessages();
};

#include "spdlog/sinks/base_sink.h"
template<typename Mutex>
class my_sink : public spdlog::sinks::base_sink <Mutex>
{
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
		Message message;
		message.SetMessageString(fmt::to_string(formatted));
		if (msg.level == spdlog::level::level_enum::err)
		{
			message.SetMessageLevel(Message::Level::Error);
		}
		else if (msg.level == spdlog::level::level_enum::info)
		{
			message.SetMessageLevel(Message::Level::Info);
		}
		else
		{
			message.SetMessageLevel(Message::Level::Trace);
		}
		Log::Add(message);
	}

	void flush_() override
	{
	}
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using my_sink_mt = my_sink <std::mutex>;


#define LOG_TRACE(...)        Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)         Log::GetLogger()->info(__VA_ARGS__)
#define LOG_ERROR(...)        Log::GetLogger()->error(__VA_ARGS__)