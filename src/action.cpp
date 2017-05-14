#include "action.hpp"
#include "grammar.h"

namespace tproc {

bool MinMaxAction::equals(const ActionPtr &other) const {
    auto ptr = std::dynamic_pointer_cast<MinMaxAction>(other);
    if (ptr == nullptr) {
        return false;
    }
    return minOccurences == ptr->minOccurences &&
            maxOccurences == ptr->maxOccurences;
}

bool QuoteAction::equals(const ActionPtr &other) const {
    return typeid (this) == typeid (other.get());
}

}
