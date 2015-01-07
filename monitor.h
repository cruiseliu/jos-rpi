#pragma once

/**
 * @brief The kernel shell.
 *
 * The monitor use stdio to communicate with user, and is not interested in
 * the implementation details of stdio. Thus stdio should be set up before
 * starting the shell.
 */

namespace Monitor {
    /// Run the shell.
    void run();

    /// @name Shell commands
    //@{

    /// Show the list of commands.
    int help(int argc, char *argv[]);

    /// Display some address information of the kernel.
    int kerninfo(int argc, char *argv[]);

    /// @brief Display the stack backtrace.
    int backtrace(int argc, char *argv[]);

    int showmap(int argc, char *argv[]);
    int alloc(int argc, char *argv[]);
    int write(int argc, char *argv[]);
    int read(int argc, char *argv[]);
    int map(int argc, char *argv[]);
    int unmap(int argc, char *argv[]);
    //@}
}
