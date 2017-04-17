#include "stack.h"

namespace tproc {

std::set<GSSNodePtr> GSSStateNode::getSucc() const {
    return this->succ;
}

//std::set<GSSNodePtr> GSSStateNode::getPred() const {
//    return this->pred;
//}

std::set<GSSNodePtr> GSSSymbolNode::getSucc() const {
    return this->succ;
}

//std::set<GSSNodePtr> GSSSymbolNode::getPred() const {
//    return this->pred;
//}

std::vector<GSSNodePtr> findAllDestsForPath(const GSSNodePtr &startNode, int pathLength) {
    if (pathLength == 0) {
        return {startNode};
    }

    std::vector<GSSNodePtr> endNodes;
    auto successors = startNode->getSucc();
    for (auto &succ : successors) {
        auto endNodesForCurrentPath = findAllDestsForPath(succ, pathLength - 1);
        endNodes.insert(endNodes.end(), endNodesForCurrentPath.begin(), endNodesForCurrentPath.end());
    }

    return  endNodes;
}


bool isExistsPath(const GSSNodePtr &startNode, const GSSNodePtr &destNode) {
    if (startNode == destNode) {
        return true;
    }
    auto successors = startNode->getSucc();
    for (auto &succ : successors) {
        if (isExistsPath(succ, destNode)) {
            return true;
        }
    }
    return false;
}

}

