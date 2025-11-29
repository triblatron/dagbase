root=
{
	ops=
	{
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
			result=true,
			message="bit0 should be true",
		},
		{
			opcode="TEST_BIT",
			operand=31,
			result=true,
			message="bit31 should be true",
		},
		{
			opcode="TEST_BIT",
			operand=32,
			result=true,
			message="bit32 should be true",
		},
	}
}
