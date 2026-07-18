#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_

#include <cstdint>

#include <chrono>
#include <ostream>


//extern "C" {
    namespace pht {
        class Timer;
    }

    /**
     * Overrides the <<-operator to allow printing of Timers. 
     * @param out The stream to print to. 
     * @param timer The timer whose value to print. 
     * @return std::ostream& The stream, for call-chaining. 
     */
    std::ostream &operator<<(std::ostream &out, const pht::Timer &timer);

    namespace pht {
        /**
         * This class implements a simple timer to measure time between two time point. 
         */
        class __declspec(dllexport) Timer {
        public:
            /**
             * Start the time measurement. 
             * This can be called any time, but may produce negative times if called after stop. 
             */
            void start();

            /**
             * Stop the time measurement. 
             * This can be called any time, but may produce negative times if called without calling start. 
             */
            void stop();

            /**
             * Returns the duration between start() and stop(). 
             * @return std::chrono::duration<uint32_t, std::micro> The duration in microseconds. 
             */
            [[nodiscard]] std::chrono::duration<uint32_t, std::micro> getDuration() const;

            /**
             * Converts the duration of this Timer to a human-readable format. 
             * @return std::string The duration as "hours:minuts:seconds.millis_micros". 
             */
            [[nodiscard]] std::string toString() const;

            ///Befriend the print-operator. 
            friend std::ostream &::operator<<(std::ostream &out, const Timer &timer);

        private:
            #ifdef _MSC_VER
            #pragma warning(disable:4251)
            #endif
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> stopTime = std::chrono::high_resolution_clock::now();
            #ifdef _MSC_VER
            #pragma warning(default:4251)
            #endif
        };
    }

//}
#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_TIMER_H_
