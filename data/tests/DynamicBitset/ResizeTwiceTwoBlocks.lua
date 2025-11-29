root=
{
	ops=
	{
		{
			opcode="PUSH_BACK",
			operand=false,
			result=1,
		},
		{
			opcode="TEST_BIT",
			operancd=0,
			result=false,
			message="bit0 should be false before resize",
		},
		{
			opcode="RESIZE",
			operand=33,
			operand2=true,
		},
		{
			opcode="SIZE",
			result=33,
		},
		{
			opcode="TEST_BIT",
			operand=0,
			result=false,
			message="bit0 should be true after resize",
		},
		{
			opcode="TEST_BIT",
			operand=31,
			result=true,
			message="bit31 should be true after resize",
		},
		{
			opcode="TEST_BIT",
			operand=32,
			result=true,
			message="bit32 should be true after resize",
		},
		{
			opcode="RESIZE",
			operand=65,
			operand2=false,
		},
		{
			opcode="SIZE",
			result=65,
		},
		{
			opcode="TEST_BIT",
			operand=32,
			result=true,
			message="bit32 should be true after second resize",
		},
		{
			opcode="TEST_BIT",
			operand=33,
			result=false,
			message="bit33 should be false after second resize",
		},
		{
			opcode="TEST_BIT",
			operand=63,
			result=false,
			message="bit63 should be false after second resize",
		},
		{
			opcode="TEST_BIT",
			operand=64,
			result==false,
			message="bit64 should be false after second resize",
		},
	}
}
