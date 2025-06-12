

#include <boost/context/continuation.hpp>

#include <evtlet/evt/evt_fiber.hpp>

namespace evtlet {

namespace ctx = boost::context;

using namespace std::placeholders;

template <typename T> class evt_cc : public evt_fiber<T> {

public:
  using evt_fiber<T>::evt_fiber;

  // trivial wrapping for the evt_fiber implementation, with call_cc

  virtual void dispatch() {
    DBGMSG("dispatch_cc ...");
    auto &&ff = std::bind(&evt_cc<T>::dispatch_cc, this, _1);
    ctx::callcc(std::move(ff));
    DBGMSG("... dispatch_cc");
  }

  virtual void wait() {
    DBGMSG("wait_cc ...");
    auto ff = std::bind(&evt_cc<T>::wait_cc, this, _1);
    ctx::callcc(std::move(ff));
    DBGMSG("... wait_cc");
  }

protected:
  virtual ctx::continuation dispatch_cc(ctx::continuation &&sink) {
    evt_fiber<T>::dispatch();
    return std::move(sink);
  }

  virtual ctx::continuation wait_cc(ctx::continuation &&sink) {
    evt_fiber<T>::wait();
    return std::move(sink);
  }
};

} // namespace evtlet