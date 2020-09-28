////////////////////////////////////////////////////////////

struct VSOutClear
{
	uint data : data;
};

// Clears a single-component, single-element stream out vertex to MAGIC_NUMBER
VSOutClear VSClear()
{
	VSOutClear output;
	output.data = MAGIC_NUMBER;
	return output;
}


