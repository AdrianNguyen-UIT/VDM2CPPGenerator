#include "pch.h"
#include "Log.h"

std::shared_ptr<spdlog::logger> Log::s_Logger;
std::vector<Message> Log::msgs;

Log::Log()
{

}

Log::~Log()
{

}

void Log::Init()
{
	auto sink = std::make_shared<my_sink_mt>();
	s_Logger = std::make_shared<spdlog::logger>("loggerssss", sink);
	s_Logger->set_pattern("[%T] %l -> %v");
	s_Logger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> Log::GetLogger()
{
	return Log::s_Logger;
}

void Log::Add(const Message& msg)
{
	msgs.emplace_back(msg);
}

void Log::Clear()
{
	msgs.clear();
}

const std::vector<Message>& Log::GetMessages()
{
	return msgs;
}

Message::Message(const Level& pLevel, const std::string& pMsg)
	: level(pLevel), msg(pMsg)
{
}

void Message::SetMessageLevel(const Level& pLevel)
{
	level = pLevel;
}

void Message::SetMessageString(const std::string& pMsg)
{
	msg = pMsg;
}

const Message::Level& Message::GetMessageLevel() const
{
	return level;
}

const std::string& Message::GetMessageString() const
{
	return msg;
}