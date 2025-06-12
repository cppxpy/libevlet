
#include <evtlet/evt/evt_cc.hpp>

using namespace evtlet;

class fib_evt : public evt_cc<size_t> {

private:
  const size_t &m_lh, &m_rh, &m_n;

public:
  fib_evt(size_t &lh, size_t &rh, size_t &n)
      : evt_cc<size_t>(), m_lh(lh), m_rh(rh), m_n(n) {}

  fib_evt(const size_t &lh, const size_t &rh, const size_t &n)
      : evt_cc<size_t>(), m_lh(lh), m_rh(rh), m_n(n) {}

  virtual size_t func() {
    const bool rh_nonzero = static_cast<bool>(m_rh);

    std::cout << "[" << m_n << "] func(" << m_lh << ", " << m_rh << ")" << std::endl;

    const size_t vv = rh_nonzero ? m_lh + m_rh : m_lh;
    if (m_n) {
      size_t m = m_n - 1;
      auto nxt = fib_evt(vv, m_lh, m);
      return nxt.get();
    } else {
      return vv;
    }
  }
};

auto main() -> int {
  // not a very computationally expensive example albeit ...
  auto thunk = fib_evt(1, 0, 15);
  auto &vv = thunk.get();
  std::cout << "rslt: " << vv << std::endl;
}
