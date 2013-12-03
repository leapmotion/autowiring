#include "stdafx.h"
#include "MarshalingTest.h"
#include "uuid.h"

DECLARE_UUID(EventWithUuid, "6EC2129F-5DD7-43D5-ACB5-864E8BB5D6B4") :
  public virtual EventReceiver
{

}

TEST_F(MarshalingTest, VerifySimpleSerialization) {

}