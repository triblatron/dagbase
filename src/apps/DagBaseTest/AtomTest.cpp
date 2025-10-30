//
// Created by Tony Horrobin on 15/04/2025.
//

#include "config/config.h"

#include "core/Atom.h"

#include <gtest/gtest.h>
#include <string>

TEST(Atom, Atom_afterInterningTwoAtomsForTheSameStringCompareEqual)
{
    std::string s1 = "Hello World";
    std::string s2 = "Hello World";

    auto & a1 = dagbase::Atom::intern(s1);
    auto & a2 = dagbase::Atom::intern(s2);

    EXPECT_EQ(a1, a2);
    dagbase::Atom::clear();
}
