/*======================================================================
Vulkan Presentation : VulkanPresentation.cpp
Author:			Sim Luigi
Last Modified:	2020.12.13

2020.12.13: Framework class complete.
=======================================================================*/
#include "VulkanFramework.h"

// ÉÅÉCÉìä÷êî
int main(void)
{
	CVulkanFramework mainProgram;

	try
	{
		mainProgram.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}