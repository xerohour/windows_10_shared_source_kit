#ifndef DEFINES_SET
#define LAYOUT uint4 data0 : Data0;
#endif

struct Vertex
{
	LAYOUT
};

Vertex main( Vertex v )
{
	return v;
}

