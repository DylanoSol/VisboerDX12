#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>
#include <Windows.h>

Texture::Texture(const char* filePath)
{
	int width, height, nrChannels; 

	// Load texture file
	unsigned char* data = stbi_load(filePath, & width, & height, & nrChannels, 0);
	if (data)
	{
		OutputDebugStringA("Texture has succesfully been loaded \n");
	}
	else
	{
		OutputDebugStringA("Could not be loaded \n"); 
	}

	// Make sure to clear up texture memory
	stbi_image_free(data); 
}
