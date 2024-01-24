#pragma once

namespace trait
{
    // Disallow copy operations.
    class no_copy
    {
    protected:
        no_copy() = default;
        ~no_copy() = default;
        
        no_copy(const no_copy&) = delete;
        no_copy& operator=(const no_copy&) = delete;

        no_copy(no_copy&&) = default;
        no_copy& operator=(no_copy&&) = default;
    };

    // Disallow copy and move operations.
    class no_copy_or_move
    {
    protected:
        no_copy_or_move() = default;
        ~no_copy_or_move() = default;

        no_copy_or_move(const no_copy_or_move&) = delete;
        no_copy_or_move& operator=(const no_copy_or_move&) = delete;

        no_copy_or_move(no_copy_or_move&&) = delete;
        no_copy_or_move& operator=(no_copy_or_move&&) = delete;
    };
}