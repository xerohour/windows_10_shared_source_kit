//
// Setup interface information
//

interface Superhero
{
  uint SaveTheDay();
};

class BatmanSampler : Superhero
{
  uint SaveTheDay() 
  { 
	  float2 borderTextureUV = float2(2, 2);
	  return tex.SampleLevel(s, borderTextureUV, 0);
  }

  SamplerState s;
  Texture2D<float> tex;
};

class SupermanSampler : Superhero
{
  uint SaveTheDay() 
  { 
	  float2 borderTextureUV = float2(2, 2);
	  return tex.SampleLevel(s, borderTextureUV, 0);
  }

  SamplerState s;
  Texture2D<float> tex; 
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