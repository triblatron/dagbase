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
			operand=66,
			operand2=true,
		},
		{
			opcode="SIZE",
			result=66,
		},
		{
			opcode="TEST_BIT",
			operand=0,
			result=false,
			message="bit0 should be false after resize",
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
			opcode="TEST_BIT",
			operand=63,
			result=true,
			message="bit63 should be true after resize",
		},
		{
			opcode="TEST_BIT",
			operand=64,
			result=true,
			message="bit64 should be true after resize",
		},
		{
			opcode="TEST_BIT",
			operand=65,
			result=true,
			message="bit65 should be true after resize",
		},
	}
}
