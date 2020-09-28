//
// Setup interface information
//

interface Superhero
{
  uint SaveTheDay();
};

class BatmanTexture2D : Superhero
{
  uint SaveTheDay() 
  { 
	  return tex[uint2(0, 0)];
  }

  Texture2D<uint> tex;
};

class BatmanBuffer : Superhero
{
  uint SaveTheDay() 
  { 
	  return buf[0];
  }

  Buffer<uint> buf; 
};

class SupermanTexture2D : Superhero
{
  uint SaveTheDay() 
  { 
	  return tex[uint2(0, 0)];
  }

  Texture2D<uint> tex; 
};

class SupermanBuffer : Superhero
{
  uint SaveTheDay() 
  { 
	  return buf[0];
  }

  Buffer<uint> buf; 
};

//
// Constant buffer data
//

// Can have 253 instances bound
Superhero heros[253];
uint successSlot;


uint TestEntryPoint( ) : RESOURCE_DATA
{
  return heros[successSlot].SaveTheDay();
}

#include "ResourceIndexing.fx"