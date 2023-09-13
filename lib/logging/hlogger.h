//
// Created by Triage on 9/3/2023.
//

#ifndef HALCYONICUS_HLOGGER_H
#define HALCYONICUS_HLOGGER_H

#include "../tools/timetools.h"
#include <termcolor/termcolor.hpp>
#include <sstream>
#include <fstream>
#include <stack>
#include <chrono>
#include <thread>
#include <chrono>

#define nl std::endl

#define sc HLOGGER::clearScopes()
#define si HLOGGER::scopeIn(__LINE__, __FUNCTION__)
#define so HLOGGER::scopeOut()

namespace hlogger{

    class HLOGGER{
    public:

        struct scope{
            unsigned int line;
            std::string func;
        };

    private:

        static std::stack<scope> scopes;

    public:

        static void setOutputFile(std::string out);
        static void shouldWriteOut(bool shouldWrite);

        static size_t scopeSize();
        static void scopeIn(unsigned int line, std::string function);
        static void scopeOut();
        static void clearScopes();
        static std::string scopeSizeInHex();

        enum LTYPE{
            INFO,
            WARN,
            CHRON,
            ERR,
            CRIT,
            SCOPE
        };

        class LSTREAM{
            LTYPE loggerType;
            char outputLevel = 0;
            bool isWriting = false;
            std::string outFile {""};

        public:
            void setIsWriting(bool shouldWrite){ isWriting = shouldWrite; };
            void setOutFile(std::string file){ outFile = file; };
            void setType(LTYPE type){ loggerType = type; };

            LSTREAM(LTYPE ltype) : loggerType{ltype}{};

            template <typename T>
            std::ostream& operator<<(const T& message);
        };

    };

    std::stack<HLOGGER::scope> HLOGGER::scopes;

    template<typename T>
    std::ostream &HLOGGER::LSTREAM::operator<<(const T &message) {
        //If level specified here is lesser than the level specified within the class setting, do not output or log
        std::string timestamp = TIMETOOLS::getCurrentDateTimeLabel();
        std::stringstream ss;
        std::string type;

        switch (loggerType) {
            case INFO:
                type = "[INFO]:\t";
                break;
            case WARN:
                type = "[WARN]:\t";
                break;
            case CHRON:
                type = "[CHRON]:\t";
                break;
            case ERR:
                type = "[ERR]:\t";
                break;
            case CRIT:
                type = "[CRIT]:\t";
                break;
            case SCOPE:
                type = "[SCP]:\t";
                break;
        }
        ss << timestamp<< "\t[S" << scopeSizeInHex() << "]" << type << message;

        if (isWriting){

            const int maxAttempts = 3; // Maximum number of attempts
            const std::chrono::milliseconds waitTime(10); // Time to wait between attempts

            for (int attempts = 0; attempts < maxAttempts; ++attempts) {
                std::ofstream outputFile(outFile, std::ios::app);
                // Check if the file was opened successfully
                if (outputFile.is_open()) {
                    outputFile << ss.str();
                    switch(loggerType){
                        case SCOPE:
                        case INFO:
                            return std::cout << std::flush << termcolor::reset << ss.str();

                        case WARN:
                            return std::cout << std::flush << termcolor::reset << termcolor::yellow << ss.str() ;

                        case CHRON:
                            return std::cout << std::flush << termcolor::reset << termcolor::bright_blue << ss.str();

                        case ERR:
                            return std::cout << std::flush << termcolor::reset << termcolor::red << ss.str();

                        case CRIT:
                            return std::cout << std::flush << termcolor::reset << termcolor::on_red << termcolor::grey << ss.str();

                    }
                } else {
                    std::cout << std::flush << termcolor::reset << termcolor::yellow << timestamp << "\t[L" << (short) outputLevel << "][WARN]:  Failed to log to file, lock present: "<< this->outFile << termcolor::reset << std::endl;
                    std::this_thread::sleep_for(waitTime);
                }
            }
            std::cout << std::flush << termcolor::reset << termcolor::on_red << termcolor::grey << timestamp << "\t[L" << (short) outputLevel << "][CRIT]:  Could not log to file, lock present: "<< this->outFile << termcolor::reset << std::endl;
        }
        switch(loggerType){
            case SCOPE:
            case INFO:
                return std::cout << std::flush << termcolor::reset << ss.str();

            case WARN:
                return std::cout << std::flush << termcolor::reset << termcolor::yellow << ss.str();

            case CHRON:
                return std::cout << std::flush << termcolor::reset << termcolor::bright_blue << ss.str();

            case ERR:
                return std::cout << std::flush << termcolor::reset << termcolor::red << ss.str();

            case CRIT:
                return std::cout << std::flush << termcolor::reset << termcolor::on_red << termcolor::grey << ss.str();

        }
        return std::cout << termcolor::reset << ss.str() << std::flush;

    }

