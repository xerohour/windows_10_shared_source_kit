//
// Setup interface information
//

interface Superhero
{
  int SaveTheDay();
};

class BatmanConstantBuffer : Superhero
{
  int SaveTheDay() 
  { 
	  return cbValue;
  }

  int cbValue;
};

class SupermanConstantBuffer : Superhero
{
  int SaveTheDay() 
  { 
	  return cbValue;
  }

  int cbValue;
};

//
// Constant buffer data
//

// Can have 253 instances bound
Superhero heros[253];

// We want the instance slot in constant buffer 0, offeset 0, everytime.
cbuffer MyBuffer : register( b[0] )
{
	uint successSlot : packoffset(c0);
}

uint TestEntryPoint() : RESOURCE_DATA
{
  return (uint)heros[successSlot].SaveTheDay();
}

#include "ResourceIndexing.fx"