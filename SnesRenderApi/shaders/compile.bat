cd shaders
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe triangle.vert -o triangle.spv
echo Compiled triangle shader
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe triangle2.vert -o triangle2.spv
echo Compiled triangle2 shader
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe blank.frag -o blank.spv
echo Compiled blank shader
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe color.frag -o color.spv
echo Compiled color shader
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe flat.vert -o flat.spv
echo Compiled flat shader
C:/VulkanSDK/1.2.176.1/Bin/glslc.exe palette.frag -o palette.spv
echo Compiled palette shader