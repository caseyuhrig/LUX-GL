#pragma once

//#define SPDLOG_FMT_EXTERNAL

#include <iostream>
#include <string>
#define FMT_EXPORT
#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "fmt/printf.h"
#include <fmt/color.h>

#include "../Utils.hpp"

//#include <type_traits>

// make this a spdlog wrapper

namespace lux {

	class Log
	{
	public:
		Log() {}

		template <typename S, typename... Args>
		static void LOG_DEBUG(const S& format_str, const Args&... args)
		{
			auto now = Utils::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			fmt::printf("%02d:%02d:%02d [DEBUG] %s\n", now.Hour, now.Minute, now.Second, message);
		}

		template <typename S, typename... Args>
		static void LOG_INFO(const S& format_str, const Args&... args)
		{
			auto now = Utils::Time::now();
			std::string message = fmt::sprintf(format_str, args...);		
			std::string level = fmt::format(fmt::fg(fmt::color::blue_violet) | fmt::emphasis::bold, "INFO");
			fmt::printf("%02d:%02d:%02d  [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}


		template <typename S, typename... Args>
		static void LOG_ERROR(const S& format_str, const Args&... args)
		{
			auto now = Utils::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			std::string level = fmt::format(fmt::bg(fmt::color::black) | fmt::fg(fmt::color::red) | fmt::emphasis::bold, "ERROR");
			fmt::printf("%02d:%02d:%02d [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}


		template <typename S, typename... Args>
		static void LOG_FATAL(const S& format_str, const Args&... args)
		{
			auto now = Utils::Time::now();
			std::string message = fmt::sprintf(format_str, args...);
			std::string level = fmt::format(fmt::bg(fmt::color::red) | fmt::fg(fmt::color::black) | fmt::emphasis::bold, "FATAL");
			fmt::printf("%02d:%02d:%02d [%s] %s\n", now.Hour, now.Minute, now.Second, level, message);
		}

	};

}

#define UX_LOG_DEBUG(...) lux::Log::LOG_DEBUG(__VA_ARGS__)
#define UX_LOG_INFO(...) lux::Log::LOG_INFO(__VA_ARGS__)
#define UX_LOG_ERROR(...) lux::Log::LOG_ERROR(__VA_ARGS__)
#define UX_LOG_FATAL(...) lux::Log::LOG_FATAL(__VA_ARGS__)
