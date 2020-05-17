/*
libfive: a CAD kernel for modeling with implicit functions

Copyright (C) 2020  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "catch.hpp"

#include <array>
#include <future>

#include "libfive/tree/simple_tree.hpp"
#include "util/oracles.hpp"

using namespace libfive;

TEST_CASE("SimpleTree: basic operations")
{
    auto t = SimpleTree::X() + 1;
    REQUIRE(t->op() == Opcode::OP_ADD);
    REQUIRE(t->lhs()->op() == Opcode::VAR_X);
    REQUIRE(t->rhs()->op() == Opcode::CONSTANT);
    REQUIRE(t->rhs()->value() == 1);
}

TEST_CASE("SimpleTree::remap()")
{
    auto t = SimpleTree::X();
    auto y = SimpleTree::Y();
    for (unsigned i=0; i < 32768; ++i) {
        t = t + y * i;
    }
    auto z = t.remap(SimpleTree::Z(), SimpleTree::X(), SimpleTree::Y());

    // Make sure the original hasn't changed
    REQUIRE(t->op() == Opcode::OP_ADD);
    REQUIRE(t->lhs()->op() == Opcode::OP_ADD);
    REQUIRE(t->rhs()->op() == Opcode::OP_MUL);
    REQUIRE(t->rhs()->lhs()->op() == Opcode::VAR_Y);
    REQUIRE(t->rhs()->rhs()->op() == Opcode::CONSTANT);
    REQUIRE(t->lhs()->rhs()->lhs()->op() == Opcode::VAR_Y);
    REQUIRE(t->lhs()->rhs()->rhs()->op() == Opcode::CONSTANT);

    // Check that the remapping went through
    REQUIRE(z->op() == Opcode::OP_ADD);
    REQUIRE(z->lhs()->op() == Opcode::OP_ADD);
    REQUIRE(z->rhs()->op() == Opcode::OP_MUL);
    REQUIRE(z->rhs()->lhs()->op() == Opcode::VAR_X);
    REQUIRE(z->rhs()->rhs()->op() == Opcode::CONSTANT);
    REQUIRE(z->lhs()->rhs()->lhs()->op() == Opcode::VAR_X);
    REQUIRE(z->lhs()->rhs()->rhs()->op() == Opcode::CONSTANT);

    auto f = SimpleTree::X();
    f = f * 2 + f * 3 + f;
    REQUIRE(f.size() == 7);

    auto g = f.remap(SimpleTree::Y(), SimpleTree::Y(), SimpleTree::Z());
    REQUIRE(g.size() == 7);

    // TODO: remap in a way that simplifies something
}

TEST_CASE("SimpleTree::size()")
{
    auto x = SimpleTree::X();
    REQUIRE(x.size() == 1);

    auto y = SimpleTree::Y();
    REQUIRE(y.size() == 1);

    auto t = x + y;
    REQUIRE(t.size() == 3);

    auto z = x + x;
    REQUIRE(z.size() == 2);

    auto q = x + SimpleTree::X();
    REQUIRE(q.size() == 2); // X is a singleton
}

TEST_CASE("SimpleTree::unique()")
{
    SECTION("Basic") {
        auto x = SimpleTree::X();
        auto y = SimpleTree::Y();

        auto a = x + y;
        auto b = x + y;

        auto z = a * b;
        REQUIRE(z.size() == 5);

        auto q = z.unique();
        REQUIRE(q.size() == 4);

        auto f = a * b + (a * 2);
        REQUIRE(f.size() == 8);

        auto g = f.unique();
        REQUIRE(g.size() == 7);
    }
    SECTION("With vars") {
        auto a = SimpleTree::var();
        auto c = SimpleTree::var();
        auto b = SimpleTree::var();

        auto t = (a*1 + b*2 + c*3).unique();
        REQUIRE(t.walk().size() == 9);
    }
}


TEST_CASE("SimpleTree::operator<<")
{
    SECTION("Basic")
    {
        std::stringstream ss;
        ss << (SimpleTree::X() + 5);
        REQUIRE(ss.str() == "(+ x 5)");
    }

    SECTION("With oracle")
    {
        std::stringstream ss;
        std::shared_ptr<OracleClause> p(new CubeOracleClause);
        auto o = SimpleTree(p);
        ss << (SimpleTree::X() + 5 + o);
        REQUIRE(ss.str() == "(+ x 5 'CubeOracle)");
    }
}

TEST_CASE("SimpleTree::with_const_vars") {
    auto v = SimpleTree::var();
    auto w = SimpleTree::var();
    auto a = 2 * v + 5 * w;
    auto b = a.with_const_vars();

    {
        std::stringstream ss;
        ss << a;
        REQUIRE(ss.str() == "(+ (* 2 var-free) (* 5 var-free))");

    }
    {
        std::stringstream ss;
        ss << b;
        REQUIRE(ss.str() == "(const-var (+ (* 2 var-free) (* 5 var-free)))");
    }
}
