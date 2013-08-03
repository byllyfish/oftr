#include <gtest/gtest.h>
#include "ofp/enetaddress.h"

using namespace ofp;

TEST(enetaddress, valid) 
{
	EnetAddress enet{"01-02-03-04-05-06"};

	EXPECT_TRUE(enet.valid());
	EXPECT_EQ("01-02-03-04-05-06", enet.toString());

	EnetAddress enet2{"01-02-03-04-05-06-07"};		// okay

	EXPECT_TRUE(enet2.valid());
	EXPECT_EQ("01-02-03-04-05-06", enet2.toString());

}


TEST(enetaddress, invalid) 
{
	EnetAddress enet{"01-02-03-04-05"};

	EXPECT_FALSE(enet.valid());
	EXPECT_EQ("00-00-00-00-00-00", enet.toString());
}