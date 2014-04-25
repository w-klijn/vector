#include <gtest/gtest.h>

#include <storage.h>

// basic test that gtest is working and installed
TEST(storage, types) {
    //Storage<T, N, M>
    typedef memory::Storage<float,  16, 1> StorageFloatAoS;
    typedef memory::Storage<double, 16, 1> StorageDoubleAoS;
    typedef memory::Storage<float,  16, 4> StorageFloatAoSoA;
    typedef memory::Storage<double, 16, 4> StorageDoubleAoSoA;

    // verify that the correct type is used for internal storage
    ::testing::StaticAssertTypeEq<float,  StorageFloatAoS::value_type>();
    ::testing::StaticAssertTypeEq<double, StorageDoubleAoS::value_type>();

    // verify that the default vector width of 1 is used
    ::testing::StaticAssertTypeEq<memory::Storage<float,  16>, StorageFloatAoS>();
    ::testing::StaticAssertTypeEq<memory::Storage<double, 16>, StorageDoubleAoS>();
}

TEST(storage, static_values) {
    //Storage<T, N, M>
    typedef memory::Storage<float,  16, 1> StorageFloatAoS;
    typedef memory::Storage<double, 16, 1> StorageDoubleAoS;
    typedef memory::Storage<float,  16, 4> StorageFloatAoSoA;
    typedef memory::Storage<double, 16, 4> StorageDoubleAoSoA;

    // verify that the correct storage dimensions are available at compile time
    // we have to use a temporary variable because googletest will barf on
    // EXPECT_EQ(1, StorageFloatAoS::width); because static const variables that
    // are not separately initialized in a .cpp file violate 
    int w;
    EXPECT_EQ(1,w=StorageFloatAoS::width);
    EXPECT_EQ(1,w=StorageDoubleAoS::width);
    EXPECT_EQ(4,w=StorageFloatAoSoA::width);
    EXPECT_EQ(4,w=StorageDoubleAoSoA::width);

    int s;
    EXPECT_EQ(16,s=StorageFloatAoS::size);
    EXPECT_EQ(16,s=StorageDoubleAoS::size);
    EXPECT_EQ(16,s=StorageFloatAoSoA::size);
    EXPECT_EQ(16,s=StorageDoubleAoSoA::size);

    int n;
    EXPECT_EQ(1*16,s=StorageFloatAoS::number_of_values);
    EXPECT_EQ(1*16,s=StorageDoubleAoS::number_of_values);
    EXPECT_EQ(4*16,s=StorageFloatAoSoA::number_of_values);
    EXPECT_EQ(4*16,s=StorageDoubleAoSoA::number_of_values);
}

// test that the fill function, which sets all values in the underlying
// storage to a constant value.
TEST(storage, fill) {
    typedef memory::Storage<float,  1, 1> S11;
    typedef memory::Storage<float,  1, 4> S14;
    typedef memory::Storage<float,  8, 1> S81;
    typedef memory::Storage<float,  8, 4> S84;

    {
        S11 s;
        s.fill(1.2);
        for(int i=0; i<s.number_of_values; i++){
            EXPECT_EQ(1.2f, s[i]);
        }
    }
    {
        S14 s;
        s.fill(1.2);
        for(int i=0; i<s.number_of_values; i++){
            EXPECT_EQ(1.2f, s[i]);
        }
    }
    {
        S81 s;
        s.fill(1.2);
        for(int i=0; i<s.number_of_values; i++){
            EXPECT_EQ(1.2f, s[i]);
        }
    }
    {
        S84 s;
        s.fill(1.2);
        for(int i=0; i<s.number_of_values; i++){
            EXPECT_EQ(1.2f, s[i]);
        }
    }
}

// test that the equality operator works
TEST(storage, equals) {
    typedef memory::Storage<float,  4, 2> S42;
    typedef memory::Storage<int,  3, 7> S37;

    {
        S42 sfrom;
        for(int i=0; i<sfrom.size; i++){
            for(int j=0; j<sfrom.width; j++){
                sfrom(i,j) = float(i+j);
            }
        }

        S42 sto = sfrom;
        for(int i=0; i<sto.size; i++){
            for(int j=0; j<sto.width; j++){
                EXPECT_EQ(float(i+j), sfrom(i,j));
            }
        }
    }

    {
        S37 sfrom;
        for(int i=0; i<sfrom.size; i++){
            for(int j=0; j<sfrom.width; j++){
                sfrom(i,j) = int(i+j);
            }
        }

        S37 sto = sfrom;
        for(int i=0; i<sto.size; i++){
            for(int j=0; j<sto.width; j++){
                EXPECT_EQ(int(i+j), sfrom(i,j));
            }
        }
    }
}

// test that the equality operator works
TEST(storage, constructors) {
    typedef memory::Storage<double, 4, 2> Sdouble;
    typedef memory::Storage<float,  4, 2> Sfloat;

    Sdouble from(3.1);
    Sdouble to_same(from);
    Sfloat  to_other(from);
}

// test that slice operator works
TEST(storage, slice) {
    typedef memory::Storage<double, 4, 3> AoSoA;
    typedef memory::Storage<double,  4, 1> AoS;

    AoSoA from;
    for(int i=0; i<from.size; i++){
        for(int j=0; j<from.width; j++){
            from(i,j) = float(j);
        }
    }

    AoS to = from(2);
    for(int i=0; i<AoSoA::size; i++)
        EXPECT_EQ(double(2), to[i]);
}
