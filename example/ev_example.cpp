// C++ example for libev, based on the example program
// from the ev(3) manual page
//
// changes:
// - using ev++.h
// - passing data in the watcher.data field
// - using std::cout for console output
//
#include <ev++.h>
#include <iostream>

// all watcher callbacks have a similar signature
// this callback is called when data is readable on stdin
static void
stdin_cb(EV_P_ ev_io *w, int)
{
    std::string &name = *(static_cast<std::string *>(w->data));
    std::cout << "io ready: " << name.data() << std::endl;
    // for one-shot events, one must manually stop the watcher
    // with its corresponding stop function.
    ev_io_stop(EV_A_ w);

    // this causes all nested ev_run's to stop iterating
    ev_break(EV_A_ EVBREAK_ALL);
}

// another callback, this time for a time-out
static void
timeout_cb(EV_P_ ev_timer *w, int)
{
    std::string &name = *(static_cast<std::string *>(w->data));
    std::cout << "timeout: " << name.data() << std::endl;

    // this causes the innermost ev_run to stop iterating
    ev_break(EV_A_ EVBREAK_ONE);
}

int main(void)
{
    // use the default event loop unless you have special needs
    auto &&loop = ev::default_loop{};

    // initialise an io watcher, then start it
    // this one will watch for stdin to become readable
    auto stdin_watcher = ev::io(loop);
    auto stdin_name = "stdin";
    stdin_watcher.data = static_cast<void *>(&stdin_name);
    stdin_watcher.cb = stdin_cb;
    stdin_watcher.start(0, ev::READ);

    // initialise a timer watcher, then start it
    // simple non-repeating 5.5 second timeout
    auto timeout_watcher = ev::timer(loop);
    auto timeout_name = "timer";
    timeout_watcher.data = static_cast<void *>(&timeout_name);
    timeout_watcher.cb = timeout_cb;
    timeout_watcher.start(5.5);

    // now wait for events to arrive
    loop.run();

    // break was called, so exit
    return 0;
}
