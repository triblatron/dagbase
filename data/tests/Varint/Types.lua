root=
{
	--~ selector=
	--~ {
		--~ typeIndex="TYPE_UINT",
		--~ value=7,
	--~ },
	cases=
	{
		{
			valueType="TYPE_UINT8",
			value=0,
			expected=0,
		},
		{
			valueType="TYPE_UINT8",
			value=(1<<8)-1,
			expected=(1<<8)-1,
		},
		{
			valueType="TYPE_INT8",
			value=(1<<7)-1,
			expected=(1<<7)-1,
		},
		{
			valueType="TYPE_UINT16",
			value=(1<<16)-1,
			expected=(1<<16)-1,
		},
		{
			valueType="TYPE_INT16",
			value=(1<<15)-1,
			expected=(1<<15)-1,
		},
		{
			valueType="TYPE_UINT32",
			value=(1<<32)-1,
			expected=(1<<32)-1,
		},
		{
			valueType="TYPE_INT32",
			value=(1<<31)-1,
			expected=(1<<31)-1,
		},
		{
			valueType="TYPE_UINT64",
			-- 2^53 is the limit of precision for integers as doubles
			value=(2^53)-1,
			expected=(2^53)-1,
		},
		{
			valueType="TYPE_UINT64",
			value=(1<<63)-1,
			expected=(1<<63)-1,
		},
		{
			valueType="TYPE_INT64",
			value=(1<<63)-1,
			expected=(1<<63)-1,
		},
	},
}