    extern HLOGGER::LSTREAM info = HLOGGER::LSTREAM(HLOGGER::LTYPE::INFO);
    extern HLOGGER::LSTREAM warn = HLOGGER::LSTREAM(HLOGGER::LTYPE::WARN);
    extern HLOGGER::LSTREAM err = HLOGGER::LSTREAM(HLOGGER::LTYPE::ERR);
    HLOGGER::LSTREAM chron = HLOGGER::LSTREAM(HLOGGER::LTYPE::CHRON);
    extern HLOGGER::LSTREAM crit = HLOGGER::LSTREAM(HLOGGER::LTYPE::CRIT);
    HLOGGER::LSTREAM scp = HLOGGER::LSTREAM(HLOGGER::LTYPE::SCOPE);

    void HLOGGER::setOutputFile(std::string out){
        info.setOutFile(out);
        warn.setOutFile(out);
        err.setOutFile(out);
        chron.setOutFile(out);
        crit.setOutFile(out);
    }

    void HLOGGER::shouldWriteOut(bool shouldWrite) {
        info.setIsWriting(shouldWrite);
        warn.setIsWriting(shouldWrite);
        err.setIsWriting(shouldWrite);
        chron.setIsWriting(shouldWrite);
        crit.setIsWriting(shouldWrite);
    }

    size_t HLOGGER::scopeSize(){
        return scopes.size();
    }

    std::string HLOGGER::scopeSizeInHex(){
        std::stringstream ss;
        ss << std::hex << scopes.size();
        return ss.str();
    }

    void HLOGGER::scopeIn(unsigned int line, std::string function) {
        scope s{line, function};
        scopes.push(s);
        std::string timestamp = TIMETOOLS::getCurrentDateTimeLabel();
        std::cout << std::flush << termcolor::reset << termcolor::cyan << timestamp << "\t+[S" << scopeSizeInHex() << "][SCP]:\tIN: " << s.func << "[" << s.line << "]" << nl << std::flush;
    }

    void HLOGGER::scopeOut() {
        std::string timestamp = TIMETOOLS::getCurrentDateTimeLabel();
        std::cout << std::flush << termcolor::reset << termcolor::cyan << timestamp << "\t-[S" << scopeSizeInHex() << "][SCP]:\tOUT: "<< scopes.top().func << "[" << scopes.top().line << "]" << nl << std::flush;
        scopes.pop();
    }

    void HLOGGER::clearScopes() {
        while (!scopes.empty()) scopes.pop();
    }

    class sw{
        std::chrono::steady_clock::time_point start;
    public:
        sw(){
            start = std::chrono::high_resolution_clock::now();
            chron << "SET" << nl;
        }

        void rs(){
            start = std::chrono::high_resolution_clock::now();
            chron << "RESET" << nl;
        }

        void laprs(){
            const auto finish = std::chrono::high_resolution_clock::now();
            const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
            start = std::chrono::high_resolution_clock::now();
            chron << "FIN, LAP " << ((double)delta)/1000000.0 << "ms"<< nl;
        }

        void lap(){
            const auto finish = std::chrono::high_resolution_clock::now();
            const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
            start = std::chrono::high_resolution_clock::now();
            chron << "LAP " << ((double)delta)/1000000.0 << "ms" << nl;
        }
    };

}


#endif //HALCYONICUS_HLOGGER_H
