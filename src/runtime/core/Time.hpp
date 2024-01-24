#pragma once

#include <chrono>

namespace jaccen
{
    class Time
    {
    public:
        static void endFrame(double elapsed);

        // Last frame time in seconds.
        static float deltaTime();

    private:
        static double m_time;
        static float m_deltaTime;
    };

    class Timer 
    {
    public:
        static Timer start();

        void begin();

        // Reset timer and return elapsed time in milliseconds.
        double reset();
        // Return elapsed time in milliseconds.
        double elapsed() const;

    private:
        std::chrono::steady_clock::time_point m_begin;
    };
}
