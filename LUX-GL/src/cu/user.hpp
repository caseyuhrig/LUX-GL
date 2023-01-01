#include <pch.hpp>


namespace cu {

    

    class CurrentUser
    {
    public:
        static const auto GetIdleDuration()
        {
            const auto ticks = GetIdleMilliseconds();
            const auto milliseconds = std::chrono::milliseconds(ticks);
            const auto duration = std::chrono::duration(milliseconds);
            return duration;
        }

        static const std::uint64_t GetIdleMilliseconds()
        {
            const auto current_tick = GetTickCount64();
            const auto last_tick = GetLastTick();
            const auto difference = current_tick - last_tick;
            return difference;
        }
    private:
        static const bool IsScreenSaverActive()
        {
            bool screensaver_active = false;
            if (!SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &screensaver_active, 0))
            {
                const auto errorMessageID = ::GetLastError();
                throw std::exception(GetLastErrorAsString(errorMessageID).c_str());
            }
        }

        static const std::uint32_t GetLastTick()
        {
            LASTINPUTINFO last_input;
            last_input.cbSize = sizeof(LASTINPUTINFO);

            if (!GetLastInputInfo(&last_input))
            {
                const auto errorMessageID = ::GetLastError();
                spdlog::error("GetLastTick...errorMessageID = {}", errorMessageID);
                std::string message = "GetLastTick: " + GetLastErrorAsString(errorMessageID);
                throw std::exception(message.c_str());
            }
            return last_input.dwTime;
        }

        static const std::string GetLastErrorAsString(const std::uint32_t errorMessageID)
        {
            if (errorMessageID == 0) {
                spdlog::debug("GetLastErrorAsString(errorMessageID = 0)");
                return ""; //No error message has been recorded
            }

            LPSTR messageBuffer = nullptr;

            spdlog::trace("BEFORE FormatMessageA");
            // Ask Win32 to give us the string version of that message ID.
            // The parameters we pass in, tell Win32 to create the buffer that 
            // holds the message for us (because we don't yet know how long the message string will be).
            const auto size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, 
                (DWORD)errorMessageID, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                (LPSTR)&messageBuffer,
                //reinterpret_cast<LPSTR>(&messageBuffer),
                //dynamic_cast<LPSTR>(&messageBuffer),
                0, NULL);
            spdlog::trace("AFTER FormatMessageA");
            //Copy the error message into a std::string.
            const std::string message(messageBuffer, size);

            //Free the Win32's string's buffer.
            LocalFree(messageBuffer);

            return message;
        }
    };
}