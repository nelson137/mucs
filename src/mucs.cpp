#include "mucs.hpp"


function<void()> mucs_callback(Mucs& mucs, void(Mucs::*subcmd)()) {
    return bind(mem_fn(subcmd), ref(mucs));
}
