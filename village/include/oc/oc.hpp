// oc/oc.hpp — Umbrella header for the OpenCog Header-Only Library
// Header-only C++11, zero external dependencies, namespace oc::*
// SPDX-License-Identifier: AGPL-3.0
//
// Modules (in dependency order):
//   oc::util       — cogutil:       Logger, RandGen, Config, concurrent_queue, SigSlot
//   oc::atomspace  — atomspace:     Type system, Atom, Node, Link, Handle, TruthValue, AtomSpace
//   oc::attention  — attention:     ECAN, AttentionBank, ImportanceDiffusion, HebbianUpdating
//   oc::server     — cogserver:     CogServer, Shell, SchemeShell, RequestManager, ModuleManager
//   oc::ure        — ure:           Unified Rule Engine, Unifier, ForwardChainer, BackwardChainer
//   oc::pln        — pln:           Probabilistic Logic Networks, deduction, induction, abduction
//   oc::matrix     — matrix:        Sparse vector/matrix, PairAPI, MI, cosine similarity, entropy
//   oc::persist    — atomspace-cog: Serializer, StorageNode, ProxyNode, FrameServer, CogChannel
//
// Usage:
//   #include <oc/oc.hpp>       // everything
//   #include <oc/pln/pln.hpp>  // just PLN (pulls in atomspace + ure + util)
//
// Compile:
//   g++ -std=c++11 -I include -pthread your_file.cpp -o your_binary
//
#ifndef OC_HPP
#define OC_HPP

#include "util/util.hpp"
#include "atomspace/atomspace.hpp"
#include "attention/attention.hpp"
#include "server/server.hpp"
#include "ure/ure.hpp"
#include "pln/pln.hpp"
#include "matrix/matrix.hpp"
#include "persist/persist.hpp"

#endif // OC_HPP
