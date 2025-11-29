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
			operand=34,
			operand2=true,
		},
		{
			opcode="SIZE",
			result=34,
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
			operand=33,
			result=true,
			message="bit32 should be true after resize",
		},
	}
}
