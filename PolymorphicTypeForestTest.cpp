#include "stdafx.h"
#include "PolymorphicTypeForestTest.h"
#include SHARED_PTR_HEADER

class UnrelatedInterface:
{
public:
  virtual ~UnrelatedInterface(void);
};

class ObjA:
  public Object
{
};

class ObjB:
  public Object
{
};

class ObjB1:
  public ObjB
{
};

class ObjB2:
  public ObjB,
  public UnrelatedInterface
{
};

TEST_F(PolymorphicTypeForestTest, SimpleInsertion) {
  std::shared_ptr<ObjA> pA(new ObjA);
  std::shared_ptr<ObjB> pB(new ObjB);

  // Prime the forest:
  ASSERT_TRUE(forest.AddTree(pA)) << "Failed to add a witness to an empty tree";
  ASSERT_TRUE(forest.AddTree(pB)) << "Failed to add an unrelated witness to a tree";

  // Ensure we can find the items we just added
  std::shared_ptr<ObjA> foundA;
  std::shared_ptr<ObjB> foundB;
  EXPECT_TRUE(forest.Resolve(foundA)) << "Failed to resolve the first-inserted type";
  EXPECT_TRUE(forest.Resolve(foundB)) << "Failed to resolve the second-inserted type";

  // Ensure object identities line up:
  EXPECT_EQ(pA, foundA) << "Failed to resolve object A back to its original pointer";
  EXPECT_EQ(pB, foundB) << "Failed to resolve object B back to its original pointer";
}

TEST_F(PolymorphicTypeForestTest, AncestralCollision) {
  // Prime the forest:
  forest.AddTree(std::make_shared<ObjA>());
  forest.AddTree(std::make_shared<ObjB1>());
  forest.AddTree(std::make_shared<ObjB2>());

  // Attempt the illegal insertion:
  EXPECT_FALSE(forest.AddTree(std::make_shared<ObjB>())) << "An insertion of an ambiguity-creating type was incorrectly allowed";
}

TEST_F(PolymorphicTypeForestTest, DescendantCollision) {
  // Prime the forest:
  forest.AddTree(std::make_shared<ObjA>());
  forest.AddTree(std::make_shared<ObjB>());

  // Attempt the illegal insertion:
  EXPECT_FALSE(forest.AddTree(std::make_shared<ObjB1>())) << "An insertion of an ambiguity-creating type was incorrectly allowed";
}

TEST_F(PolymorphicTypeForestTest, APrioriAmbiguation) {
  // Prime the forest:
  forest.AddTree(std::make_shared<ObjA>());
  
  // Attempt to find type B, which should not resolve yet because we haven't added B:
  std::shared_ptr<ObjB> ptr;
  EXPECT_TRUE(forest.Resolve(ptr)) << "A resolution attempt was considered ambiguous, even though it should have been unresolvable";
  ASSERT_TRUE(ptr == nullptr) << "Was able to resolve a type name that should have been unavailable";

  // Now add an object which will allow the prior resolution to succeed:
  EXPECT_TRUE(forest.AddTree(std::make_shared<ObjB1>())) << "An insertion was not allowed which should have succeeded cleanly";

  // And then, add an object which will make the prior resolution _ambiguous_
  EXPECT_FALSE(forest.AddTree(std::make_shared<ObjB2>())) << "An ambiguating insertion was incorrectly allowed";

  // Attempt the resolution again
  EXPECT_TRUE(forest.Resolve(ptr)) << "Failed to resolve a type which should have been available";
}

TEST_F(PolymorphicTypeForestTest, AmbiguousAncestorRequest) {
  // Prime the forest:
  forest.AddTree(std::make_shared<ObjA>());
  forest.AddTree(std::make_shared<ObjB1>());

  // At this point, resolving B should be possible:
  std::shared_ptr<ObjB> ptr;
  EXPECT_TRUE(forest.Resolve(ptr)) << "Failed to resolve an ancestor of an existing type in the forest";

  // Adding this type should not be possible, now that ObjB resolution has taken place
  // The introduction of this tree would make the prior resolution ambiguous.
  EXPECT_FALSE(forest.AddTree(std::make_shared<ObjB2>())) << "Resolved a type which should not have been resolvable due to existing memos";

  // Attempt to find type B, which should still be resolvable
  EXPECT_TRUE(forest.Resolve(ptr)) << "Was able to resolve a type name that should have been ambiguous";
}

TEST_F(PolymorphicTypeForestTest, GroundResolutionCheck) {
  static_assert(
    std::is_same<
      ground_type_of<Object>::type,
      Object
    >::value,
    "Incorrect identity property on ground_type_of"
  );
  static_assert(
    std::is_same<
    ground_type_of<ObjA>::type,
    Object
    >::value,
    "Failed to match the ground type of an Object-grounded type"
  );

  // Now try to establish the ground of an ungrounded type:
  static_assert(
    std::is_same<
      ground_type_of<std::string>::type,
      std::string
    >::value,
    "Failed to reflect the ground type of a type which has no explicit ground"
  );
}

TEST_F(PolymorphicTypeForestTest, UnrelatedInterfaceTest) {
  // Add the object which has our interface:
  forest.AddTree(std::make_shared<ObjB2>());
  
  // Attempt to resolve based on this interface:
  std::shared_ptr<UnrelatedInterface> urif;
  EXPECT_TRUE(forest.Resolve(urif)) << "An ungrounded interface resolution attempt was incorrectly interpreted as ambiguous";
  EXPECT_TRUE(urif != nullptr) << "Failed to resolve an ungrounded interface on ObjB2";
}